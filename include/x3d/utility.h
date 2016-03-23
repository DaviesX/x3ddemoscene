#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED


#include <system/log.h>
#include <system/allocator.h>
#include <math/math.h>

typedef void (*f_Lerp_2)(void* x[2], float t, void* xo);
typedef void (*f_Lerp_3)(void* x[3], float t[3], void* xo);

/** \brief stream-to-shader connector utility
 */
struct util_stream {
        char*           stream;
        char*           copy_zone;
        char            cache[128];
        void**          dest[16];
        int             sub_size[16];
        int             n_sub;
        int             elm_size;
        int             elm_count;
        f_Lerp_2        lerp2;
        f_Lerp_3        lerp3;
};

static inline void u_stream_init(struct util_stream* s, char* stream, int elm_size, int elm_count,
                                  int sub_size[], void** dest[], int n_sub,
                                  f_Lerp_2 l2, f_Lerp_3 l3)
{
        s->stream       = stream;
        s->elm_size     = elm_size;
        s->elm_count    = elm_count;
        int i;
        for(i = 0; i < n_sub; i ++) {
                s->sub_size[i] = sub_size[i];
                s->dest[i]     = dest[i];
        }
        s->lerp2        = l2;
        s->lerp3        = l3;
        s->n_sub        = n_sub;
        s->copy_zone    = s->cache;
}

static inline void* u_stream_cache_mem(struct util_stream* s)
{
        return s->cache;
}

static inline void u_stream_set_copy_zone(struct util_stream* s, int n_streams, void* cz)
{
        char* cp_zone = cz;
        int i;
        for(i = 0; i < n_streams; i ++) {
                s[i].copy_zone = cp_zone;
                cp_zone += s[i].elm_size;
        }
}

static inline void u_stream_link(struct util_stream s[], int n_stream, int p)
{
        int i;
        for(i = 0; i < n_stream; i ++) {
                char* src = s[i].stream + p*s[i].elm_size;
                int j;
                for(j = 0; j < s[i].n_sub; j ++) {
                        *s[i].dest[j] = src;
                        src += s[i].sub_size[j];
                }
        }
}

static inline void u_stream_lerp2_link(struct util_stream s[], int n_stream, int p[2], float t)
{
        int i;
        for(i = 0; i < n_stream; i ++) {
                char* src[2];
                src[0] = s[i].stream + p[0]*s[i].elm_size;
                src[1] = s[i].stream + p[1]*s[i].elm_size;
                s[i].lerp2((void*) src, t, s[i].copy_zone);
                char* csrc = s[i].copy_zone;
                int j;
                for(j = 0; j < s[i].n_sub; j ++) {
                        *s[i].dest[j] = csrc;
                        csrc += s[i].sub_size[j];
                }
        }
}

static inline void u_stream_lerp3_link(struct util_stream s[], int n_stream, int p[3], float t[3])
{
        int i;
        for(i = 0; i < n_stream; i ++) {
                char* src[3];
                src[0] = s[i].stream + p[0]*s[i].elm_size;
                src[1] = s[i].stream + p[1]*s[i].elm_size;
                src[2] = s[i].stream + p[2]*s[i].elm_size;
                s[i].lerp3((void*) src, t, s[i].copy_zone);
                char* csrc = s[i].copy_zone;
                int j;
                for(j = 0; j < s[i].n_sub; j ++) {
                        *s[i].dest[j] = csrc;
                        csrc += s[i].sub_size[j];
                }
        }
}

static inline void* u_stream_access(struct util_stream* s, int i)
{
        return s->stream + i*s->elm_size;
}

#define u_stream_lerp2( _s, _n_stream,  _code)                                       \
{                                                                                    \
        int _i_stream;                                                               \
        for(_i_stream = 0; _i_stream < _n_stream; _i_stream ++) {                    \
                f_Lerp_2 _f_lerp2 = (_s)[_i_stream].lerp2;                           \
                _code                                                                \
        }                                                                            \
}

#define u_stream_lerp3( _s, _n_stream,  _code)                                       \
{                                                                                    \
        int _i_stream;                                                               \
        for(_i_stream = 0; _i_stream < _n_stream; _i_stream ++) {                    \
                f_Lerp_3 _f_lerp3 = (_s)[_i_stream].lerp3;                           \
                _code                                                                \
        }                                                                            \
}

static inline void u_stream_store_to_buffer(struct util_stream* s, int n_streams, uint8_t* buffer)
{
        int i;
        for(i = 0; i < n_streams; i ++) {
                int j;
                for(j = 0; j < s[i].n_sub; j ++) {
                        memcpy(buffer, *s[i].dest[j], s[i].sub_size[j]);
                        buffer += s[i].sub_size[j];
                        // dest_status[i + j] = *s[i].dest[j];
                }
        }
}

static inline void u_stream_load_from_buffer(struct util_stream* s, int n_streams, uint8_t* buffer)
{
        int i;
        for(i = 0; i < n_streams; i ++) {
                int j;
                for(j = 0; j < s[i].n_sub; j ++) {
                        *s[i].dest[j] = buffer;
//                        memcpy(*s[i].dest[j], buffer, s[i].sub_size[j]);
                        buffer += s[i].sub_size[j];
//                        *s[i].dest[j] = dest_status[i + j];
                }
        }
}

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
int     u_aos_get_vertex(struct util_aos* self, void* vertex[], int* n_streams);
void*   u_aos_get_index(struct util_aos* self, int* n_index);
bool*   u_aos_get_availibility(struct util_aos* self);

#define u_aos_vertex_at(_self, _i) \
        ((struct point3d*) (_self)->aos[_AttriVertex].s_data + (_i))

#define u_aos_index_at(_self, _i) \
        ((_self)->index + (_i))



struct util_access;

enum UtilAccessorType {
        UtilAccessorLinear,
        UtilAccessorBvh
};

typedef bool (*f_Access_Simplex)(int i, struct box3d* simplex, void* data);
typedef bool (*f_Access_Real)(int i, void* data);

typedef void (*f_Util_Access_Build) (struct util_access* self);
typedef void (*f_Util_Access_Free) (struct util_access* self);
/*
 * <util_access> decl
 * provide various ways to access spatial objects.
 */
struct util_access {
        enum UtilAccessorType   type;
        f_Util_Access_Build     f_build;
        f_Util_Access_Build     f_free;
        struct box3d*           simplex;
        int                     n_objects;
};
/*
 * <util_access> public
 */
void            u_access_init(struct util_access* self, enum UtilAccessorType type,
                              f_Util_Access_Build f_build, f_Util_Access_Free f_free);
void            u_access_set_objects(struct util_access* self, struct box3d* simplex, int n_objects);
struct box3d*   u_access_get_objects(struct util_access* self, int* n_objects);
void            u_access_free(struct util_access* self);
void            u_access_build(struct util_access* self);

#define u_access_find(_self, _data, f_Access_Simplex, f_Access_Accurate) \
{\
        switch((_self)->type) {\
        case UtilAccessorLinear:\
                u_linear_find((struct util_linear*) (_self), _data, f_Access_Simplex, f_Access_Accurate);\
                break;\
        case UtilAccessorBvh:\
                break;\
        }\
}

#define u_access_find2(_self, _data, f_Access_Simplex, f_Access_Accurate) \
{ \
        switch((_self)->type) { \
        case UtilAccessorLinear: { \
                u_linear_find2((struct util_linear*) (_self), _data, f_Access_Simplex, f_Access_Accurate); \
                break; \
        } \
        case UtilAccessorBvh: { \
                break; \
        } \
        } \
}

/*
 * <util_linear> decl
 * provides a linaer time access of the spatial objects.
 */
struct util_linear {
        struct util_access      _parent;
};
/*
 * <util_linear> public
 */
void u_linear_init(struct util_linear* self, struct box3d* simplex, int n_objects);
void u_linear_free(struct util_linear* self);
void u_linear_build(struct util_linear* linear);

#define u_linear_find(_self, _data, f_Access_Simplex, f_Access_Accurate)             \
{                                                                                    \
        struct util_access* _p = &(_self)->_parent;                                  \
        int _i;                                                                      \
        for(_i = 0; _i < _p->n_objects; _i ++) {                                     \
                if(!f_Access_Simplex(_i, &_p->simplex[_i], _data))                  \
                        continue;                                                    \
                f_Access_Accurate(_i, _data);                                           \
        }                                                                            \
}

#define u_linear_find2(_self, _data, f_Accesss_Simplex, f_Acesss_Accurate)  \
{                                                                                    \
        struct util_access* _p = &(_self)->_parent;                                   \
        int _i;                                                                      \
        for(_i = 0; _i < _p->n_objects; _i ++) {                                     \
                if(f_Accesss_Simplex(_i, &_p->simplex[_i], _data) && f_Acesss_Accurate(_i, _data))                  \
                        break;                                                    \
        }                                                                            \
}

/*
 * <util_bvh> decl
 * provides a O(log n) time access of the spatial objects through bvh subdivide.
 */
struct util_bvh {
};



enum UtilImageFormat {
        UtilImgRGB24,
        UtilImgRGBA32,
        UtilImgRGBRadiance
};

/** \brief utility for representing in-memory image
 */
struct util_image {
  	int                     nlevel;
  	enum UtilImageFormat    format;
  	int                     width;
  	int                     height;
  	int                     pix_size;
  	int                     stride;
        void*                   data[14];
};

static inline void u_image_init(struct util_image* img, int nlevel, enum UtilImageFormat format,
                                int width, int height)
{
        zero_obj(img);
        img->nlevel = nlevel;
        img->format = format;
        img->width = width;
        img->height = height;

        switch (img->format) {
        case UtilImgRGB24:
                {
                img->pix_size = 3;
                break;
                }
        case UtilImgRGBA32:
                {
                img->pix_size = 4;
                break;
                }
        case UtilImgRGBRadiance:
                {
                img->pix_size = 12;
                break;
                }
        }
        img->stride = img->width*img->pix_size;
}

static inline void u_image_import(struct util_image* img, int nlevel, void* data)
{
}

static inline void u_image_alloc(struct util_image* img, int nlevel)
{
        img->data[nlevel] = alloc_fix(1, img->width*img->height*img->pix_size);
}

static inline void u_image_delete(struct util_image* img, int nlevel)
{
        free_fix(img->data[nlevel]);
}

static inline void u_image_free(struct util_image* img)
{
        int i;
        for(i = 0; i < 14; i ++) {
                free_fix(img->data[i]);
        }
        zero_obj(img);
}

static inline void* u_image_read(struct util_image* img, int nlevel, int x, int y)
{
        return &((uint8_t*) img->data[nlevel])[x*img->pix_size + y*img->stride];
}

static inline void u_image_write(struct util_image* img, int nlevel, int x, int y, void* pix_data)
{
        memcpy(&((uint8_t*) img->data[nlevel])[x*img->pix_size + y*img->stride], pix_data, img->pix_size);
}

static inline void u_image_get_level0_dimension(struct util_image* img, int* width, int* height)
{
        *width  = img->width;
        *height = img->height;
}


#endif // UTILITY_H_INCLUDED
