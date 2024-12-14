#define HX_MEM_DBG

#include "hxlang.h"

int main() {
	hx_object* obj = hx_object_new();
	hx_object_realloc(obj, 114);
	memcpy(obj->custom, "1145141919810", sizeof("1145141919810"));
	hx_object* obj2 = hx_object_new();
	hx_object_set(obj2, obj);
	hx_object_delete(obj);
	hx_object_delete(obj2);
	return 0;
}