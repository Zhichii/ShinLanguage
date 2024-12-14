#include "basic.h"

#ifdef HX_MEM_DBG
void* hx_mem_alloc(hx_uint size) {
	printf("[");
	void* block = malloc(size);
	if (block) memset(block, 0, size);
	return block;
}
void hx_mem_free(void* block) {
	printf("]");
	free(block);
}
#else
void* hx_mem_alloc(hx_uint size) {
	void* block = malloc(size);
	if (block) memset(block, 0, size);
	return block;
}
#endif

hx_list*hx_list_new() {
	hx_list* this = ALLOC(hx_list);
	if (!this) return NULL;
	this->head = NULL;
	this->tail = NULL;
	this->size = 0;
	return this;
}
hx_ptr*	hx_list_add_before(hx_list* this, hx_uint index) {
	if (!this) return NULL;
	if (index > this->size) index = this->size;
	hx_node* node = ALLOC(hx_node);
	if (!node) return NULL;
	node->prev = NULL;
	node->value = NULL;
	node->next = NULL;
	if (this->head == NULL) {
		this->head = node;
		this->tail = node;
		this->size++;
		return &node->value;
	}
	if (index == this->size) {
		this->tail->next = node;
		node->prev = this->tail;
		this->tail = node;
		this->size++;
		return &node->value;
	}
	hx_uint i = 0;
	hx_node* cur;
	for (cur = this->head; cur != NULL; cur = cur->next) {
		if (i == index) break;
		i++;
	}
	node->prev = cur->prev;
	node->next = cur;
	if (cur->prev) {
		cur->prev->next = node;
		cur->prev = node;
	}
	if (cur == this->head) this->head = cur;
	this->size++;
	return &node->value;
}
hx_ptr	hx_list_pop(hx_list* this, hx_uint index) {
	if (!this) return NULL;
	if (index > this->size) index = this->size;
	if (this->head == NULL) return NULL;
	hx_ptr res;
	if (this->head == this->tail) {
		res = this->head->value;
		FREE(this->head);
		this->head = NULL;
		this->tail = NULL;
		this->size = 0;
		return res;
	}
	hx_uint i = 0;
	hx_node* cur;
	for (cur = this->head; cur != NULL; cur = cur->next) {
		if (i == index) break;
		i++;
	}
	if (cur->prev) cur->prev->next = cur->next;
	if (cur->next) cur->next->prev = cur->prev;
	if (cur == this->head) this->head = cur->next;
	if (cur == this->tail) this->tail = cur->prev;
	res = cur->value;
	FREE(cur);
	this->size--;
	return res;
}
void	hx_list_delete(hx_list* this, hx_custom_func* custom_func, hx_ptr client_data) {
	if (!this) return;
	hx_node* cur;
	for (cur = this->head; cur != NULL; ) {
		if (custom_func) custom_func(&cur->value, client_data);
		hx_node* cur_next = cur->next;
		FREE(cur);
		cur = cur_next;
	}
	FREE(this);
}
