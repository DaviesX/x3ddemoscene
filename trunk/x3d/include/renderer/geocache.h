#ifndef GEOCACHE_H_INCLUDED
#define GEOCACHE_H_INCLUDED

#include <algorithm.h>
#include <x3d/common.h>
#include <renderer/geometry.h>
#include <x3d/renderer.h>


#define GEOPASS_MAX		32

struct alg_list;
struct geometry;
struct primitive;
struct prim_part;

/*
 * Structures
 */

struct geopass_desc {
        int state;
        bool copy_primitve;
};

struct geopass {
        int state;
        pass_id_t id;
        struct geopass_desc desc;
        struct alg_list geo;
        struct alg_list prim;
        struct prim_part primpart;
};

struct geocache {
        struct geopass pass[GEOPASS_MAX];
        int n_pass;
};

struct geopass_iterator {
        struct geopass pass;
        uint32_t pos;
};

/*
 * Functions' declaration
 */

void init_geocache ( struct geocache *gc );
void free_geocache ( struct geocache *gc );
void geocache_add_pass ( pass_id_t id, struct geopass_desc *desc, struct geocache *gc );
void geocache_remove_pass ( pass_id_t id, struct geocache *gc );
void geocache_build_pass ( pass_id_t id, enum PRIMPART_IDR part_method,
                           int extra_params, struct geocache *gc );
void geocache_add_geometry ( struct geometry *geo, pass_id_t id, struct geocache *gc );
void geocache_add_from_pass ( pass_id_t src_id, pass_id_t dest_id, struct geocache *gc );
void geocache_flush ( pass_id_t id, struct geocache *gc );
struct prim_part *geocache_get_pass_prim ( pass_id_t id, struct geocache *gc );

void geocache_send_passes_desc ( struct geocache *gc, struct alg_list *descs );


#endif // GEOCACHE_H_INCLUDED
