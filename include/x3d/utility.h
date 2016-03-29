#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED


#include <system/log.h>
#include <system/allocator.h>
#include <math/math.h>
#include <x3d/colorspectrum.h>

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


#endif // UTILITY_H_INCLUDED
