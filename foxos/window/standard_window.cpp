#include <foxos/window.h>

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

    this->calculate_buffer_size();
	old_frame = (uint32_t*) malloc(this->buffer_size * sizeof(uint32_t));

    this->set_title(title);
}

standard_foxos_window_t::~standard_foxos_window_t() {
    if (this->buffer) { //Only free if the buffer is allocated
        free(this->buffer);
    }

	free(old_frame);
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

int standard_foxos_window_t::add_button(foxos_button_callback_t callback, int x, int y, int width, int height) {
	for (int i = 0; i < MAX_WINDOW_BUTTONS; i++) {
		if (this->buttons[i] == NULL) {
			this->buttons[i] = new foxos_button(x, y, width, height, callback);
			return i;
		}
	}

	return -1;
}

void standard_foxos_window_t::remove_button(int button_id) {
	delete this->buttons[button_id];
	this->buttons[button_id] = nullptr;
}


void standard_foxos_window_t::all_buttons_draw_outline(graphics_buffer_info_t* info, uint32_t color) {
	for (int i = 0; i < MAX_WINDOW_BUTTONS; i++) {
		if (this->buttons[i] != NULL) {
			this->buttons[i]->draw_outline(color, info);
		}
	}
}

void standard_foxos_window_t::all_buttons_call_callback_if_necessary(int mouse_x, int mouse_y, int mouse_button) {
	for (int i = 0; i < MAX_WINDOW_BUTTONS; i++) {
		if (this->buttons[i] != NULL) {
			if (mouse_x >= this->buttons[i]->x && mouse_x <= this->buttons[i]->x + this->buttons[i]->width && mouse_y >= this->buttons[i]->y && mouse_y <= this->buttons[i]->y + this->buttons[i]->height) {
				this->buttons[i]->callback(mouse_button);
			}
		}
	}
}
