#ifndef X3DBVHSUBDIVIDE_H_INCLUDED
#define X3DBVHSUBDIVIDE_H_INCLUDED

/*
 * Definitions
 */

#define BVH_BUILD_AUTO			0X0
#define BVH_BUILD_SAH			0X1
#define BVH_BUILD_MIDDLE		0X2
#define BVH_BUILD_EQUAL_COUNT		0X3


/*
 * Structures
 */

/* it's a tree node */
#pragma pack(4)
struct bvh_tree {
        struct box3d bound;	/* A union bound of all sub-primitive */
        uint32_t start_prim;	/* Starting position */
        uint8_t axis;		/* Which axis is choosen to subdivide */
        uint8_t num_prim;	/* Number of primitives are under the current node */
};
#pragma pack()

/*
 * a BVH accelerating structure
 * it's a subdivide because primitive objects are not splitted
 * objects are divide to 2 child branch for each node, currently
 */
struct bvh_subdivide {
        struct alg_list *prim_list;
        struct bvh_tree *bvh_tree;	/* The actual tree structure, unlike a common tree,
	                                   the primitives inside is linearly placed */
        int num_nodes;
};


/*
 * Functions' declarations
 */

struct bvh_subdivide *build_bvh_subdivide ( struct alg_list *prim_list, int build_method );
void free_bvh_subdivide ( struct bvh_subdivide *bvh );


#endif // X3DBVHSUBDIVIDE_H_INCLUDED
