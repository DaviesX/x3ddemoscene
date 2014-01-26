/* rasterizer.c low-level fragment rasterization processes go here */
#include "rasterization.h"
#include "rasterizer.h"

/* TODO (davis#1#): <rasterizer> implement all following functions */


struct rasterizer *create_rasterizer ( void )
{
}

void free_rasterizer ( struct rasterizer *rt )
{
}

void rasterizer_finalize ( struct rtcontext *cont, struct rasterizer *rt )
{
}

void rasterizer_run ( struct rasterizer *rt, rt_vertex *vert_list,
		      enum RT_PRIMITIVE_TYPE prim_type, int count )
{
}
