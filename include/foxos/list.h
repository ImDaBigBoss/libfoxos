#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

template <typename T>
class list_t {
	public:
		struct node {
			int id;
			T data;
		};

		list_t() {
			length = 0;
			current_id = 0;
            data = 0;
		};

		~list_t() {
            if (data) {
			    free(data);
            }
		}

		template <typename Y>
		node* find(bool (*f)(Y, node*), Y arg) {
			for (size_t i = 0; i < length; i++) {
				if (f(arg, &data[i])) {
					return &data[i];
				}
			}
			
			return nullptr;
		}

		void foreach(void (*f)(node*, void* lambda_data), void* lambda_data) {
			for (size_t i = 0; i < length; i++) {
				f(&data[i], lambda_data);
			}
		}

		void foreach(void (*f)(node*)) {
			for (size_t i = 0; i < length; i++) {
				f(&data[i]);
			}
		}

		bool remove_index(int index) {
			if (index < 0 || index >= length) {
				return false;
			}

			length--;
			if (length == 0) { //If there are no more nodes, then just free the list
				free(data);
				data = 0;
			} else { //If there are still nodes, then reallocate the list
				node* new_data = (node*) malloc(sizeof(node) * length);

				int current_id = 0;
				for (int i = 0; i < length + 1; i++) {
					if (i != index) {
						new_data[current_id] = data[i];
						current_id++;
					}
				}

				free(data);
				data = new_data;
			}

			return true;
		}

		bool remove(int id) {
			for (size_t i = 0; i < length; i++) {
				if (data[i].id == id) {
					return remove_index(i);
				}
			}
			
			return false;
		}

		bool remove(node* n) {
			for (size_t i = 0; i < length; i++) {
				if (&data[i] == n) {
					return remove_index(i);
				}
			}

			return false;
		}

		int add(T d, int index) {
			length++;
			if (index < 0 || index >= length) {
				return -1;
			}

			data = (node*) realloc((void*) data, sizeof(node) * length);
			if (!data) {
				return -1;
			}
			
			node* new_node;

			if (index == length - 1) {
				new_node = &data[index];
			} else {
				for (int i = index + 1; i < length; i++) {
					data[i] = data[i - 1];
				}

				new_node = &data[index];
			}

			memset(new_node, 0, sizeof(node));
			new_node->data = d;
			new_node->id = current_id++;

			return new_node->id;
		}

		int add(T d) {
			return add(d, length); //Add to end
		}

        int get_length() {
            return length;
        }

		node* get(int index) {
			if (index < 0 || index >= length) {
				return nullptr;
			}

			return &data[index];
		}
	
    private:
		int length;
		int current_id;
		node* data;
};
