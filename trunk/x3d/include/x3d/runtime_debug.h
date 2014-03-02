#ifndef RUNTIME_DEBUG_H_INCLUDED
#define RUNTIME_DEBUG_H_INCLUDED

struct alg_named_params;

enum DBG_POSITION {
	DBG_KERNEL_START 	= 0X1,
	DBG_KERNEL_LOOP_ONCE 	= 0X2,
	DBG_KERNEL_LOOP 	= 0X4,
	DBG_KERNEL_HALT 	= 0X8
};

struct unit_test {
	char *test_name;
	void (*init) ( struct alg_named_params *global_params );
	void (*test) ( struct alg_named_params *global_params );
	void (*free) ( struct alg_named_params *global_params );
	enum DBG_POSITION pos;
};

struct lib_func {
	char *func_name;
	char *lib_code;
	int (*bin_func) ( void );
};

void kernel_unit_test_add ( struct unit_test *ut );


#endif // RUNTIME_DEBUG_H_INCLUDED
