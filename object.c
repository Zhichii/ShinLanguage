#include "object.h"

hx_object*	hx_object_new() {
	hx_object* this = ALLOC(hx_object);
	if (!this) return NULL;
	this->custom = NULL;
	this->size = 0;
	this->type = NULL;
	this->__bytes = NULL;
	this->__call = NULL;
	this->__delete = NULL;
	this->__init = NULL;
	this->__new = NULL;
	this->__repr = NULL;
	return this;
}
void		hx_object_realloc(hx_object* this, hx_uint size) {
	if (!this) return;
	if (size == 0) {
		this->size = size;
		if (this->custom) FREE(this->custom);
		this->custom = NULL;
		return;
	}
	hx_byte* temp = NEWLIST(hx_byte, size);
	if (!temp) return;
	if (this->size) {
		memcpy(temp, this->custom, MIN(this->size, size));
		FREE(this->custom);
	}
	this->custom = temp;
	this->size = size;
}
void		hx_object_set(hx_object* this, hx_object* that) {
	if (!this) return;
	if (!that) return;
	if (this == that) return;
	hx_object_realloc(this, that->size);
	if (this->size) memcpy(this->custom, that->custom, that->size);
	this->__bytes = that->__bytes;
	this->__call = that->__call;
	this->__delete = that->__delete;
	this->__init = that->__init;
	this->__new = this->__new;
	this->__repr = this->__repr;
}
void		hx_object_delete(hx_object* this) {
	if (!this) return;
	if (!this->__delete) {
		FREE(this->custom);
		return;
	}
	this->__delete(this, NULL);
}