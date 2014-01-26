/* bvh.c: Manage Bvh subdivide primitive partition */
#include <algorithm.h>
#include <math/math.h>
#include <renderer/geometry.h>
#include "shape_common.h"
#include "bvh.h"


#define TREE_MAX_CHILDS			2
#define TREE_DEPTH			256

#define NODE_MAX_PRIMITIVES		32
#define SAH_MAX_BUCKETS			12
#define SAH_CUT_OFF			4

#define COST_INTERSECT_BOUND		(float)(1.0f)
#define COST_INTERSECT_PRIMITIVE	(float)(8.0f*COST_INTERSECT_BOUND)
#define COST_RECURSE			(float)(1.0f/8.0f*COST_INTERSECT_BOUND)


struct bound_data {
	struct point3d centroid;
	struct box3d bound;
	struct primitive *prim;
};

struct initial_tree {
	struct box3d bound;
	int start_prim;
	int axis;
	int num_prim;
	struct initial_tree *child[TREE_MAX_CHILDS];
};

/* Stack for tree traversal */
struct stack {
	struct initial_tree node;
	int split_pos[TREE_MAX_CHILDS - 1];
	int end_pos[TREE_MAX_CHILDS - 1];
};

struct sah_bucket {
	struct box3d bound;
	int num_prim;
	float cost;
};


/* packed mid-point information for partitioning */
struct mid_point {
	int axis;
	float mid;
};

struct initial_tree *build_initial_tree ( struct alg_list *prim_list, int part_method,
		struct alg_list *new_prim_list, int *num_nodes );
void free_initial_tree ( struct initial_tree *tree );
struct bvh_tree *initial_to_linear ( struct initial_tree *init_tree, int *num_nodes );
int part_by_mid_point ( struct bound_data *prim_bound, struct mid_point *volume );
int part_by_mid_prim ( struct bound_data *prim_bound0, struct bound_data *prim_bound1,
		       struct mid_point *volume );
void get_centroid ( struct box3d *bound, struct point3d *cen );
void create_initial_node ( struct box3d *scale, int axis, int num_prim, int start_pos,
			   int ichild, int *node_count, struct initial_tree *node );


struct bvh_subdivide *build_bvh_subdivide ( struct alg_list *prim_list, int build_method )
{
	struct bvh_subdivide *bvh = alloc_fix ( sizeof ( *bvh ), 1 );

	/* Select Sah as default build method if it is not specified */
	if ( build_method != BVH_BUILD_MIDDLE &&
	     build_method != BVH_BUILD_EQUAL_COUNT &&
	     build_method != BVH_BUILD_SAH ) {
		build_method = BVH_BUILD_SAH;
	}

	/* Recursively build Bvh tree for primitives */
	struct alg_list new_prim_list;
	create_alg_list ( &new_prim_list, sizeof ( struct primitive ), alg_list_len ( prim_list ) );
	int num_nodes;
	struct initial_tree *initial =
		build_initial_tree ( prim_list, build_method, &new_prim_list, &num_nodes );
	swap_alg_list ( &new_prim_list, prim_list );
	free_alg_list ( &new_prim_list );
	bvh->prim_list = prim_list;

	/* Convert to a linear tree, which is to be actually used */
	bvh->bvh_tree = initial_to_linear ( initial, &num_nodes );
	free_initial_tree ( initial );
	bvh->num_nodes = num_nodes;
	return bvh;
}

void free_bvh_subdivide ( struct bvh_subdivide *bvh )
{
	free_fix ( bvh->bvh_tree );
	memset ( bvh, 0, sizeof ( *bvh ) );
	free_fix ( bvh );
}

int part_by_mid_point ( struct bound_data *prim_bound, struct mid_point *volume )
{
	return prim_bound->centroid.p[volume->axis] < volume->mid;
}

int part_by_mid_prim ( struct bound_data *prim_bound0, struct bound_data *prim_bound1,
		       struct mid_point *volume )
{
	return prim_bound0->centroid.p[volume->axis] < prim_bound1->centroid.p[volume->axis];
}

void get_centroid ( struct box3d *b, struct point3d *c )
{
	add_point3d ( &b->min, &b->max, c );
	scale_point3d_u ( 1.0f/2.0f, c );
}

void create_initial_node ( struct box3d *scale, int axis, int num_prim, int start_pos,
			   int ichild, int *node_count, struct initial_tree *node )
{
	copy_box3d ( scale, &node->bound );
	node->axis = axis;
	node->num_prim = num_prim;
	node->start_prim = start_pos;
	node->child[ichild] = alloc_fix ( sizeof ( *node ), 1 );
	(*node_count) ++;
	int i;
	for ( i = 0; i < TREE_MAX_CHILDS; i ++ ) {
		node->child[ichild]->child[i] = nullptr;
	}
}

/* Produce an initial Bvh tree, in which the is not linearly organized */
struct initial_tree *build_initial_tree ( struct alg_list *prim_list, int part_method,
		struct alg_list *new_prim_list, int *num_nodes )
{
	/* Create bound information for each primitive */
	struct bound_data *bounds = alloc_fix ( sizeof ( *bounds ), alg_list_len ( prim_list ) );
	int i;
	for ( i = 0; i < alg_list_len ( prim_list ); i ++ ) {
		primitive_get_bound ( alg_list_i ( prim_list, i ), &bounds[i].bound );
		get_centroid ( &bounds[i].bound, &bounds[i].centroid );
		bounds[i].prim = alg_list_i ( prim_list, i );
	}

	int start_pos = 0;
	int split_pos = start_pos;
	int end_pos = alg_list_len ( prim_list );
	*num_nodes = 0;
	declare_stack ( stack, TREE_DEPTH*4*sizeof (int) );

	/*
	 * Construction of the initial bvh tree
	 * This is the first time writing this code. I have a feeling that the bvh
	 * would change to 4 branched tree, so I use ichild to indicate branches to
	 * make it flexible for the change. But at the time I am writing this, it
	 * is implemented to be a 2 branched binary tree
	 */
	struct initial_tree *tree = alloc_fix ( sizeof ( *tree ), 1 );
	struct initial_tree *curr_node = tree;
	for ( i = 0; i < TREE_MAX_CHILDS; i ++ ) {
		curr_node->child[i] = nullptr;
	}
	int ichild = 0;

	/** BEGIN TRAVERSAL **/
	begin_tree_traversal ( TT0 );

	/* Backtrack to other node when only 1 primitive left in current branch */
	int num_prim = end_pos - start_pos;
	if ( num_prim <= 1 ) {
		backtrack_tree_branch ( &stack, TT0,
					pop_stack ( &stack, end_pos );
					pop_stack ( &stack, split_pos );
					pop_stack ( &stack, ichild );
					start_pos = split_pos;
					ichild ++;
				      );
	}

	/* bound in all primitives under the current node */
	struct box3d scale;
	init_box3d ( &scale );
	for ( i = start_pos; i < end_pos; i ++ ) {
		union_box3d_u ( &scale, &bounds[i].bound );
	}

	// Select the axis that has maximum distance
	int axis;
	struct vector3d diff;
	sub_point3d ( &scale.max, &scale.min, &diff );
	if ( diff.x > diff.y && diff.x > diff.z ) {
		axis = VECTOR_X_AXIS;
	} else if ( diff.y > diff.z ) {
		axis = VECTOR_Y_AXIS;
	} else {
		axis = VECTOR_Z_AXIS;
	}
	/*
			if ( vDelta.p[axis] == 0.0f )
			{
				nodeIndicator = 1;
	            BackTrackToRightBranch ( bvhStack, stackDepth, start, end );

			}// End If ( Non object left ? )
	*/
	switch ( part_method ) {
	case BVH_BUILD_MIDDLE: {
		/* mid point of the choosen axis of the current volume */
		float mid = diff.v[axis]/2.0f;
		struct mid_point vol_mid;
		vol_mid.axis = axis;
		vol_mid.mid = mid;
		partition_alg ( &bounds[start_pos], end_pos - start_pos, &vol_mid,
				part_by_mid_point, &split_pos );
		enter_tree_branch (
			push_stack ( &stack, curr_node );
			push_stack ( &stack, ichild );
			push_stack ( &stack, split_pos );
			push_stack ( &stack, end_pos );
			end_pos = split_pos;
			ichild = 0;	/* Enter left child */
		);
		break;
	}

	case BVH_BUILD_EQUAL_COUNT: {
		/* mid primitive of the current bounded array */
		split_pos = (start_pos + end_pos) >> 1;
		struct mid_point vol_mid;
		vol_mid.axis = axis;
		split_i_alg ( &bounds[start_pos], end_pos - start_pos, split_pos,
			      &vol_mid, part_by_mid_prim );
		enter_tree_branch (
			push_stack ( &stack, curr_node );
			push_stack ( &stack, ichild );
			push_stack ( &stack, split_pos );
			push_stack ( &stack, end_pos );
			end_pos = split_pos;
			ichild = 0;	/* Enter left child */
		);
		break;
	}

	case BVH_BUILD_SAH: {
		/* Stop applying SAH model since it's not worthwhile */
		if ( num_prim <= SAH_CUT_OFF ) {
			/* Uses equal count splitting method
			 * part by the middle primitive */
			split_pos = (start_pos + end_pos) >> 1;
			struct mid_point vol_mid;
			vol_mid.axis = axis;
			split_i_alg ( &bounds[start_pos], end_pos - start_pos, split_pos,
				      &vol_mid, part_by_mid_prim );
			enter_tree_branch (
				push_stack ( &stack, curr_node );
				push_stack ( &stack, ichild );
				push_stack ( &stack, split_pos );
				push_stack ( &stack, end_pos );
				end_pos = split_pos;
				ichild = 0;	/* Enter left child */
			);
		} else {
			struct sah_bucket buckets[SAH_MAX_BUCKETS];

			for ( i = 0; i < SAH_MAX_BUCKETS; i ++ ) {
				init_box3d ( &buckets[i].bound );
				buckets[i].num_prim = 0;
			}

			/*
			 * We will give cost estimation to each bucket (, or batch of primitives)
			 * |p,p,p, ...|p,p,p|p,p, ...|...|, each bucket line is perpendicular to
			 * the choosen axis, the bucket is choosen by
			 * ibucket = floor ( (prim_pos - bound_start)/(bound_end - bound_start) )
			 */
			float inv_ext = 1.0f/diff.v[axis];
			for ( i = start_pos; i < end_pos; i ++ ) {
				int ibucket = (bounds[i].centroid.p[axis] - scale.min.p[axis])*inv_ext;

				/*
				 * The last bucket line is not used for partitioning
				 * but this can be caused by floating point error.
				 * we also will have to find out the exact boundary of the bucket
				 * since the boundary is usually less than the extent
				 * between two bucket lines, or the extent of the perpendicular axis
				 */
				if ( i == SAH_MAX_BUCKETS ) {
					i = SAH_MAX_BUCKETS - 1;
				}
				union_box3d_u ( &buckets[i].bound, &bounds[i].bound );
				buckets[ibucket].num_prim ++;
			}

			float inv_scale_area = 1.0f/surface_area_box3d ( &scale );
			for ( i = 0; i < SAH_MAX_BUCKETS; i ++ ) {
				/*
				 * Scost = Ttraverse + Pa*(a(0) + a(1) + ... + a(Na)) +
				 *                     Pb*(b(0) + b(1) + ... + b(Nb))
				 * Pa denotes the possibility of bound B being intersected
				 * Pb is possibility for bound B, which is equivalent to:
				 * Pa = Sa/S, Pb = Sb/S (S denotes for surface area)
				 */
				struct box3d ba;
				struct box3d bb;
				init_box3d ( &ba );
				init_box3d ( &bb );
				int na = 0;
				int nb = 0;
				int j;
				for ( j = 0; j <= i; j ++ ) {
					union_box3d_u ( &ba, &buckets[j].bound );
					na += buckets[j].num_prim;
				}

				for ( /* j = i */; j < SAH_MAX_BUCKETS; j ++ ) {
					union_box3d_u ( &bb, &buckets[j].bound );
					nb += buckets[j].num_prim;
				}

				/* gathering cost info for current bucket */
				float pa = surface_area_box3d ( &ba )*inv_scale_area;
				float pb = surface_area_box3d ( &bb )*inv_scale_area;
				buckets[i].cost = COST_RECURSE + COST_INTERSECT_BOUND*(pa*na + pb*nb);
			}

			/* best position if we are to split */
			float min_cost = buckets[0].cost;
			int ibucket = 0;
			int j;
			for ( j = 1; j < SAH_MAX_BUCKETS; j ++ ) {
				if ( buckets[j].cost < min_cost ) {
					min_cost = buckets[j].cost;
					ibucket = j;
				}
			}

			/*
			 * Now, we got to think: if too many primitives are within the current branch,
			 * we die with massive amount of intersections. if too few primitives,
			 * our traversal time would cover the Sbox/Sscale ray-box intersection benefit
			 */
			if ( num_prim > NODE_MAX_PRIMITIVES ||
			     min_cost < num_prim*COST_INTERSECT_PRIMITIVE ) {
				/*
				 * we choose to split, we use the best position where we spent
				 * uncountable effort to find
				 * effective_pos =
				 *   bound_start + min_cost_bucket*(bound_end - bound_start)/n_buckets
				 */
				float best_point =
					scale.min.p[axis] + ibucket*diff.v[axis]/(SAH_MAX_BUCKETS);
				struct mid_point vol_mid;
				vol_mid.axis = axis;
				vol_mid.mid = best_point;
				partition_alg ( &bounds[start_pos], end_pos - start_pos, &vol_mid,
						part_by_mid_point, &split_pos );
				enter_tree_branch (
					push_stack ( &stack, curr_node );
					push_stack ( &stack, ichild );
					push_stack ( &stack, split_pos );
					push_stack ( &stack, end_pos );
					end_pos = split_pos;
					ichild = 0;	/* Enter left child */
				);
			} else {
				/* We would choose not to split since it's not worthwhile */
				create_initial_node ( &scale, axis, num_prim, start_pos, ichild,
						      num_nodes, curr_node );
				backtrack_tree_branch ( &stack, TT0,
							pop_stack ( &stack, end_pos );
							pop_stack ( &stack, split_pos );
							pop_stack ( &stack, ichild );
							pop_stack ( &stack, curr_node );
							start_pos = split_pos;
							ichild ++;
							curr_node = curr_node->child[ichild];
						      );
			}
		}// End If ( Stop applying SAH model ? )

		break;
	}
	}// End Switch ( Build method )

	create_initial_node ( &scale, axis, num_prim, start_pos, ichild, num_nodes, curr_node );
	curr_node = curr_node->child[ichild];

	/** END TRAVERSAL **/
	end_tree_traversal ( TT0 );

	for ( i = 0; i < alg_list_len ( prim_list ); i ++ ) {
		add_element_alg_list ( bounds[i].prim, new_prim_list );
	}
	free_fix ( bounds );
	return tree;
}

void free_initial_tree ( struct initial_tree *tree )
{
	declare_stack ( stack, TREE_DEPTH*4*sizeof (int) );
	struct initial_tree *curr_node = tree;
	int ichild = 0;

	begin_tree_traversal ( TT0 );
	// Backtrack to the right branch
	if ( curr_node == nullptr || ichild < TREE_MAX_CHILDS ) {
		struct initial_tree *tmp;
		backtrack_tree_branch ( &stack, TT0,
					pop_stack ( &stack, ichild );
					pop_stack ( &stack, curr_node );
					ichild ++;
					tmp = (ichild >= TREE_MAX_CHILDS) ? curr_node : nullptr;
					curr_node = curr_node->child[ichild];
					free_fix ( tmp );
				      );
	}
	enter_tree_branch (
		push_stack ( &stack, curr_node );
		push_stack ( &stack, ichild );
		ichild = 0;
		curr_node = curr_node->child[ichild];
	);
	end_tree_traversal ( TT0 );
}

struct bvh_tree *initial_to_linear ( struct initial_tree *init_tree, int *num_nodes )
{
	/* TODO (davis#1#): implement linear bvh generation */
	return nullptr;
}
