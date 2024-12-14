#ifndef HX_BASIC
#define HX_BASIC

#include <malloc.h>
#include <stdio.h>
#include <string.h>

typedef long long hx_sint;
typedef unsigned long long hx_uint;
typedef char hx_schr;
typedef unsigned char hx_byte;
typedef wchar_t hx_char; // Do not be confused with hx_schr and hx_byte. 
typedef void* hx_ptr;
typedef hx_uint hx_custom_func(hx_ptr*, hx_ptr);

#define CLASS(NAME) typedef struct NAME NAME; struct NAME
#define ENUM(NAME) typedef enum NAME NAME; enum NAME
#define UNION(NAME) typedef union NAME NAME; union NAME

#define MIN(A, B) ((A<B)?(A):(B))
#define MAX(A, B) ((A>B)?(A):(B))

ENUM(hx_bool) { false_ = 0, true_ = 1 };

#ifdef HX_MEM_DBG
void* hx_mem_alloc(hx_uint size);
void hx_mem_free(void* block);
#define ALLOC(TYPE) hx_mem_alloc(sizeof(TYPE))
#define NEWLIST(TYPE, CNT) hx_mem_alloc(sizeof(TYPE)*CNT)
#define FREE(PTR) hx_mem_free(PTR)
#else
void* hx_mem_alloc(hx_uint size);
#define ALLOC(TYPE) hx_mem_alloc(sizeof(TYPE))
#define NEWLIST(TYPE, CNT) hx_mem_alloc(sizeof(TYPE)*CNT)
#define FREE(PTR) free(PTR)
#endif

CLASS(hx_node) {
	hx_node* prev;
	hx_ptr value;
	hx_node* next;
};
CLASS(hx_list) {
	hx_node* head;
	hx_node* tail;
	hx_uint size;
};
hx_list*hx_list_new();
hx_ptr*	hx_list_add_before(hx_list* this, hx_uint index);
hx_ptr	hx_list_pop(hx_list* this, hx_uint index);
void	hx_list_delete(hx_list* this, hx_custom_func* custom_func, hx_ptr client_data);

#endif