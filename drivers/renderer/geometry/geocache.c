/* geocache.c: Overall scene definitions and management all go here */
#include <system/log.h>
#include <renderer/geometry.h>
#include <renderer/geocache.h>
#include "shape_common.h"

#define GEOPASS_NOT_BUILT	0
#define GEOPASS_BUILT		1

#define GEOPASS_NO_DESC		0
#define GEOPASS_HAS_DESC	1


void init_geocache ( struct geocache *gc )
{
        memset ( gc, 0, sizeof ( *gc ) );
        int i;
        for ( i = 0; i < GEOPASS_MAX; i ++ ) {
                gc->pass[i].id = -1;
                gc->pass[i].state = GEOPASS_NOT_BUILT;
                gc->pass[i].desc.state = GEOPASS_NO_DESC;
        }
}

void free_geocache ( struct geocache *gc )
{
        int i;
        for ( i = 0; i < GEOPASS_MAX; i ++ ) {
                geocache_remove_pass ( i, gc );
        }
        memset ( gc, 0, sizeof ( *gc ) );
}

void geocache_add_pass ( uuid_t id, struct geopass_desc *desc, struct geocache *gc )
{
        if ( gc->pass[id].id != -1 ) {
                log_severe_err_dbg ( "overriding existing pass <%d> with <%d>",
                                     gc->pass[id].id, id );
                geocache_remove_pass ( id, gc );
        }
        if ( gc->n_pass == GEOPASS_MAX ) {
                log_critical_err_dbg ( "geocache overflow. num_pass: %d. max_pass: %d",
                                       gc->n_pass, GEOPASS_MAX );
                return ;
        }
        gc->pass[id].id = id;
        memcpy ( &gc->pass[id].desc, desc, sizeof ( *desc ) );
        gc->pass[id].desc.state = GEOPASS_HAS_DESC;
        alg_list_init ( &gc->pass[id].geo, sizeof ( struct geometry ), 0 );
        alg_list_init ( &gc->pass[id].prim, sizeof ( struct primitive ), 0 );
        gc->n_pass ++;
}

void geocache_remove_pass ( uuid_t id, struct geocache *gc )
{
        if ( gc->pass[id].id == -1 ) {
                log_normal_dbg ( "pass: %d had already been removed", id );
                return ;
        }
        alg_list_free ( &gc->pass[id].geo );
        alg_list_free ( &gc->pass[id].prim );
        free_primitive_partition ( &gc->pass[id].primpart );
        gc->pass[id].id = -1;
        gc->pass[id].state = GEOPASS_NOT_BUILT;
        gc->n_pass --;
}

void geocache_build_pass ( uuid_t id, enum PRIMPART_IDR part_method,
                           int extra_params, struct geocache *gc )
{
        if ( gc->pass[id].desc.state != GEOPASS_HAS_DESC ) {
                log_critical_err_dbg ( "This pass: %d has no description !", id );
                return ;
        }
        build_primitive_partition ( &gc->pass[id].prim, part_method, extra_params,
                                    &gc->pass[id].primpart );
}

void geocache_add_geometry ( struct geometry *geo, uuid_t id, struct geocache *gc )
{
        alg_push_back ( list, &gc->pass[id].geo, geo );
        if ( gc->pass[id].desc.copy_primitve ) {
                struct shape geo_shape;
                geometry_get_shape ( geo, &geo_shape );
                shape_get_primitives ( &geo_shape, &gc->pass[id].prim );
        }
}

void geocache_add_from_pass ( uuid_t src_id, uuid_t dest_id, struct geocache *gc )
{
        memcpy ( &gc->pass[dest_id], &gc->pass[src_id], sizeof ( struct geopass ) );
}

void geocache_flush ( uuid_t id, struct geocache *gc )
{
}

struct prim_part *geocache_get_pass_prim ( uuid_t id, struct geocache *gc )
{
        return &gc->pass[id].primpart;
}

void geocache_send_passes_desc ( struct geocache *gc, struct alg_list *descs )
{
}
