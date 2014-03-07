/* rtpass.c: Pack up data required for a raster pass */
#include "rtpass.h"


#if 0
void get_pass_context ( int passType, struct pass_context *pc )
{
}

void push_prim_list_to_vibuffer ( struct alg_list *prim_list, struct pass_context *pc,
                                  struct vertex_buffer *vb, struct index_buffer *ib )
{
        set_ibuffer_size ( alg_list_len ( prim_list )*3, ib );
        struct triangle_mesh_shape **tmp_mesh =
                alloc_var ( sizeof ( struct triangle_mesh_shape* ), 0 );

#define UNTOUCHED		-1
        /* First I mark all the triangle mesh to be untouched, and then I traverse each
         * primitive to find out all first-seen meshes and put them to a temporary array tmp_mesh
         * A new index space is given each time we see a new mesh*/
        int i;
        for ( i = 0; i < alg_list_len ( prim_list ); i ++ ) {
                struct primitive *p = alg_list_i ( prim_list, i );
                struct triangle_mesh_shape *mesh = p->tri_prim.mesh;
                int *mark = get_obj_preserve ( &mesh->preserve, int, 0 );
                *mark = UNTOUCHED;
        }

        int start_off = 0;
        int nmesh = 0;
        for ( i = 0; i < alg_list_len ( prim_list ); i ++ ) {
                struct primitive *p = alg_list_i ( prim_list, i );
                struct triangle_mesh_shape *mesh = p->tri_prim.mesh;
                int *mark = get_obj_preserve ( &mesh->preserve, int, 0 );

                if ( *mark == UNTOUCHED ) {
                        /* Newly found mesh will be put in the temporary array
                         * Besides, I also mark down the start offset of the mesh's index */
                        *mark = start_off;
                        start_off += mesh->num_vertex;
                        nmesh ++;
                        tmp_mesh = push_var ( mesh, tmp_mesh );
                }
        }

        /* Push vertices into the index buffer and add the offset just found accordingly */
        for ( i = 0; i < alg_list_len ( prim_list ); i ++ ) {
                struct primitive *p = alg_list_i ( prim_list, i );
                struct triangle_mesh_shape *mesh = p->tri_prim.mesh;
                int *offset = get_obj_preserve ( &mesh->preserve, int, 0 );
                int j;
                for ( j = 0; j < 3; j ++ ) {
                        ib->index[i*3 + j] = p->tri_prim.i[j] + *offset;
                }
        }

        /* Push vertex attributes into the vertex buffer */
        int nv = 0;
        for ( i = 0; i < nmesh; i ++ ) {
                struct raster_vertex *v = get_vertex_from_vbuffer ( nv, vb );
                nv += tmp_mesh[i]->num_vertex;
                set_vbuffer_size ( nv, vb );
                int j;
                for ( j = 0; j < tmp_mesh[i]->num_vertex; j ++ ) {
                        struct raster_vertex *next = get_next_vbuffer ( v, vb );

                        if ( pc->vmask & VMK_POSITION ) {
                                struct point3d *p = get_vertex_attr ( VATR_POSITION, v, vb );
                                copy_point3d ( &tmp_mesh[i]->v[j], p );
                        }

                        if ( pc->vmask & VMK_NORMAL ) {
                                struct vector3d *n = get_vertex_attr ( VATR_NORMAL, v, vb );
                                copy_vector3d ( &tmp_mesh[i]->n[j], n );
                        }

                        if ( pc->vmask & VMK_TANGENT ) {
                                struct vector3d *t = get_vertex_attr ( VATR_TANGENT, v, vb );
                                copy_vector3d ( &tmp_mesh[i]->t[j], t );
                        }

                        if ( pc->vmask & VMK_TEXCOOR ) {
                                struct vector2d *uv = get_vertex_attr ( VATR_TEXCOOR, v, vb );
                                copy_point2d ( &tmp_mesh[i]->uv[j], uv );
                        }
                        v = next;
                }
        }
#undef UNTOUCHED
}
#endif
