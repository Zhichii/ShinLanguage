#define X_STAT_CHK_DBG
#include "xlang.h"

static x_stat print(x_int* this) {
	if (this->spec == x_real_spec_any) {
		printf("any");
		return x_stat_ok;
	}
	if (this->spec == x_real_spec_nan) {
		printf("nan");
		return x_stat_ok;
	}
	if (this->symbol == x_real_sym_neg) {
		printf("-");
	}
	if (this->symbol == x_real_sym_zero) {
		printf("0");
		return x_stat_ok;
	}
	if (this->spec == x_real_spec_inf) {
		printf("inf");
		return x_stat_ok;
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
	return x_stat_ok;
}
static x_stat print_frac(x_frac* this) {
	if (this->spec == x_real_spec_any) {
		printf("any");
		return x_stat_ok;
	}
	if (this->spec == x_real_spec_nan) {
		printf("nan");
		return x_stat_ok;
	}
	if (this->symbol == x_real_sym_neg) {
		printf("-");
	}
	if (this->symbol == x_real_sym_zero) {
		printf("0");
		return x_stat_ok;
	}
	if (this->spec == x_real_spec_inf) {
		printf("inf");
		return x_stat_ok;
	}
	printf("fractions.Fraction(");
	print(this->u);
	printf(",");
	print(this->d);
	printf(")");
	return x_stat_ok;
}

int main() {
	int cnt = 0;
chi:
	x_frac* test1;
	x_frac_new(&test1);
	x_frac_set_v(&test1, 33333333333333333, 33333);
	print_frac(test1);
	printf(" + ");
	x_frac* test2;
	x_frac_new(&test2);
	x_frac_set_v(&test2, 1145, 1);
	print_frac(test2);
	printf(" == ");
	x_frac* test3;
	x_frac_new(&test3);
	//x_frac_div(&test3, test1, test2);
	print_frac(test3);
	x_frac_del(&test1);
	x_frac_del(&test2);
	x_frac_del(&test3);
	cnt++;
	if (cnt == 1);
	else goto chi;
	return 0;
}