#ifndef RASTERIZER_H_INCLUDED
#define RASTERIZER_H_INCLUDED


#include "rasterization.h"

struct rtcontext;
struct rt_point;
struct rt_line;
struct rt_triangle;
struct shader;

/*
 * structures
 */

struct rasterizer;

/*
 * functions' declaration
 */
struct rasterizer *create_rasterizer ( void );
void free_rasterizer ( struct rasterizer *rt );
void rasterizer_finalize ( struct rtcontext *cont, struct rasterizer *rt );
void rasterizer_run ( struct rasterizer *rt, rt_vertex *vert_list,
		      enum RT_PRIMITIVE_TYPE prim_type, int count );


#endif // RASTERIZER_H_INCLUDED
