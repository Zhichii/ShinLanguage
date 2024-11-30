#ifndef X_BIG_VALUE
#define X_BIG_VALUE

#include "basic.h"
typedef enum x_real_sym x_real_sym;
enum x_real_sym {
	x_real_sym_neg = -1,
	x_real_sym_zero = 0,
	x_real_sym_pos = 1
};
typedef enum x_real_spec x_real_spec;
enum x_real_spec {
	x_real_spec_def = 0, // Default
	x_real_spec_nan = 1,
	x_real_spec_inf = 2,
	x_real_spec_any = 3
};
typedef struct x_int x_int;
struct x_int {
	x_li* value;
	x_real_spec spec; // Compression
	x_real_sym symbol;
};
typedef x_stat x_int_calc_func(x_int**, x_int*, x_int*);
static x_stat x_int_new(x_int** pthis) {
	if (!pthis) return x_stat_error;
	x_int* this = *pthis;
	this = new(x_int);
	if (!this) return x_stat_nomem;
	x_stat stat = x_li_new(&this->value);
	x_check_stat_do(stat, { del(this); });
	this->spec = x_real_spec_def;
	this->symbol = x_real_sym_zero;
	*pthis = this;
	return x_stat_ok;
}
static x_stat x_int_del(x_int** pthis) {
	if (!pthis) return x_stat_error;
	x_int* this = *pthis;
	if (!this) return x_stat_already;
	x_li_del(&this->value, NULL, NULL);
	del(this);
	*pthis = NULL;
	return x_stat_ok;
}
static x_stat x_int_set_v(x_int** pthis, x_sint val) {
	if (!pthis) return x_stat_error;
	x_int* this = *pthis;
	if (!this) {
		x_stat stat = x_int_new(pthis);
		x_check_stat(stat);
	}
	x_li_del(&this->value, NULL, NULL);
	x_stat stat = x_li_new(&this->value);
	x_check_stat(stat);
	this->spec = x_real_spec_def;
	this->symbol = x_real_sym_pos;
	if (val < 0) {
		this->symbol = x_real_sym_neg;
		if (val + 1 == -9223372036854775807ll) {
			x_stat stat;
			stat = x_li_add_tail(&this->value, (void*)0);
			x_check_stat(stat);
			stat = x_li_add_tail(&this->value, (void*)0);
			x_check_stat(stat);
			stat = x_li_add_tail(&this->value, (void*)1);
			x_check_stat(stat);
			return x_stat_ok;
		}
		val = -val;
	}
	if (val == 0) {
		this->symbol = x_real_sym_zero;
		return x_stat_ok;
	}
	if (0 < val && val < 4294967296) {
		stat = x_li_add_tail(&this->value, (void*)val);
		x_check_stat(stat);
		return x_stat_ok;
	}
	if (4294967296 <= val) {
		stat = x_li_add_tail(&this->value, (void*)(val % 4294967296));
		x_check_stat(stat);
		stat = x_li_add_tail(&this->value, (void*)((val >> 32) % 4294967296));
		x_check_stat(stat);
		return x_stat_ok;
	}
	return x_stat_error;
}
static x_stat x_int_make_nan(x_int** pthis) {
	if (!pthis) return x_stat_error;
	x_int* this = *pthis;
	if (!this) {
		x_stat stat = x_int_new(pthis);
		x_check_stat(stat);
	}
	x_stat stat = x_int_set_v(pthis, 0);
	x_check_stat(stat);
	this->spec = x_real_spec_nan;
	return x_stat_ok;
}
static x_stat x_int_make_inf(x_int** pthis, x_real_sym symbol) {
	if (!pthis) return x_stat_error;
	x_int* this = *pthis;
	if (!this) {
		x_stat stat = x_int_new(pthis);
		x_check_stat(stat);
	}
	x_stat stat = x_int_set_v(pthis, 1);
	x_check_stat(stat);
	this->spec = x_real_spec_inf;
	if (symbol == x_real_sym_zero) symbol = x_real_sym_pos;
	this->symbol = symbol;
	return x_stat_ok;
}
static x_stat x_int_make_any(x_int** pthis) {
	if (!pthis) return x_stat_error;
	x_int* this = *pthis;
	if (!this) {
		x_stat stat = x_int_new(pthis);
		x_check_stat(stat);
	}
	x_stat stat = x_int_set_v(pthis, 0);
	x_check_stat(stat);
	this->spec = x_real_spec_any;
	return x_stat_ok;
}
static x_stat x_int_del_lead_zero(x_int** pthis) {
	if (!pthis) return x_stat_error;
	x_int* this = *pthis;
	if (!this) {
		x_stat stat = x_int_new(pthis);
		x_check_stat(stat);
	}
	for (x_node* cur = this->value->tail; cur != NULL; ) {
		if (cur->value == 0) {
			cur = cur->prev;
			x_li_pop_tail(&this->value, NULL);
		}
		else {
			cur = cur->prev;
		}
	}
	if (this->spec == x_real_spec_def && this->value->size == 0) this->symbol = x_real_sym_zero;
	return x_stat_ok;
}
static x_stat x_int_set(x_int** pthis, x_int* that) {
	if (!pthis) return x_stat_error;
	x_int* this = *pthis;
	if (!this) {
		x_stat stat = x_int_new(pthis);
		x_check_stat(stat);
	}
	if (!that) {
		x_stat stat = x_int_set_v(pthis, 0);
		return stat;
	}
	x_li_del(&this->value, NULL, NULL);
	x_stat stat = x_li_new(&this->value);
	x_check_stat(stat);
	this->spec = that->spec;
	this->symbol = that->symbol;
	if (this->spec == x_real_spec_def) {
		for (x_node* cur = that->value->head; cur != NULL; cur = cur->next) {
			x_stat stat = x_li_add_tail(&this->value, (void*)cur->value);
			x_check_stat(stat);
		}
	}
	return x_stat_ok;
}
static x_stat x_int_less(x_bool* pthis, x_int* a, x_int* b) {
	if (!pthis) return x_stat_error;
	if (!a && !b) {
		*pthis = false;
		return x_stat_ok;
	}
	if (!a) {
		*pthis = b->spec == x_real_spec_def && b->symbol == x_real_sym_neg;
		return x_stat_ok;
	}
	if (!b) {
		*pthis = a->spec == x_real_spec_def && a->symbol == x_real_sym_pos;
		return x_stat_ok;
	}
	if (a->spec == x_real_spec_any || b->spec == x_real_spec_any) {
		*pthis = either;
		return x_stat_ok;
	}
	if (a->spec == x_real_spec_nan || b->spec == x_real_spec_nan) {
		*pthis = false;
		return x_stat_ok;
	}
	if (a->symbol < b->symbol) { // They can compare size. See the definition of x_int_sym_xxx.
		*pthis = true;
		return x_stat_ok;
	}
	if (a->symbol > b->symbol) {
		*pthis = false;
		return x_stat_ok;
	}
	// The symbols are the same. 
	if (a->spec == x_real_spec_inf && b->spec == x_real_spec_inf) {
		*pthis = false;
		return x_stat_ok;
	}
	if (a->spec == x_real_spec_inf || b->spec == x_real_spec_inf) {
		if (a->symbol == x_real_sym_pos) *pthis = b->spec == x_real_spec_inf;
		if (a->symbol == x_real_sym_zero) *pthis = b->spec == x_real_spec_inf; // Default to positive. 
		if (a->symbol == x_real_sym_neg) *pthis = a->spec == x_real_spec_inf;
		return x_stat_ok;
	}
	if (a->value->size < b->value->size) {
		*pthis = true;
		return x_stat_ok;
	}
	if (a->value->size > b->value->size) {
		*pthis = false;
		return x_stat_ok;
	}
	x_node* cur_a = a->value->tail;
	x_node* cur_b = b->value->tail;
	for (; cur_a != NULL && cur_b != NULL; ) {
		if (cur_a->value < cur_b->value) {
			*pthis = true;
			return x_stat_ok;
		}
		if (cur_a->value > cur_b->value) {
			*pthis = false;
			return x_stat_ok;
		}
		cur_a = cur_a->prev;
		cur_b = cur_b->prev;
	}
	*pthis = false;
	return x_stat_ok;
}
static x_stat x_int_same(x_bool* pthis, x_int* a, x_int* b) {
	if (!pthis) return x_stat_error;
	if (!a && !b) {
		*pthis = true;
		return x_stat_ok;
	}
	if (!a) {
		*pthis = b->spec == x_real_spec_def && b->symbol == x_real_sym_zero;
		return x_stat_ok;
	}
	if (!b) {
		*pthis = a->spec == x_real_spec_def && a->symbol == x_real_sym_zero;
		return x_stat_ok;
	}
	if (a->spec == x_real_spec_any || b->spec == x_real_spec_any) {
		*pthis = true;
		return x_stat_ok;
	}
	if (a->spec == x_real_spec_nan && b->spec == x_real_spec_nan) {
		*pthis = true;
		return x_stat_ok;
	}
	if (a->spec == x_real_spec_nan || b->spec == x_real_spec_nan) {
		*pthis = false;
		return x_stat_ok;
	}
	if (a->symbol != b->symbol) {
		*pthis = false;
		return x_stat_ok;
	}
	// The symbols are the same. 
	if (a->spec == x_real_spec_inf && b->spec == x_real_spec_inf) {
		*pthis = true;
		return x_stat_ok;
	}
	if (a->spec == x_real_spec_inf || b->spec == x_real_spec_inf) {
		*pthis = false;
		return x_stat_ok;
	}
	if (a->value->size != b->value->size) {
		*pthis = false;
		return x_stat_ok;
	}
	x_node* cur_a = a->value->tail;
	x_node* cur_b = b->value->tail;
	for (; cur_a != NULL && cur_b != NULL; ) {
		if (cur_a->value != cur_b->value) {
			*pthis = false;
			return x_stat_ok;
		}
		cur_a = cur_a->prev;
		cur_b = cur_b->prev;
	}
	*pthis = true;
	return x_stat_ok;
}
static x_stat x_int_calc_base(x_int** pthis, x_int_calc_func* func, x_int* b) {
	if (!pthis) return x_stat_error;
	x_int* this = *pthis;
	if (!this) {
		x_stat stat = x_int_new(pthis);
		x_check_stat(stat);
	}
	x_stat stat;
	x_int* a;
	stat = x_int_new(&a);
	x_check_stat(stat);
	stat = x_int_set(&a, this);
	x_check_stat_do(stat, { x_int_del(&a); });
	stat = func(pthis, a, b);
	x_int_del(&a);
	return stat;
}
static x_stat x_int_calc_base_v(x_int** pthis, x_int_calc_func* func, x_sint val) {
	if (!pthis) return x_stat_error;
	x_int* this = *pthis;
	if (!this) {
		x_stat stat = x_int_new(pthis);
		x_check_stat(stat);
	}
	x_stat stat;
	x_int* a;
	x_int* b;
	stat = x_int_new(&a);
	x_check_stat(stat);
	stat = x_int_new(&b);
	x_check_stat_do(stat, { x_int_del(&a); });
	stat = x_int_set(&a, this);
	x_check_stat_do(stat, { x_int_del(&a); x_int_del(&b); });
	stat = x_int_set_v(&b, val);
	x_check_stat_do(stat, { x_int_del(&a); x_int_del(&b); });
	stat = func(pthis, a, b);
	x_int_del(&a);
	x_int_del(&b);
	return stat;
}
static x_stat x_int_add(x_int** pthis, x_int* a, x_int* b) {
	if (!pthis) return x_stat_error;
	x_int* this = *pthis;
	if (!this) {
		x_stat stat = x_int_new(pthis);
		x_check_stat(stat);
	}
	if (!a || (a->spec == x_real_spec_def && a->symbol == x_real_sym_zero)) {
		x_stat stat = x_int_set(pthis, b);
		return stat;
	}
	if (!b || (b->spec == x_real_spec_def && b->symbol == x_real_sym_zero)) {
		x_stat stat = x_int_set(pthis, a);
		return stat;
	}
	if (a->spec == x_real_spec_nan || b->spec == x_real_spec_nan) {
		x_stat stat = x_int_make_nan(pthis);
		return stat;
	}
	if (a->spec == x_real_spec_any || b->spec == x_real_spec_any) {
		x_stat stat = x_int_make_any(pthis);
		return stat;
	}
	x_stat stat;
	x_bool swap = false;
	if (a->value->size < b->value->size) swap = true;
	else if (a->value->size == b->value->size) {
		x_node* cur_a = a->value->tail;
		x_node* cur_b = b->value->tail;
		while (cur_a != NULL && cur_b != NULL) {
			if (cur_a->value < cur_b->value) {
				swap = true;
				break;
			}
			else if (cur_a->value > cur_b->value) {
				swap = false;
				break;
			}
			cur_a = cur_a->prev;
			cur_b = cur_b->prev;
		}
	}
	if (swap) {
		x_int* c = a;
		a = b;
		b = c;
	}
	stat = x_int_set(pthis, a);
	x_check_stat(stat);
	if (a->symbol == b->symbol) {
		if (a->spec == x_real_spec_inf && b->spec == x_real_spec_inf) {
			x_stat stat;
			stat = x_int_make_inf(pthis, x_real_sym_pos);
			x_check_stat(stat);
			return x_stat_ok;
		}
		// Do addition. 
		x_node* cur_t = this->value->head;
		x_node* cur_b = b->value->head;
		for (; cur_b != NULL; ) {
			x_sint temp = (x_sint)cur_t->value + (x_sint)cur_b->value;
			x_sint carry = temp / 4294967296;
			cur_t->value = (void*)(temp % 4294967296);
			cur_t = cur_t->next;
			if (carry != 0) {
				if (cur_t == NULL) {
					stat = x_li_add_tail(&this->value, (void*)(carry));
					x_check_stat(stat);
				}
				else {
					cur_t->value = (void*)(((x_sint)cur_t->value) + carry);
				}
			}
			cur_b = cur_b->next;
		}
	}
	else {
		if (a->spec == x_real_spec_inf && b->spec == x_real_spec_inf) {
			x_stat stat = x_int_make_nan(pthis);
			x_check_stat(stat);
			return x_stat_ok;
		}
		if (a->spec == x_real_spec_inf || b->spec == x_real_spec_inf) {
			x_stat stat = x_int_make_inf(pthis, x_real_sym_neg);
			x_check_stat(stat);
			return x_stat_ok;
		}
		// Do subtract. 
		x_node* cur_t = this->value->head;
		x_node* cur_b = b->value->head;
		for (; cur_b != NULL; ) {
			x_sint borrow = 0;
			x_sint temp = (x_sint)cur_t->value - (x_sint)cur_b->value;
			while (temp < 0) {
				borrow++;
				temp += 4294967296;
			}
			cur_t->value = (void*)(temp % 4294967296);
			cur_t = cur_t->next;
			if (borrow != 0) {
				if (cur_t == NULL) {
					__debugbreak();
					return x_stat_error; // I think it's impossible to get here. 
				}
				else {
					cur_t->value = (void*)(((x_sint)cur_t->value) - borrow);
				}
			}
			cur_b = cur_b->next;
		}
		x_int_del_lead_zero(pthis);
	}
	return x_stat_ok;
}
// If you are multiplying powers of 2, use left shift (<<) instead. 
static x_stat x_int_mul(x_int** pthis, x_int* a, x_int* b) {
	if (!pthis) return x_stat_error;
	x_int* this = *pthis;
	if (!this) {
		x_stat stat = x_int_new(pthis);
		x_check_stat(stat);
	}
	if (!a && !b) {
		x_stat stat = x_int_set_v(pthis, 0);
		return stat;
	}
	if (!a || (a->spec == x_real_spec_def && a->symbol == x_real_sym_zero)) {
		if (b->spec == x_real_spec_inf) {
			x_stat stat = x_int_make_nan(pthis);
			return stat;
		}
		x_stat stat = x_int_set_v(pthis, 0);
		return stat;
	}
	if (!b || (b->spec == x_real_spec_def && b->symbol == x_real_sym_zero)) {
		if (a->spec == x_real_spec_inf) {
			x_stat stat = x_int_make_nan(pthis);
			return stat;
		}
		x_stat stat = x_int_set_v(pthis, 0);
		return stat;
	}
	if (a->spec == x_real_spec_nan || b->spec == x_real_spec_nan) {
		x_stat stat = x_int_make_nan(pthis);
		return stat;
	}
	if (a->spec == x_real_spec_any || b->spec == x_real_spec_any) {
		x_stat stat = x_int_make_any(pthis);
		return stat;
	}
	if (a->spec == x_real_spec_inf && b->spec == x_real_spec_inf) {
		x_stat stat = x_int_make_inf(pthis, x_real_sym_pos);
		this->symbol = a->symbol * b->symbol;
		return stat;
	}
	if (a->spec == x_real_spec_inf || b->spec == x_real_spec_inf) {
		x_stat stat = x_int_make_inf(pthis, x_real_sym_pos);
		this->symbol = a->symbol * b->symbol;
		return stat;
	}
	x_stat stat;
	stat = x_int_set_v(pthis, 0);
	x_check_stat(stat);
	x_int* m;
	stat = x_int_new(&m);
	x_check_stat(stat);
	for (x_node* cur_a = a->value->head; cur_a != NULL; cur_a = cur_a->next) {
		x_int* k;
		stat = x_int_new(&k);
		x_check_stat(stat);
		k->symbol = x_real_sym_pos;
		{
			x_int* i;
			x_int_new(&i);
			while (1) {
				x_bool compare;
				stat = x_int_less(&compare, i, m);
				x_check_stat_do(stat, { x_int_del(&i); x_int_del(&k); x_int_del(&a); });
				if (!compare) break;
				stat = x_li_add_tail(&k->value, (void*)0);
				x_check_stat_do(stat, { x_int_del(&i); x_int_del(&k); x_int_del(&a); });
				stat = x_int_calc_base_v(&i, x_int_add, 1);
				x_check_stat_do(stat, { x_int_del(&i); x_int_del(&k); x_int_del(&a); });
			}
			x_int_del(&i);
		}
		x_sint t = 0;
		for (x_node* cur_b = b->value->head; cur_b != NULL; cur_b = cur_b->next) {
			t += ((x_sint)cur_a->value) * ((x_sint)cur_b->value);
			x_li_add_tail(&k->value, (void*)(t % 4294967296));
			t >>= 32;
		}
		if (t != 0) {
			x_li_add_tail(&k->value, (void*)(t));
		}
		x_int_calc_base(pthis, x_int_add, k);
		x_int_calc_base_v(&m, x_int_add, 1);
		x_int_del(&k);
	}
	this->symbol = a->symbol * b->symbol;
	x_int_del(&m);
	return x_stat_ok;
}
// If you are dividing powers of 2, use right shift (>>) instead. 
static x_stat x_int_div(x_int** pthis, x_int* u, x_int* d) {
	if (!pthis) return x_stat_error;
	x_int* this = *pthis;
	if (!this) {
		x_stat stat = x_int_new(pthis);
		x_check_stat(stat);
	}
	if (u->spec == x_real_spec_any || d->spec == x_real_spec_any) {
		x_stat stat = x_int_make_any(pthis);
		return stat;
	}
	if (u->spec == x_real_spec_nan || d->spec == x_real_spec_nan) {
		x_stat stat = x_int_make_nan(pthis);
		return stat;
	}
	if (u->spec == x_real_spec_inf && d->spec == x_real_spec_inf) {
		x_stat stat = x_int_make_any(pthis);
		return stat;
	}
	if (u->spec == x_real_spec_inf) {
		x_stat stat = x_int_make_inf(pthis, u->symbol * d->symbol);
		return stat;
	}
	if (d->spec == x_real_spec_inf) {
		x_stat stat = x_int_set_v(pthis, 0);
		return stat;
	}
	if (d->spec == x_real_spec_def && d->symbol == x_real_sym_zero) {
		if (u->spec == x_real_spec_def && u->symbol == x_real_sym_zero) {
			x_stat stat = x_int_make_any(pthis);
			return stat;
		}
		else {
			x_stat stat = x_int_make_inf(pthis, u->symbol);
			return stat;
		}
	}
	x_bool compare = false;
	if (u->value->size < d->value->size) compare = true;
	else if (u->value->size == d->value->size) {
		x_node* cur_u = u->value->tail;
		x_node* cur_d = d->value->tail;
		while (cur_u != NULL && cur_d != NULL) {
			if (cur_u->value < cur_d->value) {
				compare = true;
				break;
			}
			else if (cur_u->value > cur_d->value) {
				compare = false;
				break;
			}
			cur_u = cur_u->prev;
			cur_d = cur_d->prev;
		}
	}
	if (compare) {
		x_stat stat = x_int_set_v(pthis, 0);
		return stat;
	}
	x_stat stat;
	stat = x_int_set_v(pthis, 0);
	x_check_stat(stat);
	this->symbol = u->symbol * d->symbol;
	// Simulate. 
	x_int* d_shift;
	x_int* left;
	x_int* d_abs;
	x_sint k, b;
	stat = x_int_new(&d_shift);
	x_check_stat(stat);
	stat = x_int_new(&left);
	x_check_stat_do(stat, { x_int_del(&d_shift); });
	stat = x_int_new(&d_abs);
	x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); });
	stat = x_int_set(&d_abs, d);
	x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); });
	d_abs->symbol = x_real_sym_pos;
	x_sint u_sz = u->value->size;
	x_sint d_sz = d->value->size;
	for (x_sint i = 0; i < u_sz - d_sz + 1; i++) {
		stat = x_int_set_v(&d_shift, 0);
		x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); });
		x_node* cur_u = u->value->tail;
		for (x_sint j = 0; j < i; j++) cur_u = cur_u->prev;
		for (x_sint j = 0; j < d_sz; j++) {
			stat = x_li_add_head(&d_shift->value, (void*)cur_u->value);
			x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&d_abs); });
		}
		for (x_node* j = left->value->head; j != NULL; j = j->next) {
			stat = x_li_add_tail(&d_shift->value, (void*)j->value);
			x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&d_abs); });
		}
		d_shift->symbol = x_real_sym_pos;
		compare = false;
		stat = x_int_less(&compare, d_shift, d_abs);
		x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&d_abs); });
		if (compare) {
			stat = x_li_add_head(&this->value, (void*)0);
			x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&d_abs); });
			stat = x_int_set(&left, d_shift);
			x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&d_abs); });
			continue;
		}
		// Binary search. 
		k = 4294967296 / 2;
		b = k / 2;
		x_int* temp;
		stat = x_int_new(&temp);
		x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); });
		stat = x_int_set(&temp, d_abs);
		x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); x_int_del(&temp); });
		stat = x_int_calc_base_v(&temp, x_int_mul, k);
		x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); x_int_del(&temp); });
		temp->symbol = -temp->symbol;
		stat = x_int_add(&left, d_shift, temp);
		x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); x_int_del(&temp); });
		while (1) {
			x_bool compare1;
			x_bool compare2;
			stat = x_int_less(&compare1, d_abs, left);
			x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); x_int_del(&temp); });
			stat = x_int_same(&compare2, d_abs, left);
			x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); x_int_del(&temp); });
			if (left->symbol < x_real_sym_zero) {
				k -= b;
				b /= 2;
			}
			else if (compare1 || compare2) {
				k += b;
				b /= 2;
			}
			else break;
			if (b == 0) b = 1;
			stat = x_int_set(&temp, d_abs);
			x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); x_int_del(&temp); });
			stat = x_int_calc_base_v(&temp, x_int_mul, k);
			x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); x_int_del(&temp); });
			temp->symbol = -temp->symbol;
			stat = x_int_add(&left, d_shift, temp);
			x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); x_int_del(&temp); });
		}
		stat = x_li_add_head(&this->value, (void*)k);
		x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); x_int_del(&temp); });
		x_int_del(&temp);
	}
	x_int_del_lead_zero(pthis);
	x_int_del(&d_shift);
	x_int_del(&left);
	x_int_del(&d_abs);
	return x_stat_ok;
}
// If you are dividing powers of 2, use minus (add negative +) instead. 
static x_stat x_int_mod(x_int** pthis, x_int* u, x_int* d) {
	if (!pthis) return x_stat_error;
	x_int* this = *pthis;
	if (!this) {
		x_stat stat = x_int_new(pthis);
		x_check_stat(stat);
	}
	if (u->spec == x_real_spec_any || d->spec == x_real_spec_any) {
		x_stat stat = x_int_make_any(pthis);
		return stat;
	}
	if (u->spec == x_real_spec_nan || d->spec == x_real_spec_nan) {
		x_stat stat = x_int_make_nan(pthis);
		return stat;
	}
	if (u->spec == x_real_spec_inf && d->spec == x_real_spec_inf) {
		x_stat stat = x_int_make_any(pthis);
		return stat;
	}
	if (u->spec == x_real_spec_inf) {
		x_stat stat = x_int_make_inf(pthis, u->symbol * d->symbol);
		return stat;
	}
	if (d->spec == x_real_spec_inf) {
		x_stat stat = x_int_set_v(pthis, 0);
		return stat;
	}
	if (d->spec == x_real_spec_def && d->symbol == x_real_sym_zero) {
		if (u->spec == x_real_spec_def && u->symbol == x_real_sym_zero) {
			x_stat stat = x_int_set_v(pthis, 0);
			return stat;
		}
		else {
			x_stat stat = x_int_set(pthis, u);
			return stat;
		}
	}
	x_bool compare = false;
	if (u->value->size < d->value->size) compare = true;
	else if (u->value->size == d->value->size) {
		x_node* cur_u = u->value->tail;
		x_node* cur_d = d->value->tail;
		while (cur_u != NULL && cur_d != NULL) {
			if (cur_u->value < cur_d->value) {
				compare = true;
				break;
			}
			else if (cur_u->value > cur_d->value) {
				compare = false;
				break;
			}
			cur_u = cur_u->prev;
			cur_d = cur_d->prev;
		}
	}
	if (compare) {
		x_stat stat = x_int_set(pthis, u);
		return stat;
	}
	x_stat stat;
	// Simulate. 
	x_int* d_shift;
	x_int* left;
	x_int* d_abs;
	x_sint k, b;
	stat = x_int_new(&d_shift);
	x_check_stat(stat);
	stat = x_int_new(&left);
	x_check_stat_do(stat, { x_int_del(&d_shift); });
	stat = x_int_new(&d_abs);
	x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); });
	stat = x_int_set(&d_abs, d);
	x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); });
	d_abs->symbol = x_real_sym_pos;
	x_sint u_sz = u->value->size;
	x_sint d_sz = d->value->size;
	for (x_sint i = 0; i < u_sz - d_sz + 1; i++) {
		stat = x_int_set_v(&d_shift, 0);
		x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); });
		x_node* cur_u = u->value->tail;
		for (x_sint j = 0; j < i; j++) cur_u = cur_u->prev;
		for (x_sint j = 0; j < d_sz; j++) {
			stat = x_li_add_head(&d_shift->value, (void*)cur_u->value);
			x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&d_abs); });
		}
		for (x_node* j = left->value->head; j != NULL; j = j->next) {
			stat = x_li_add_tail(&d_shift->value, (void*)j->value);
			x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&d_abs); });
		}
		d_shift->symbol = x_real_sym_pos;
		compare = false;
		stat = x_int_less(&compare, d_shift, d_abs);
		x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&d_abs); });
		if (compare) {
			stat = x_int_set(&left, d_shift);
			x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&d_abs); });
			continue;
		}
		// Binary search. 
		k = 4294967296 / 2;
		b = k / 2;
		x_int* temp;
		stat = x_int_new(&temp);
		x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); });
		stat = x_int_set(&temp, d_abs);
		x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); x_int_del(&temp); });
		stat = x_int_calc_base_v(&temp, x_int_mul, k);
		x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); x_int_del(&temp); });
		temp->symbol = -temp->symbol;
		stat = x_int_add(&left, d_shift, temp);
		x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); x_int_del(&temp); });
		while (1) {
			x_bool compare1;
			x_bool compare2;
			stat = x_int_less(&compare1, d_abs, left);
			x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); x_int_del(&temp); });
			stat = x_int_same(&compare2, d_abs, left);
			x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); x_int_del(&temp); });
			if (left->symbol < x_real_sym_zero) {
				k -= b;
				b /= 2;
			}
			else if (compare1 || compare2) {
				k += b;
				b /= 2;
			}
			else break;
			if (b == 0) b = 1;
			stat = x_int_set(&temp, d_abs);
			x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); x_int_del(&temp); });
			stat = x_int_calc_base_v(&temp, x_int_mul, k);
			x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); x_int_del(&temp); });
			temp->symbol = -temp->symbol;
			stat = x_int_add(&left, d_shift, temp);
			x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); x_int_del(&temp); });
		}
		x_int_del(&temp);
	}
	stat = x_int_set(pthis, left);
	x_check_stat_do(stat, { x_int_del(&d_shift); x_int_del(&left); x_int_del(&d_abs); });
	x_int_del(&d_shift);
	x_int_del(&left);
	x_int_del(&d_abs);
	return x_stat_ok;
}

typedef struct x_frac x_frac;
struct x_frac {
	x_int* u;
	x_int* d;
	x_real_spec spec; // Compression
	x_real_sym symbol;
};
typedef x_stat x_frac_calc_func(x_frac**, x_frac*, x_frac*);
static x_stat x_frac_new(x_frac** pthis) {
	if (!pthis) return x_stat_error;
	x_frac* this = *pthis;
	this = new(x_frac);
	if (!this) return x_stat_nomem;
	x_stat stat;
	stat = x_int_new(&this->u);
	x_check_stat_do(stat, { del(this); });
	stat = x_int_new(&this->d);
	x_check_stat_do(stat, { del(this); x_int_del(&this->u); });
	this->spec = x_real_spec_def;
	this->symbol = x_real_sym_zero;
	*pthis = this;
	return x_stat_ok;
}
static x_stat x_frac_del(x_frac** pthis) {
	if (!pthis) return x_stat_error;
	x_frac* this = *pthis;
	if (!this) return x_stat_already;
	x_int_del(&this->u);
	x_int_del(&this->d);
	del(this);
	*pthis = NULL;
	return x_stat_ok;
}
static x_stat x_frac_optimize(x_frac** pthis);
static x_stat x_frac_set_v(x_frac** pthis, x_sint u, x_sint d) {
	if (!pthis) return x_stat_error;
	x_frac* this = *pthis;
	if (!this) {
		x_stat stat = x_frac_new(pthis);
		x_check_stat(stat);
	}
	x_stat stat;
	stat = x_int_set_v(&this->u, u);
	x_check_stat(stat);
	stat = x_int_set_v(&this->d, d);
	x_check_stat(stat);
	this->symbol = x_real_sym_pos;
	x_frac_optimize(pthis);
	return x_stat_error;
}
static x_stat x_frac_make_nan(x_frac** pthis) {
	if (!pthis) return x_stat_error;
	x_frac* this = *pthis;
	if (!this) {
		x_stat stat = x_frac_new(pthis);
		x_check_stat(stat);
	}
	x_stat stat = x_frac_set_v(pthis, 0, 1);
	x_check_stat(stat);
	this->spec = x_real_spec_nan;
	return x_stat_ok;
}
static x_stat x_frac_make_inf(x_frac** pthis, x_real_sym symbol) {
	if (!pthis) return x_stat_error;
	x_frac* this = *pthis;
	if (!this) {
		x_stat stat = x_frac_new(pthis);
		x_check_stat(stat);
	}
	x_stat stat = x_frac_set_v(pthis, 1, 1);
	x_check_stat(stat);
	this->spec = x_real_spec_inf;
	if (symbol == x_real_sym_zero) symbol = x_real_sym_pos;
	this->symbol = symbol;
	return x_stat_ok;
}
static x_stat x_frac_make_any(x_frac** pthis) {
	if (!pthis) return x_stat_error;
	x_frac* this = *pthis;
	if (!this) {
		x_stat stat = x_frac_new(pthis);
		x_check_stat(stat);
	}
	x_stat stat = x_frac_set_v(pthis, 0, 1);
	x_check_stat(stat);
	this->spec = x_real_spec_any;
	return x_stat_ok;
}
static x_stat x_frac_optimize(x_frac** pthis) {
	if (!pthis) return x_stat_error;
	x_frac* this = *pthis;
	if (!this) {
		x_stat stat = x_frac_new(pthis);
		x_check_stat(stat);
	}
	if (this->u->spec == x_real_spec_any || this->d->spec == x_real_spec_any) {
		x_stat stat = x_frac_make_any(pthis);
		return stat;
	}
	if (this->u->spec == x_real_spec_nan || this->d->spec == x_real_spec_nan) {
		x_stat stat = x_frac_make_nan(pthis);
		return stat;
	}
	if (this->u->spec == x_real_spec_inf && this->d->spec == x_real_spec_inf) {
		x_stat stat = x_frac_make_any(pthis);
		return stat;
	}
	if (this->u->spec == x_real_spec_inf) {
		x_stat stat = x_frac_make_inf(pthis, this->symbol * this->u->symbol * this->d->symbol);
		return stat;
	}
	if (this->d->spec == x_real_spec_inf) {
		x_stat stat = x_frac_set_v(pthis, 0, 1);
		return stat;
	}
	if (this->d->spec == x_real_spec_def && this->d->symbol == x_real_sym_zero) {
		if (this->u->spec == x_real_spec_def && this->u->symbol == x_real_sym_zero) {
			x_stat stat = x_frac_make_any(pthis);
			return stat;
		}
		else {
			x_stat stat = x_frac_make_inf(pthis, this->u->symbol);
			return stat;
		}
	}
	this->symbol = this->u->symbol * this->d->symbol;

	return x_stat_ok;
}
static x_stat x_frac_set(x_frac** pthis, x_frac* that) {
	if (!pthis) return x_stat_error;
	x_frac* this = *pthis;
	if (!this) {
		x_stat stat = x_frac_new(pthis);
		x_check_stat(stat);
	}
	x_stat stat;
	stat = x_int_set(&this->u, that->u);
	x_check_stat(stat);
	stat = x_int_set(&this->d, that->d);
	x_check_stat(stat);
	this->spec = that->spec;
	this->symbol = that->symbol;
	return x_stat_error;
}
static x_stat x_frac_set_int(x_frac** pthis, x_int* that) {
	if (!pthis) return x_stat_error;
	x_frac* this = *pthis;
	if (!this) {
		x_stat stat = x_frac_new(pthis);
		x_check_stat(stat);
	}
	x_stat stat;
	stat = x_int_set(&this->u, that);
	x_check_stat(stat);
	stat = x_int_set_v(&this->d, 1);
	x_check_stat(stat);
	return x_stat_error;
}
static x_stat x_frac_calc_base(x_frac** pthis, x_frac_calc_func* func, x_frac* b) {
	if (!pthis) return x_stat_error;
	x_frac* this = *pthis;
	if (!this) {
		x_stat stat = x_frac_new(pthis);
		x_check_stat(stat);
	}
	x_stat stat;
	x_frac* a;
	stat = x_frac_new(&a);
	x_check_stat(stat);
	stat = x_frac_set(&a, this);
	x_check_stat_do(stat, { x_frac_del(&a); });
	stat = func(pthis, a, b);
	x_frac_del(&a);
	return stat;
}
static x_stat x_frac_calc_base_v(x_frac** pthis, x_frac_calc_func* func, x_sint val) {
	if (!pthis) return x_stat_error;
	x_frac* this = *pthis;
	if (!this) {
		x_stat stat = x_frac_new(pthis);
		x_check_stat(stat);
	}
	x_stat stat;
	x_frac* a;
	x_frac* b;
	stat = x_frac_new(&a);
	x_check_stat(stat);
	stat = x_frac_new(&b);
	x_check_stat_do(stat, { x_frac_del(&a); });
	stat = x_frac_set(&a, this);
	x_check_stat_do(stat, { x_frac_del(&a); x_frac_del(&b); });
	stat = x_frac_set_v(&b, val, 1);
	x_check_stat_do(stat, { x_frac_del(&a); x_frac_del(&b); });
	stat = func(pthis, a, b);
	x_frac_del(&a);
	x_frac_del(&b);
	return stat;
}

#endif