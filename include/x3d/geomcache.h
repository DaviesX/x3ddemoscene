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
        _AttriMatId,
        _AttriMatIdList
};

/*
 * <util_aos> decl
 * array-of-structure streams.
 */
struct util_aos {
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
void    u_aos_init(struct util_aos* self, enum UtilAttribute format);
void    u_aos_free(struct util_aos* self);
void    u_aos_flush(struct util_aos* self);
void    u_aos_accumulate(struct util_aos* self, int* index, int num_index, int num_vertex, ...);
int     u_aos_get_vertices(struct util_aos* self, void* vertex[], int* n_streams);
void*   u_aos_get_indices(struct util_aos* self, int* n_index);
bool*   u_aos_get_availibility(struct util_aos* self);

#define u_aos_has_vertex(_self) \
        ((_self)->avail[_AttriVertex])

#define u_aos_has_normal(_self) \
        ((_self)->avail[_AttriNormal])

#define u_aos_has_uv(_self) \
        ((_self)->avail[_AttriUV])

#define u_aos_vertex_at(_self, _i) \
        ((struct point3d*) (_self)->aos[_AttriVertex].s_data + (_i))

#define u_aos_normal_at(_self, _i) \
        ((struct vector3d*) (_self)->aos[_AttriNormal].s_data + (_i))

#define u_aos_uv_at(_self, _i) \
        ((struct vector2d*) (_self)->aos[_AttriUV].s_data + (_i))

#define u_aos_material_id_at(_self, _i) \
        ((int*) (_self)->aos[_AttriMatId].s_data + (_i))

#define u_aos_index_at(_self, _i) \
        ((_self)->index + (_i))


#endif // GEOMCACHE_H_INCLUDED
