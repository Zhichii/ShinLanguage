#include "basic.h"

#ifdef SH_MEM_DBG
void* sh_mem_alloc(sh_uint size) {
	printf("[");
	void* block = malloc(size);
	if (block) memset(block, 0, size);
	return block;
}
void sh_mem_free(void* block) {
	printf("]");
	free(block);
}
#else
void* sh_mem_alloc(sh_uint size) {
	void* block = malloc(size);
	if (block) memset(block, 0, size);
	return block;
}
#endif

sh_list* sh_list_new() {
	sh_list* this = ALLOC(sh_list);
	if (!this) return NULL;
	this->head = NULL;
	this->tail = NULL;
	this->size = 0;
	return this;
}
sh_ptr* sh_list_add_before(sh_list* this, sh_uint index) {
	if (!this) return NULL;
	if (index > this->size) index = this->size;
	sh_node* node = ALLOC(sh_node);
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
	sh_uint i = 0;
	sh_node* cur;
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
sh_ptr	sh_list_pop(sh_list* this, sh_uint index) {
	if (!this) return NULL;
	if (index > this->size) index = this->size;
	if (this->head == NULL) return NULL;
	sh_ptr res;
	if (this->head == this->tail) {
		res = this->head->value;
		FREE(this->head);
		this->head = NULL;
		this->tail = NULL;
		this->size = 0;
		return res;
	}
	sh_uint i = 0;
	sh_node* cur;
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
void	sh_list_delete(sh_list* this, sh_custom_func* custom_func, sh_ptr client_data) {
	if (!this) return;
	sh_node* cur;
	for (cur = this->head; cur != NULL; ) {
		if (custom_func) custom_func(&cur->value, client_data);
		sh_node* cur_next = cur->next;
		FREE(cur);
		cur = cur_next;
	}
	FREE(this);
}
