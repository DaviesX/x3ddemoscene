#ifndef SPATIALACCESS_H_INCLUDED
#define SPATIALACCESS_H_INCLUDED

#include <math/math.h>

struct spatial_access;

enum UtilAccessorType {
        UtilAccessorLinear,
        UtilAccessorBvh
};

typedef bool (*f_Access_Simplex)(int i, struct box3d* simplex, void* data);
typedef bool (*f_Access_Real)(int i, void* data);

typedef void (*f_Util_Access_Build) (struct spatial_access* self);
typedef void (*f_Util_Access_Free) (struct spatial_access* self);
/*
 * <spatial_access> decl
 * provide various ways to access spatial objects.
 */
struct spatial_access {
        enum UtilAccessorType   type;
        f_Util_Access_Build     f_build;
        f_Util_Access_Build     f_free;
        struct box3d*           simplex;
        int                     n_objects;
};
/*
 * <spatial_access> public
 */
void            u_access_init(struct spatial_access* self, enum UtilAccessorType type,
                              f_Util_Access_Build f_build, f_Util_Access_Free f_free);
void            u_access_set_objects(struct spatial_access* self, struct box3d* simplex, int n_objects);
struct box3d*   u_access_get_objects(struct spatial_access* self, int* n_objects);
void            u_access_free(struct spatial_access* self);
void            u_access_build(struct spatial_access* self);

#define u_access_find(_self, _data, f_Access_Simplex, f_Access_Accurate) \
{\
        switch((_self)->type) {\
        case UtilAccessorLinear:\
                u_linear_find((struct spatial_linear*) (_self), _data, f_Access_Simplex, f_Access_Accurate);\
                break;\
        case UtilAccessorBvh:\
                break;\
        }\
}

#define u_access_find2(_self, _data, f_Access_Simplex, f_Access_Accurate) \
{ \
        switch((_self)->type) { \
        case UtilAccessorLinear: { \
                u_linear_find2((struct spatial_linear*) (_self), _data, f_Access_Simplex, f_Access_Accurate); \
                break; \
        } \
        case UtilAccessorBvh: { \
                break; \
        } \
        } \
}

/*
 * <spatial_linear> decl
 * provides a linaer time access of the spatial objects.
 */
struct spatial_linear {
        struct spatial_access      _parent;
};
/*
 * <spatial_linear> public
 */
void u_linear_init(struct spatial_linear* self, struct box3d* simplex, int n_objects);
void u_linear_free(struct spatial_linear* self);
void u_linear_build(struct spatial_linear* linear);

#define u_linear_find(_self, _data, f_Access_Simplex, f_Access_Accurate)             \
{                                                                                    \
        struct spatial_access* _p = &(_self)->_parent;                                  \
        int _i;                                                                      \
        for(_i = 0; _i < _p->n_objects; _i ++) {                                     \
                if(!f_Access_Simplex(_i, &_p->simplex[_i], _data))                  \
                        continue;                                                    \
                f_Access_Accurate(_i, _data);                                           \
        }                                                                            \
}

#define u_linear_find2(_self, _data, f_Accesss_Simplex, f_Acesss_Accurate)  \
{                                                                                    \
        struct spatial_access* _p = &(_self)->_parent;                                   \
        int _i;                                                                      \
        for(_i = 0; _i < _p->n_objects; _i ++) {                                     \
                if(f_Accesss_Simplex(_i, &_p->simplex[_i], _data) && f_Acesss_Accurate(_i, _data))                  \
                        break;                                                    \
        }                                                                            \
}

/*
 * <spatial_bvh> decl
 * provides a O(log n) time access of the spatial objects through bvh subdivide.
 */
struct spatial_bvh {
};


#endif // SPATIALACCESS_H_INCLUDED
