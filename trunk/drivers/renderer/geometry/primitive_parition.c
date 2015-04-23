/* primitive_partition.c: divide-and-conquer optimization for primitives  */
#include <misc.h>
#include <renderer/geometry.h>
#include "bvh.h"
#include "grid.h"
#include "octree.h"
#include "kdtree.h"


struct primpart_ops {
        void *(*build) ( struct alg_list *prim, int extra_params );
        void (*free) ( void *parter );
};

static void *build_null ( struct alg_list *prim, int extra_params );
static void free_null ( void *parter );

static const struct primpart_ops PrimPartOps[] = {
        [PRIMITIVE_PART_NULL].build = cast(PrimPartOps->build) 	build_null,
        [PRIMITIVE_PART_NULL].free = cast(PrimPartOps->free) 	free_null,
        [PRIMITIVE_PART_GRID].build = cast(PrimPartOps->build) 	build_grid_spatial,
        [PRIMITIVE_PART_GRID].free = cast(PrimPartOps->free) 	free_grid_spatial,
        [PRIMITIVE_PART_BVH].build = cast(PrimPartOps->build) 	build_bvh_subdivide,
        [PRIMITIVE_PART_BVH].free = cast(PrimPartOps->free)	free_bvh_subdivide
};


void build_primitive_partition ( struct alg_list *prim, enum PRIMPART_IDR idr, int extra_params,
                                 struct prim_part *pp )
{
        pp->idr = idr;
        pp->prim = prim;
        pp->parter = PrimPartOps[idr].build ( prim, extra_params );
}

void free_primitive_partition ( struct prim_part *pp )
{
        PrimPartOps[pp->idr].free ( pp->parter );
        pp->idr = PRIMITIVE_PART_NULL;
        pp->parter = nullptr;
}

static void *build_null ( struct alg_list *prim, int extra_params )
{
        return nullptr;
}

static void free_null ( void *parter )
{
        parter = nullptr;
}

bool prim_part_null_first ( struct prim_part *pp, struct primpart_iterator *iter,
                            struct primitive *prim )
{
        return false;
}

bool prim_part_null_next ( bool positivity, struct primpart_iterator *iter,
                           struct primitive *prim )
{
        return false;
}
