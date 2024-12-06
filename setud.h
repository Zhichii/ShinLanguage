#ifndef X_SETUD
#define X_SETUD

#include "basic.h"
#include "global.h"

static x_stat xlang_setdown();

static x_stat xlang_setup() {
	x_stat stat;
	stat = x_li_new(&xlang_types);
	x_check_stat_do(stat, { xlang_setdown(); });
	stat = x_li_new(&xlang_vars);
	x_check_stat_do(stat, { xlang_setdown(); });
	stat = x_object_new(&xlang_object);
	x_check_stat_do(stat, { xlang_setdown(); });
	return X_STAT_OK;
}
static x_stat xlang_setdown() {
	x_li_del(&xlang_types, (x_li_del_func*)x_object_delref, NULL);
	x_li_del(&xlang_vars, (x_li_del_func*)x_object_delref, NULL);
	x_object_delref(&xlang_object);
	return X_STAT_OK;
}

#endif