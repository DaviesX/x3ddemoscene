#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED


#include <system/log.h>
#include <system/allocator.h>

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

#define u_stream_lerp2( _s, _n_stream,  _code) \
{ \
        int _i_stream; \
        for(_i_stream = 0; _i_stream < _n_stream; _i_stream ++) { \
                f_Lerp_2 _f_lerp2 = (_s)[_i_stream].lerp2; \
                _code \
        } \
}

#define u_stream_lerp3( _s, _n_stream,  _code) \
{ \
        int _i_stream; \
        for(_i_stream = 0; _i_stream < _n_stream; _i_stream ++) { \
                f_Lerp_3 _f_lerp3 = (_s)[_i_stream].lerp3; \
                _code \
        } \
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

/** \brief make array-of-structure streams
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

static inline void u_aos_init(struct util_aos* self, enum UtilAttribute format)
{
        zero_obj(self);

        self->index     = alloc_var(sizeof(int), 1);
        self->n_index   = 0;
        self->n_vertex = 0;

        if(format & UtilAttriVertex) {
                self->aos[_AttriVertex].s_data = alloc_var(sizeof(struct point3d), 1);
                self->avail[_AttriVertex] = true;
                self->n_streams ++;
        }
        if(format & UtilAttriNormal) {
                self->aos[_AttriNormal].s_data = alloc_var(sizeof(struct vector3d), 1);
                self->avail[_AttriNormal] = true;
                self->n_streams ++;
        }
        if(format & UtilAttriTangent) {
                self->aos[_AttriTangent].s_data = alloc_var(sizeof(struct vector3d), 1);
                self->avail[_AttriTangent] = true;
                self->n_streams ++;
        }
        if(format & UtilAttriUV) {
                self->aos[_AttriUV].s_data = alloc_var(sizeof(struct vector2d), 1);
                self->avail[_AttriUV] = true;
                self->n_streams ++;
        }
        if (format & UtilAttriMatId) {
                self->aos[_AttriMatId].s_data = alloc_var(sizeof(int), 1);
                self->avail[_AttriMatId] = true;
                self->n_streams ++;
        }
        if (format & UtilAttriMatIdList) {
                self->aos[_AttriMatIdList].s_data = alloc_var(sizeof(int), 1);
                self->avail[_AttriMatIdList] = true;
                self->n_streams ++;
        }

        self->format = format;
}

static inline void u_aos_free(struct util_aos* u_aos)
{
        free_var(u_aos->index);
        u_aos->n_index = 0;
        u_aos->n_vertex = 0;
        int i;
        for (i = 0; i < 10; i ++) {
                free_var(u_aos->aos[i].s_data);
        }
        zero_obj(u_aos);
}

static inline void u_aos_flush(struct util_aos* u_aos)
{
        alloc_var_flush(u_aos->index);
        u_aos->n_index = 0;
        u_aos->n_vertex = 0;
        int i;
        for (i = 0; i < 10; i ++) {
                alloc_var_flush(u_aos->aos[i].s_data);
        }
}

static inline void u_aos_accumulate(struct util_aos* u_aos, int* index, int num_index, int num_vertex, ...)
{
        /* copy vertex attributes */
        va_list v_arg;
        va_start(v_arg, num_vertex);

        enum UtilAttribute format = u_aos->format;
        if(format & UtilAttriVertex) {
                struct point3d* src = va_arg(v_arg, struct point3d*);
                u_aos->aos[_AttriVertex].s_data =
                        alloc_var_add(u_aos->aos[_AttriVertex].s_data, num_vertex);
                struct point3d* v = u_aos->aos[_AttriVertex].s_data;
                memcpy(&v[u_aos->n_vertex], src, num_vertex*sizeof(*v));
        }
        if(format & UtilAttriNormal) {
                struct vector3d* src = va_arg(v_arg, struct vector3d*);
                u_aos->aos[_AttriNormal].s_data =
                        alloc_var_add(u_aos->aos[_AttriNormal].s_data, num_vertex);
                struct vector3d* n = u_aos->aos[_AttriNormal].s_data;
                memcpy(&n[u_aos->n_vertex], src, num_vertex*sizeof(*n));
        }
        if(format & UtilAttriTangent) {
                struct vector3d* src = va_arg(v_arg, struct vector3d*);
                u_aos->aos[_AttriTangent].s_data =
                        alloc_var_add(u_aos->aos[_AttriTangent].s_data, num_vertex);
                struct vector3d* t = u_aos->aos[_AttriTangent].s_data;
                memcpy(&t[u_aos->n_vertex], src, num_vertex*sizeof(*t));
        }
        if(format & UtilAttriUV) {
                struct vector2d* src = va_arg(v_arg, struct vector2d*);
                u_aos->aos[_AttriUV].s_data =
                        alloc_var_add(u_aos->aos[_AttriUV].s_data, num_vertex);
                struct vector2d* uv = u_aos->aos[_AttriUV].s_data;
                memcpy(&uv[u_aos->n_vertex], src, num_vertex*sizeof(*uv));
        }
        if (format & UtilAttriMatId) {
                // material id is set uniformly for each accumulation
                int src = va_arg(v_arg, int);
                u_aos->aos[_AttriMatId].s_data = alloc_var_add(u_aos->aos[_AttriMatId].s_data, num_vertex);
                int* matid = u_aos->aos[_AttriMatId].s_data;
                int i;
                for (i = u_aos->n_index; i < u_aos->n_index + num_vertex; i ++) {
                        matid[i] = src;
                }
        }
        if (format & UtilAttriMatIdList) {
                int* src = va_arg(v_arg, int*);
                u_aos->aos[_AttriMatIdList].s_data = alloc_var_add(u_aos->aos[_AttriMatIdList].s_data, num_vertex);
                int* matid = u_aos->aos[_AttriMatIdList].s_data;
                memcpy(&matid[u_aos->n_vertex], src, num_vertex*sizeof(*matid));
        }

        va_end(v_arg);

        /* copy indices */
        u_aos->index = alloc_var_add(u_aos->index, num_index);
        int* d_index = &u_aos->index[u_aos->n_index];
        int i;
        for(i = 0; i < num_index; i ++) {
                d_index[i] = index[i] + u_aos->n_vertex;
        }

        u_aos->n_index += num_index;
        u_aos->n_vertex += num_vertex;
}

static inline int u_aos_get_vertex(struct util_aos* u_aos, void* vertex[], int* n_streams)
{
        int i;
        for(i = 0; i < 10; i ++) {
                vertex[i] = u_aos->aos[i].s_data;
        }
        *n_streams = u_aos->n_streams;
        return u_aos->n_vertex;
}

static inline void* u_aos_get_index(struct util_aos* u_aos, int* n_index)
{
        *n_index = u_aos->n_index;
        return u_aos->index;
}

static inline bool* u_aos_get_availibility(struct util_aos* u_aos)
{
        return u_aos->avail;
}


enum UtilAccessorType {
        UtilAccessorLinear,
        UtilAccessorBvh
};

typedef bool (*f_Access_Simplex)(int i, struct box3d* simplex, void* data);
typedef bool (*f_Access_Real)(int i, void* data);

/** \brief access utility
 */
struct util_access {
        enum UtilAccessorType   type;
        struct box3d*           simplex;
        int                     n_objects;
};

/** \brief linear access utility
 */
struct util_linear {
        struct util_access      _parent;
};

static void u_linear_init(struct util_linear* linear, enum UtilAccessorType type, 
                           struct box3d* simplex, int n_objects)
{
        linear->_parent.type = type;
        linear->_parent.simplex = simplex;
        linear->_parent.n_objects = n_objects;
}

static void u_linear_free(struct util_linear* linear)
{
        free_fix(linear->_parent.simplex);
        zero_obj(linear);
}

static void u_linear_build(struct util_linear* linear)
{
        return ;        // nothing is needed
}

#define u_linear_find( _acc, _data, _f_access_simplex, _f_access_real) \
{ \
        struct util_access* _p = &(_acc)->_parent; \
        int _i; \
        for(_i = 0; _i < _p->n_objects; _i ++) { \
                if(!_f_access_simplex(_i, &_p->simplex[_i], _data)) \
                        continue; \
                _f_access_real(_i, _data); \
        } \
}

#define u_linear_find2( _acc, _data, _f_access_simplex, _f_access_real, _has_found) \
{ \
        struct util_access* _p = &(_acc)->_parent; \
        int _i; \
        for(_i = 0; _i < _p->n_objects; _i ++) { \
                if(!_f_access_simplex(_i, &_p->simplex[_i], _data)) \
                        continue; \
                if(_f_access_real(_i, _data)) { \
                        _has_found = true; \
                        break; \
                } \
        } \
}

/** \brief bvh subdivide access utility
 */
struct util_bvh {
};

/* utitl_access */
static inline struct util_access* u_access_create (
        enum UtilAccessorType type, struct box3d* simlex, int n_objects)
{
        switch(type) {
        case UtilAccessorLinear:
                {
                struct util_linear* l = alloc_obj(l);
                u_linear_init(l, type, simlex, n_objects);
                return (struct util_access*) l;
                }
        case UtilAccessorBvh:
                {
                return nullptr;
                }
        default:
                {
                log_mild_err_dbg("failed to find such accessor type as %d", type);
                return nullptr;
                }
        }
}

static inline void u_access_free(struct util_access* acc)
{
	if (acc == nullptr) {
		return ;
	}
        switch(acc->type) {
        case UtilAccessorLinear:
                {
                u_linear_free((struct util_linear*) acc);
                free_fix(acc);
                break;
                }
        case UtilAccessorBvh:
                {
                break;
                }
        }
}

static inline void u_access_build(struct util_access* acc)
{
        switch(acc->type) {
        case UtilAccessorLinear:
                u_linear_build((struct util_linear*) acc);
                break;
        case UtilAccessorBvh:
                break;
        }
}

static inline void u_access_find(struct util_access* acc, void* data,
                                   const f_Access_Simplex f_Simplex,
                                   const f_Access_Real f_Real)
{
        switch(acc->type) {
        case UtilAccessorLinear:
                u_linear_find((struct util_linear*) acc, data,
                                f_Simplex, f_Real);
                break;
        case UtilAccessorBvh:
                break;
        }
}

static inline bool u_access_find2(struct util_access* acc, void* data,
                                    const f_Access_Simplex f_Simplex,
                                    const f_Access_Real f_Real)
{
        switch(acc->type) {
        case UtilAccessorLinear:
                {
                bool has_found = false;
                u_linear_find2((struct util_linear*) acc, data,
                                 f_Simplex, f_Real, has_found);
                return has_found;
                }
        case UtilAccessorBvh:
                {
                return false;
                }
        }
}


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
