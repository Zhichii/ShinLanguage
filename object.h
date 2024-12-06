#ifndef X_OBJECT
#define X_OBJECT

typedef enum x_access x_access;
enum x_access {
	X_ACCESS_PUBLIC,
	X_ACCESS_PRIVATE, // Warning only on use outside class. Do not be confused with X_ACCESS_FORCE_PRIVATE. 
	X_ACCESS_FORCE_PRIVATE // Error on use outside class. Do not be confused with X_ACCESS_PRIVATE. 
};

typedef struct x_object x_object;
struct x_object {
	x_object* type;
	x_byte* bytes;
	x_uint size;
	x_uint ref_cnt;
};
static x_stat x_object_delref(x_object** pthis);
static x_stat x_object_new(x_object** pthis) {
	if (!pthis) return X_STAT_ERROR;
	x_object* this = *pthis;
	if (this) x_object_delref(pthis);
	this = new(x_object);
	if (!this) return X_STAT_NO_MEM;
	this->ref_cnt = 0;
	this->type = NULL;
	this->bytes = NULL;
	this->size = 0;
	*pthis = this;
	return X_STAT_OK;
}
static x_stat x_object_newref(x_object** pthis, x_object* that) {
	if (!pthis) return X_STAT_ERROR;
	x_object* this = that;
	that->ref_cnt++;
	*pthis = this;
	return X_STAT_OK;
}
static x_stat x_object_delref(x_object** pthis) {
	if (!pthis) return X_STAT_ERROR;
	x_object* this = *pthis;
	if (!this) return X_STAT_ALREADY;
	if (this->ref_cnt == 0) return X_STAT_ALREADY;
	this->ref_cnt--;
	if (this->ref_cnt == 0) {
		if (this->bytes) del(this->bytes);
		this->type = NULL;
		this->size = 0;
	}
	*pthis = NULL;
	return X_STAT_OK;
}
static x_stat x_object_copy(x_object** pthis, x_object* that) {
	if (!pthis) return X_STAT_ERROR;
	x_object* this = *pthis;
	if (!this) {
		x_stat stat = x_object_new(pthis);
		x_check_stat(stat);
	}
	this = that;
	if (this->bytes) del(this->bytes);
	this->type = that->type;
	this->size = that->size;
	if (this->size) this->bytes = newlist(x_byte, this->size);
	*pthis = this;
	return X_STAT_OK;
}

typedef struct x_tag x_tag;
struct x_tag {
	x_bool is_static;
	x_bool is_mutable;
	x_access accessibility;
	x_char name[128];
	x_object* type;
	x_object* value; // If not static, then this will be the default value. 
};
static x_stat x_tag_del(x_tag** pthis) {
	if (!pthis) return X_STAT_ERROR;
	x_tag* this = *pthis;
	if (!this) return X_STAT_ALREADY;
	del(this);
	*pthis = NULL;
	return X_STAT_OK;
}

#endif