#ifndef IX3DUTILITY_H_INCLUDED
#define IX3DUTILITY_H_INCLUDED


typedef struct alg_named_params x3d_named_params_t;

/*
 * Functions' declaration
 */
x3d_named_params_t *create_named_params_x3d ( void );
void free_named_params_x3d ( x3d_named_params_t *np );

#define push_named_params_x3d			push_named_params
#define pop_named_params_x3d			pop_named_params
#define push_params_x3d				push_params
#define pop_params_x3d				pop_params
#define check_named_params_x3d			check_named_params


#endif // IX3DUTILITY_H_INCLUDED
