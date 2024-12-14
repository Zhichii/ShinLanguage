#ifndef HX_OBJECT
#define HX_OBJECT

#include "basic.h"

ENUM(hx_access) {
	HX_ACCESS_PUBLIC,
	HX_ACCESS_PRIVATE, // Warning only on use outside class. Do not be confused with HX_ACCESS_FORCE_PRIVATE. 
	HX_ACCESS_FORCE_PRIVATE // Error on use outside class. Do not be confused with HX_ACCESS_PRIVATE. 
};

CLASS(hx_object);
CLASS(hx_tag) {
	hx_bool is_static;
	hx_bool is_mutable;
	hx_access access;
	hx_char name[128];
};
CLASS(hx_object) {
	hx_byte* custom;
	hx_uint size;
	hx_object* type;
	hx_custom_func* __init;
	hx_custom_func* __repr;
	hx_custom_func* __bytes;
	hx_custom_func* __call;
	hx_custom_func* __new;
	hx_custom_func* __delete;
};
hx_object*	hx_object_new();
void		hx_object_realloc(hx_object* this, hx_uint size);
void		hx_object_set(hx_object* this, hx_object* that);
void		hx_object_delete(hx_object* this);

#endif