#define X_STAT_CHK_DBG
#include "xlang.h"

static x_stat print(x_int* this) {
	if (this->spec == x_real_spec_any) {
		printf("any");
		return X_STAT_OK;
	}
	if (this->spec == x_real_spec_nan) {
		printf("nan");
		return X_STAT_OK;
	}
	if (this->symbol == x_real_sym_neg) {
		printf("-");
	}
	if (this->symbol == x_real_sym_zero) {
		printf("0");
		return X_STAT_OK;
	}
	if (this->spec == x_real_spec_inf) {
		printf("inf");
		return X_STAT_OK;
	}
	for (x_node* cur = this->value->tail; cur != NULL; cur = cur->prev) {
		if (cur == this->value->tail) printf("(");
		else printf("((");
	}
	for (x_node* cur = this->value->tail; cur != NULL; cur = cur->prev) {
		if (cur != this->value->tail) printf("+");
		if (cur == this->value->head) printf("%lld)", cur->value);
		else printf("%lld)<<32)", cur->value);
	}
	return X_STAT_OK;
}
static x_stat print_frac(x_frac* this) {
	if (this->spec == x_real_spec_any) {
		printf("any");
		return X_STAT_OK;
	}
	if (this->spec == x_real_spec_nan) {
		printf("nan");
		return X_STAT_OK;
	}
	if (this->symbol == x_real_sym_neg) {
		printf("-");
	}
	if (this->symbol == x_real_sym_zero) {
		printf("0");
		return X_STAT_OK;
	}
	if (this->spec == x_real_spec_inf) {
		printf("inf");
		return X_STAT_OK;
	}
	printf("fractions.Fraction(");
	print(this->u);
	printf(",");
	print(this->d);
	printf(")");
	return X_STAT_OK;
}

int main() {
	x_stat stat;
	stat = xlang_setup();
	if (!x_stat_pass(stat)) printf("Loading error: %d! ", stat);
	stat = xlang_setdown();
	if (!x_stat_pass(stat)) printf("Exiting error: %d!", stat);
	return 0;
}