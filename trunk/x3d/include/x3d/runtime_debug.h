#ifndef RUNTIME_DEBUG_H_INCLUDED
#define RUNTIME_DEBUG_H_INCLUDED

struct alg_named_params;

enum DBG_POSITION {
	DBG_KERNEL_START = 0X1,
	DBG_KERNEL_LOOP_ONCE = 0X2,
	DBG_KERNEL_LOOP = 0X4
};

struct unit_test {
	char *test_name;
	void (*test) ( struct alg_named_params *global_params );
	enum DBG_POSITION pos;
};

struct lib_func {
	char *func_name;
	char *lib_code;
	int (*bin_func) ( void );
};

void kernel_unit_test_add ( struct unit_test *ut );
void kernel_lib_func_add ( struct lib_func *lib );
void kernel_lib_func_retrieve ( char *func_name, struct lib_func *func );


#endif // RUNTIME_DEBUG_H_INCLUDED
