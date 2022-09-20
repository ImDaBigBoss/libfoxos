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
			this->length = 0;
			this->free_segments = 0;

			this->current_id = 1;
			this->data = 0;
		}

		list_t(int initial_length) {
			this->length = initial_length;
			this->free_segments = initial_length;

			this->current_id = 1;
			this->data = (node*) malloc(initial_length * sizeof(node));
			memset(this->data, 0, initial_length * sizeof(node));
		}

		~list_t() {
			if (this->data) {
				free(this->data);
			}
		}

		int add(T d) {
			node* new_node;

			if (this->free_segments > 0) {
				for (int i = 0; i < this->length; i++) {
					if (!this->data[i].id) { //Free segment found
						new_node = &this->data[i];
						this->free_segments--;
						break;
					}
				}
			} else {
				this->length++;

				this->data = (node*) realloc((void*) this->data, sizeof(node) * this->length);
				new_node = &this->data[this->length - 1];
			}

			memset(new_node, 0, sizeof(node));
			new_node->data = d;
			new_node->id = this->current_id++;

			return new_node->id;
		}

		template <typename Y>
		node* find(bool (*f)(Y, node*), Y arg) {
			for (size_t i = 0; i < this->length; i++) {
				if (this->data[i].id) { //Used segment
					if (f(arg, &this->data[i])) {
						return &this->data[i];
					}
				}
			}
			
			return nullptr;
		}

		void foreach(void (*f)(node*, void* lambda_data), void* lambda_data) {
			for (size_t i = 0; i < this->length; i++) {
				if (this->data[i].id) { //Used segment
					f(&this->data[i], lambda_data);
				}
			}
		}

		void foreach(void (*f)(node*)) {
			for (size_t i = 0; i < this->length; i++) {
				if (this->data[i].id) { //Used segment
					f(&this->data[i]);
				}
			}
		}

		bool remove_index(int index) {
			if (index < 0 || index >= this->length) {
				return false;
			}

			if (!this->data[index].id) { //Free segment
				return false;
			}

			memset(&this->data[index], 0, sizeof(node));

			this->free_segments++;

			return true;
		}

		bool remove(int id) {
			for (size_t i = 0; i < this->length; i++) {
				if (this->data[i].id == id) {
					return remove_index(i);
				}
			}
			
			return false;
		}

		bool remove(node* n) {
			for (size_t i = 0; i < this->length; i++) {
				if (&this->data[i] == n) {
					return remove_index(i);
				}
			}

			return false;
		}

		int get_length() {
			return this->length - this->free_segments;
		}

		node* get(int index) {
			if (index < 0 || index >= this->length - this->free_segments) {
				return nullptr;
			}

			int tmp = 0;
			for (int i = 0; i < this->length; i++) {
				if (this->data[i].id) { //Used segment
					if (tmp == index) {
						return &this->data[i];
					}
					tmp++;
				}
			}

			return nullptr;
		}
	
	private:
		int length;
		int free_segments;

		int current_id;
		node* data;
};
