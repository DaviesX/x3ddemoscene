#ifndef X3DRASTERSHADING_H_INCLUDED
#define X3DRASTERSHADING_H_INCLUDED


#include <x3d/common.h>

/*
 * Structures
 */

/* Defines a shading fragment */
struct fragment {
	float *attrs;
};

/* Defines a shaded value output */
struct fragment_output {
	untyped *value;
	int offsets;
};

/* Stores a list of fragment to be shade */
struct fragment_cache {
	struct fragment *f;
	struct fragment_output *fo;
	int num_frag;
};

struct shader {
};

typedef void *(*f_Shader_Func) ( void );

/*
 * functions' declaration
 */
f_Shader_Func shader_get_func ( struct shader *shader );


#endif // X3DRASTERSHADING_H_INCLUDED
