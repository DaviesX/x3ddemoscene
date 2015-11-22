/* trimesh_shape.c: triangle mesh implementation */
#include <system/allocator.h>
#include "trimesh_shape.h"


void compute_mesh_bound ( struct triangle_mesh_shape *mesh, struct box3d *bound );
int convex_poly_to_triangles ( int *poly_index, int num_vert, int **tri_index );


/* Convert a convex polygon to a set of triangles */
int convex_poly_to_triangles ( int *poly_index, int num_vert, int **tri_index )
{
        alloc_var_flush ( tri_index );
        uint32_t i;
        uint32_t j = 0;
        for ( i = 1; i < i - 1; i ++ ) {
                tri_index = alloc_var_push ( &poly_index[0], &tri_index );
                tri_index = alloc_var_push ( &poly_index[i], &tri_index );
                tri_index = alloc_var_push ( &poly_index[i + 1], &tri_index );
                j ++;
        }
        return i;
}

void compute_mesh_bound ( struct triangle_mesh_shape *mesh, struct box3d *bound )
{
        init_box3d ( bound );
        int i;
        for ( i = 0; i < mesh->num_tri; i ++ ) {
                struct box3d tmp_box;
                int vi = i*3;
                struct triangle3d t;
                build_triangle3d ( &mesh->v[mesh->i[vi + 0]],
                                   &mesh->v[mesh->i[vi + 1]],
                                   &mesh->v[mesh->i[vi + 2]], &t );
                box_triangle3d ( &t, &tmp_box );
                union_box3d_u ( bound, &tmp_box );
        }
}

void build_triangle_mesh ( struct point3d *vertex, int num_vert, int *index, int num_tri,
                           struct vector3d *normal, struct vector3d *tangent, struct point2d *uv,
                           struct point3d *scale, struct point3d *rotation, struct point3d *world_pos,
                           int is_trans, struct triangle_mesh_shape *mesh )
{
        assert ( vertex && index );
        assert ( num_vert != 0 && num_tri != 0 );

        mesh->num_vertex = num_vert;
        mesh->num_tri = num_tri;

        mesh->v = alloc_var ( sizeof ( *mesh->v ), mesh->num_vertex );
        mesh->i = alloc_var ( sizeof ( int ), 3*mesh->num_tri );
        memcpy ( &mesh->v, vertex, mesh->num_vertex*sizeof ( *mesh->v ) );
        memcpy ( &mesh->i, index, mesh->num_tri*3*sizeof ( *mesh->i ) );

        mesh->tri = alloc_var ( sizeof ( *mesh->tri ), mesh->num_tri );
        int i;
        for ( i = 0; i < mesh->num_tri; i ++ ) {
                mesh->tri[i].mesh = mesh;
                mesh->tri[i].i = &mesh->i[i*3];
        }

        compute_mesh_bound ( mesh, &mesh->prop.bound );

        if ( uv ) {
                mesh->uv = alloc_var ( sizeof ( *mesh->uv ), mesh->num_vertex );
                memcpy ( mesh->uv, uv, mesh->num_vertex*sizeof ( *mesh->uv ) );
        }

        if ( normal ) {
                mesh->n = alloc_var ( sizeof ( *mesh->n ), mesh->num_vertex );
                memcpy ( mesh->n, normal, mesh->num_vertex*sizeof ( *mesh->n ) );
        } else {
        }

        if ( tangent ) {
                mesh->t = alloc_var ( sizeof ( *mesh->t ), mesh->num_vertex );
                memcpy ( mesh->t, tangent, mesh->num_vertex*sizeof ( *mesh->t ) );
        } else {
        }// End If

        struct matrix4x4 t_trans;
        struct matrix4x4 t_ro;
        struct matrix4x4 t_scale;
        if ( scale ) {
                get_scale_matrix4x4 ( scale->x, scale->y, scale->z, &t_scale );
        } else {
                get_scale_matrix4x4 ( 1.0f, 1.0f, 1.0f, &t_scale );
        }
        if ( rotation ) {
                get_rotate_xyz_matrix4x4 ( rotation->x, rotation->y, rotation->z, &t_ro );
        } else {
                get_rotate_xyz_matrix4x4 ( 0.0f, 0.0f, 0.0f, &t_ro );
        }
        if ( world_pos ) {
                get_translate_matrix4x4 ( world_pos->x, world_pos->y, world_pos->z, &t_trans );
        } else {
                get_translate_matrix4x4 ( 1.0f, 1.0f, 1.0f, &t_trans );
        }
        struct matrix4x4 t;
        mul_matrix4x4 ( &t_scale, &t_ro, &t );
        mul_matrix4x4 ( &t, &t_trans, &mesh->prop.t_world );
        inv_matrix4x4 ( &mesh->prop.t_world, &mesh->prop.t_object );
        if ( is_trans ) {
                for ( i = 0; i < mesh->num_vertex; i ++ ) {
                        mul_point3d_matrix4x4_u ( &mesh->v[i], &mesh->prop.t_world );
                }
        }
}

void free_triangle_mesh ( struct triangle_mesh_shape *mesh )
{
        free_var ( mesh->v );
        free_var ( mesh->i );
        free_var ( mesh->n );
        free_var ( mesh->t );
        free_var ( mesh->uv );
        free_var ( mesh->tri );
        memset ( mesh, 0, sizeof ( *mesh ) );
}

void set_transform_triangle_mesh ( struct matrix4x4 *t_world, struct matrix4x4 *t_object,
                                   struct triangle_mesh_shape *mesh )
{
        copy_matrix4x4 ( t_world, &mesh->prop.t_world );
        copy_matrix4x4 ( t_object, &mesh->prop.t_object );
}

void transform_to_world_triangle_mesh ( struct triangle_mesh_shape *mesh )
{
        int i;
        for ( i = 0; i < mesh->num_vertex; i ++ ) {
                mul_point3d_matrix4x4_u ( &mesh->v[i], &mesh->prop.t_world );
        }
}

void get_triangle3d_from_shape ( struct triangle_shape *triangle, struct triangle3d *tri_out )
{
        struct triangle_mesh_shape *mesh = triangle->mesh;
        build_triangle3d ( &mesh->v[triangle->i[0]],
                           &mesh->v[triangle->i[1]],
                           &mesh->v[triangle->i[2]], tri_out );
}

void get_triangle4d_from_shape ( struct triangle_shape *triangle, struct triangle4d *tri_out )
{
        struct triangle_mesh_shape *mesh = triangle->mesh;
        build_triangle4d ( &mesh->v[triangle->i[0]],
                           &mesh->v[triangle->i[1]],
                           &mesh->v[triangle->i[2]],
                           tri_out );
}

/* new triangle is required */
void transform_triangle ( struct triangle4d *tri, struct matrix4x4 *transform,
                          struct triangle4d *tri_out )
{
        mul_point4d_matrix4x4 ( &tri->v[0], transform, &tri_out->v[0] );
        mul_point4d_matrix4x4 ( &tri->v[1], transform, &tri_out->v[1] );
        mul_point4d_matrix4x4 ( &tri->v[2], transform, &tri_out->v[2] );

}

void transform_triangle_u ( struct triangle4d *tri, struct matrix4x4 *transform )
{
        mul_point4d_matrix4x4_u ( &tri->v[0], transform );
        mul_point4d_matrix4x4_u ( &tri->v[1], transform );
        mul_point4d_matrix4x4_u ( &tri->v[2], transform );
}

void get_triangle_mesh_bound ( struct triangle_mesh_shape *mesh, struct box3d *bound )
{
        copy_box3d ( &mesh->prop.bound, bound );
}

void get_triangle_mesh_world_bound ( struct triangle_mesh_shape *mesh, struct box3d *bound )
{
        copy_box3d ( &mesh->prop.bound, bound );
}

void get_triangle_bound ( struct triangle_shape *t, struct box3d *bound )
{
        struct triangle3d tri;
        build_triangle3d ( &t->mesh->v[t->i[0]],
                           &t->mesh->v[t->i[1]],
                           &t->mesh->v[t->i[2]], &tri );
        box_triangle3d ( &tri, bound );
}
