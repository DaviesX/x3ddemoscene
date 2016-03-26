#ifndef GEOMCACHE_H_INCLUDED
#define GEOMCACHE_H_INCLUDED

#include <x3d/common.h>

enum UtilAttribute {
        UtilAttriVertex         = 1 << 1,
        UtilAttriNormal         = 1 << 2,
        UtilAttriTangent        = 1 << 3,
        UtilAttriUV             = 1 << 4,
        UtilAttriMatId          = 1 << 5,
        UtilAttriMatIdList      = 1 << 6
};

enum _AttriMap {
        _AttriVertex,
        _AttriNormal,
        _AttriTangent,
        _AttriUV,
        _AttriMatId
};

/*
 * <geomcache> decl
 * array-of-structure streams.
 */
struct geomcache {
        struct {
                void*                   s_data;
        }       aos[10];
        enum UtilAttribute      format;
        bool                    avail[10];
        int                     n_streams;
        int*                    index;
        int                     n_index;
        int                     n_vertex;
};
/*
 * <util_aos> public
 */
void    geomcache_init(struct geomcache* self, enum UtilAttribute format);
void    geomcache_free(struct geomcache* self);
void    geomcache_flush(struct geomcache* self);
void    geomcache_accumulate(struct geomcache* self, int* index, int num_index, int num_vertex, ...);
struct box3d* geomcache_export_simplexes(struct geomcache* self, int* n_objects);
int     geomcache_get_vertices(struct geomcache* self, void* vertex[], int* n_streams);
void*   geomcache_get_indices(struct geomcache* self, int* n_index);
bool*   geomcache_get_availibility(struct geomcache* self);

#define geomcache_num_index(_self) \
        ((_self)->n_index)

#define geomcache_num_vertex(_self) \
        ((_self)->n_vertex)

#define geomcache_has_vertex(_self) \
        ((_self)->avail[_AttriVertex])

#define geomcache_has_normal(_self) \
        ((_self)->avail[_AttriNormal])

#define geomcache_has_uv(_self) \
        ((_self)->avail[_AttriUV])

#define geomcache_has_material_id(_self) \
        ((_self)->avail[_AttriMatId])

#define geomcache_vertex_at(_self, _i) \
        (((struct point3d*) (_self)->aos[_AttriVertex].s_data) + (_i))

#define geomcache_normal_at(_self, _i) \
        (((struct vector3d*) (_self)->aos[_AttriNormal].s_data) + (_i))

#define geomcache_uv_at(_self, _i) \
        (((struct vector2d*) (_self)->aos[_AttriUV].s_data) + (_i))

#define geomcache_material_id_at(_self, _i) \
        (((int*) (_self)->aos[_AttriMatId].s_data) + (_i))

#define geomcache_index_at(_self, _i) \
        ((_self)->index + (_i))


/*
 * <geomcache> tests
 */
#define RED     0
#define GREEN   1
#define BLUE    2
#define WHITE   3
#define MIRROR  4

struct geomcache* geomcache_build_test_sample();


#endif // GEOMCACHE_H_INCLUDED
