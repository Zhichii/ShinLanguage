#ifndef SHINC_BASIC
#define SHINC_BASIC

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

typedef long long sh_sint;
typedef unsigned long long sh_uint;
typedef char sh_schr;
typedef unsigned char sh_byte;
typedef wchar_t sh_char; // Do not be confused with sh_schr and sh_byte. 
typedef void* sh_ptr;
typedef sh_uint sh_custom_func(sh_ptr*, sh_ptr);

#define CLASS(NAME) typedef struct NAME NAME; struct NAME
#define ENUM(NAME) typedef enum NAME NAME; enum NAME
#define UNION(NAME) typedef union NAME NAME; union NAME

#define MIN(A,B) ((A<B)?(A):(B))
#define MAX(A,B) ((A>B)?(A):(B))

ENUM(bool) { false_ = 0, true_ = 1 };

#ifdef SH_MEM_DBG
void* sh_mem_alloc(sh_uint size);
void sh_mem_free(void* block);
#define ALLOC(TYPE) sh_mem_alloc(sizeof(TYPE))
#define NEWLIST(TYPE, CNT) sh_mem_alloc(sizeof(TYPE)*CNT)
#define FREE(PTR) sh_mem_free(PTR)
#else
void* sh_mem_alloc(sh_uint size);
#define ALLOC(TYPE) sh_mem_alloc(sizeof(TYPE))
#define NEWLIST(TYPE, CNT) sh_mem_alloc(sizeof(TYPE)*CNT)
#define FREE(PTR) free(PTR)
#endif

#define STRLEN(N) wcslen(N)
#define NEWSTR(CNT) NEWLIST(sh_char,(CNT+1))
#define STRCPY(N,M) wcscpy(N,M)
#define STRSAME(N,M) (wcscmp(N,M)==0)

CLASS(sh_node) {
	sh_node* prev;
	sh_ptr value;
	sh_node* next;
};
CLASS(sh_list) {
	sh_node* head;
	sh_node* tail;
	sh_uint size;
};
sh_list*sh_list_new();
sh_ptr* sh_list_add_before(sh_list* this, sh_uint index);
sh_ptr	sh_list_pop(sh_list* this, sh_uint index);
void	sh_list_delete(sh_list* this, sh_custom_func* custom_func, sh_ptr client_data);

#endif