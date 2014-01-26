#ifndef X3DRASTERDATAPACKER_H_INCLUDED
#define X3DRASTERDATAPACKER_H_INCLUDED


/*
 * Definitions
 */

#if 0
/*
 * Structures
 */

// Specifies pass info
struct pass_context {
	int vmask;
};


/*
 * Constants
 */

// Fixed pipeline pass
static const struct pass_context FixedPipelinePass = {
	VMK_POSITION | VMK_NORMAL | VMK_COLOR
};

// Texture pass
static const struct pass_context TexturePass = {
	VMK_POSITION | VMK_TEXCOOR
};


/*
 * Functions' declaration
 */

void get_pass_context ( int passType, struct pass_context *pc );
void push_prim_list_to_vibuffer ( struct alg_list *prim_list, struct pass_context *pc,
				  struct vertex_buffer *vb, struct index_buffer *ib );
#endif

#endif // X3DRASTERDATAPACKER_H_INCLUDED
