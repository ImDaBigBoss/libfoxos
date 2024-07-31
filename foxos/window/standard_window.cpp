#include <foxos/window.h>

#include <sys/env.h>

#include <stdint.h>
#include <stdbool.h>

standard_foxos_window_t::standard_foxos_window_t(int64_t x, int64_t y, int64_t width, int64_t height, char* title) {
    int hid = ipc_get_hid(standard_foxos_de_ipc_name);
    if (hid == -1) {
        memset(&window_info, 0, sizeof(standard_foxos_window_info_t));
    } else {
        ipc_send_message(hid, IPC_GET_WINDOW_INFO, (void*) &this->window_info);
    }

    this->window_x = x;
    this->window_y = y;
    this->window_width = width;
    this->window_height = height;

    this->frame_ready = false;

    this->calculate_buffer_size();
	old_frame = (uint32_t*) malloc(this->buffer_size * sizeof(uint32_t));

    this->set_title(title);

    this->click_callback_list = new list_t<foxos_click_callback_list_node_t>();
    this->special_key_down_callback_list = new list_t<foxos_special_key_down_callback_t>();
    this->special_key_up_callback_list = new list_t<foxos_special_key_up_callback_t>();
}

standard_foxos_window_t::~standard_foxos_window_t() {
    if (this->buffer) { //Only free if the buffer is allocated
        free(this->buffer);
    }

	free(old_frame);

    delete this->click_callback_list;
    delete this->special_key_down_callback_list;
    delete this->special_key_up_callback_list;
}

int64_t standard_foxos_window_t::get_x() {
    return this->window_x;
}

int64_t standard_foxos_window_t::get_y() {
    return this->window_y;
}

int64_t standard_foxos_window_t::get_width() {
    return this->window_width;
}

int64_t standard_foxos_window_t::get_height() {
    return this->window_height;
}

int64_t standard_foxos_window_t::get_buffer_x() {
    return this->buffer_x;
}

int64_t standard_foxos_window_t::get_buffer_y() {
    return this->buffer_y;
}

int64_t standard_foxos_window_t::get_buffer_width() {
    return this->buffer_width;
}

int64_t standard_foxos_window_t::get_buffer_height() {
    return this->buffer_height;
}

void standard_foxos_window_t::move(int64_t x, int64_t y) {
    this->window_x = x;
    this->window_y = y;
}

void standard_foxos_window_t::resize(int64_t width, int64_t height) {
    if (width < this->window_info.min_width) {
        this->window_width = this->window_info.min_width;
    } else {
        this->window_width = width;
    }

    if (height < this->window_info.min_height) {
        this->window_height = this->window_info.min_height;
    } else {
        this->window_height = height;
    }

    this->calculate_buffer_size(); //Recalculate the buffer size
}

void standard_foxos_window_t::set_title(char* title) {
    if (this->window_title) {
        free(this->window_title);
    }

    this->title_length = strlen(title);

    this->window_title = (char*) malloc(this->title_length + 1); //Allocate the title and copy the data
    memset(this->window_title, 0, this->title_length + 1);
    strcpy(this->window_title, title);
}

char* standard_foxos_window_t::get_title() {
    return this->window_title;
}

graphics_buffer_info_t standard_foxos_window_t::get_buffer_info() {
    graphics_buffer_info_t buffer_info;
	buffer_info.buffer = this->buffer;
	buffer_info.width = this->buffer_width;
	buffer_info.height = this->buffer_height;
    buffer_info.pitch = this->buffer_width * 4;
    buffer_info.buffer_size = this->buffer_size;
	buffer_info.frame_ready = &this->frame_ready;

    return buffer_info;
}

uint8_t standard_foxos_window_t::get_title_length() {
    return this->title_length;
}

void standard_foxos_window_t::calculate_buffer_size() {
    this->buffer_width = this->window_width - 2;
    this->buffer_height = this->window_height - this->window_info.buffer_height_diff;

    size_t new_size = this->buffer_width * this->buffer_height; //Check to see if the size has changed
    if (new_size == this->buffer_size) {
        return;
    }
    this->buffer_size = this->buffer_width * this->buffer_height;

    if (this->buffer) { //Make we free any existing buffer
        free(this->buffer);
    }

    this->buffer = (uint32_t*) malloc(sizeof(uint32_t) * this->buffer_size);
    if (this->buffer) {
        for (uint32_t i = 0; i < this->buffer_size; i++) {
            this->buffer[i] = this->window_info.background_colour; //This probably should use previous data if there was any
        }
    }
}

int standard_foxos_window_t::add_click_listener(foxos_click_callback_t callback) {
    foxos_click_callback_list_node_t node;
    memset(&node, 0, sizeof(foxos_click_callback_list_node_t));

    node.entire_window = true;
    memcpy(&node.callback, &callback, sizeof(foxos_click_callback_t));

    return this->click_callback_list->add(node);
}

int standard_foxos_window_t::add_click_listener(foxos_click_callback_t callback, int64_t x, int64_t y, int64_t width, int64_t height) {
    foxos_click_callback_list_node_t node;
    memset(&node, 0, sizeof(foxos_click_callback_list_node_t));

    node.entire_window = false;
    memcpy(&node.callback, &callback, sizeof(foxos_click_callback_t));

    node.x = x;
    node.y = y;
    node.width = width;
    node.height = height;

    return this->click_callback_list->add(node);
}

bool standard_foxos_window_t::remove_click_listener(int id) {
    return this->click_callback_list->remove(id);
}

void standard_foxos_window_t::send_click(int64_t mouse_x, int64_t mouse_y, mouse_buttons_e mouse_button) {
    if (mouse_button < 1 || mouse_button > 3) {
        return;
    }

    struct lambda_callback_data_t {
        int64_t mouse_x;
        int64_t mouse_y;
        mouse_buttons_e mouse_button;
    };

    lambda_callback_data_t data;
    data.mouse_x = mouse_x;
    data.mouse_y = mouse_y;
    data.mouse_button = mouse_button;

    this->click_callback_list->foreach([](list_t<foxos_click_callback_list_node_t>::node* n, void* lambda_data) {
        foxos_click_callback_list_node_t* node = &n->data;
        lambda_callback_data_t* data = (lambda_callback_data_t*) lambda_data;

        if (node->entire_window) {
            node->callback(data->mouse_x, data->mouse_y, data->mouse_button);
        } else {
            if (data->mouse_x >= node->x && data->mouse_x <= node->x + node->width &&
                data->mouse_y >= node->y && data->mouse_y <= node->y + node->height) {
                node->callback(data->mouse_x, data->mouse_y, data->mouse_button);
            }
        }
    }, (void*) &data);
}

int standard_foxos_window_t::add_special_key_down_listener(foxos_special_key_down_callback_t callback) {
    return this->special_key_down_callback_list->add(callback);
}

bool standard_foxos_window_t::remove_special_key_down_listener(int id) {
    return this->special_key_down_callback_list->remove(id);
}

void standard_foxos_window_t::send_special_key_down(special_key_e key) {
    this->special_key_down_callback_list->foreach([](list_t<foxos_special_key_down_callback_t>::node* n, void* lambda_data) {
        n->data(*((special_key_e*) lambda_data));
    }, (void*) &key);
}

int standard_foxos_window_t::add_special_key_up_listener(foxos_special_key_up_callback_t callback) {
    return this->special_key_up_callback_list->add(callback);
}

bool standard_foxos_window_t::remove_special_key_up_listener(int id) {
    return this->special_key_up_callback_list->remove(id);
}

void standard_foxos_window_t::send_special_key_up(special_key_e key) {
    this->special_key_up_callback_list->foreach([](list_t<foxos_special_key_up_callback_t>::node* n, void* lambda_data) {
        n->data(*((special_key_e*) lambda_data));
    }, (void*) &key);
}

special_keys_down_t* get_special_keys_down() {
    return (special_keys_down_t*) env(ENV_GET_SPECIAL_KEYS);
}
