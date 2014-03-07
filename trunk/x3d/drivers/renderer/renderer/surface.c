/* surface.c: surface helper functions all go here */
#include "surface.h"
#include "colorspectrum.h"


struct surface_operations {
        void (*fill_pix) ( int x, int y, void *v, struct surface *s );
        void (*fill_surface) ( void *v, struct surface *s );
        void (*gen_pix) ( void *value, pixf_generic *pix );
};


const int SizeOfPixel[] = {
        [SF_IDR_I8] = sizeof ( pixf_i8 ),
        [SF_IDR_IR5G6B5] = sizeof ( pixf_ir5g6b5 ),
        [SF_IDR_IR8G8B8] = sizeof ( pixf_ir8g8b8 ),
        [SF_IDR_IR8G8B8A8] = sizeof ( pixf_ir8g8b8a8 ),
        [SF_IDR_IA8R8G8B8] = sizeof ( pixf_ia8r8g8b8 ),
        [SF_IDR_F24] = sizeof ( pixf_f24 ),
        [SF_IDR_F32] = sizeof ( pixf_f32 ),
        [SF_IDR_FR10G11B10] = sizeof ( pixf_fr10g11b10 ),
        [SF_IDR_FR10G10B10A2] = sizeof ( pixf_fr10g10b10a2 ),
        [SF_IDR_FR32G32B32] = sizeof ( pixf_fr32g32b32 ),
        [SF_IDR_FR32G32B32A32] = sizeof ( pixf_fr32g32b32a32 )
};

void fill_pix_i8 ( int x, int y, pixf_i8 *v, struct surface *s );
void fill_pix_ir5g6b5 ( int x, int y, pixf_ir5g6b5 *v, struct surface *s );
void fill_pix_ir8g8b8 ( int x, int y, pixf_ir8g8b8 *v, struct surface *s );
void fill_pix_ir8g8b8a8 ( int x, int y, pixf_ir8g8b8a8 *v, struct surface *s );
void fill_pix_f24 ( int x, int y, pixf_f24 *v, struct surface *s );
void fill_pix_f32 ( int x, int y, pixf_f32 *v, struct surface *s );
void fill_pix_f64 ( int x, int y, pixf_f64 *v, struct surface *s );
void fill_pix_fr32g32b32 ( int x, int y, pixf_fr32g32b32 *v, struct surface *s );
void fill_pix_fr32g32b32a32 ( int x, int y, pixf_fr32g32b32a32 *v, struct surface *s );

void fill_surface_i8 ( pixf_generic *pix, struct surface *s );
void fill_surface_ir5g6b5 ( pixf_generic *pix, struct surface *s );
void fill_surface_ir8g8b8 ( pixf_generic *pix, struct surface *s );
void fill_surface_ir8g8b8a8 ( pixf_generic *pix, struct surface *s );
void fill_surface_ia8r8g8b8 ( pixf_generic *pix, struct surface *s );
void fill_surface_f24 ( pixf_generic *pix, struct surface *s );
void fill_surface_f32 ( pixf_generic *pix, struct surface *s );
void fill_surface_f64 ( pixf_generic *pix, struct surface *s );
void fill_surface_fr32g32b32 ( pixf_generic *pix, struct surface *s );
void fill_surface_fr32g32b32a32 ( pixf_generic *pix, struct surface *s );

void rgb_to_ir5g6b5 ( struct float_color3 *c3, pixf_ir5g6b5 *c_out );
void rgb_to_ir8g8b8 ( struct float_color3 *c3, pixf_ir8g8b8 *c_out );
void rgb_to_ir8g8b8a8 ( struct float_color4 *c4, pixf_ir8g8b8a8 *c_out );
void rgb_to_ia8r8g8b8 ( struct float_color4 *c4, pixf_ia8r8g8b8 *c_out );
void rgb_to_fr32g32b32 ( struct float_color3 *c3, pixf_fr32g32b32 *c_out );
void rgb_to_fr32g32b32a32 ( struct float_color4 *c4, pixf_fr32g32b32a32 *c_out );
void float_to_f24 ( float *f, pixf_f24 *v );
void float_to_f32 ( float *f, pixf_f32 *v );
void float_to_f64 ( double *f, pixf_f64 *v );

static void fill_surface ( pixf_generic *pix, struct surface *s );
static void gen_pixel_surface ( void *value, pixf_generic *pix, struct surface *s );


const struct surface_operations SurfOps[] = {
        [SF_IDR_I8].fill_pix = cast(SurfOps->fill_pix) fill_pix_i8,
        [SF_IDR_IR5G6B5].fill_pix = cast(SurfOps->fill_pix) fill_pix_ir5g6b5,
        [SF_IDR_IR8G8B8].fill_pix = cast(SurfOps->fill_pix) fill_pix_ir8g8b8,
        [SF_IDR_IR8G8B8A8].fill_pix = cast(SurfOps->fill_pix) fill_pix_ir8g8b8a8,
        [SF_IDR_IA8R8G8B8].fill_pix = cast(SurfOps->fill_pix) fill_pix_ir8g8b8a8,
        [SF_IDR_F24].fill_pix = cast(SurfOps->fill_pix) fill_pix_f24,
        [SF_IDR_F32].fill_pix = cast(SurfOps->fill_pix) fill_pix_f32,
        [SF_IDR_FR32G32B32].fill_pix = cast(SurfOps->fill_pix) fill_pix_fr32g32b32,
        [SF_IDR_FR32G32B32A32].fill_pix = cast(SurfOps->fill_pix) fill_pix_fr32g32b32a32,

        [SF_IDR_I8].fill_surface = cast(SurfOps->fill_surface) fill_surface_i8,
        [SF_IDR_IR5G6B5].fill_surface = cast(SurfOps->fill_surface) fill_surface_ir5g6b5,
        [SF_IDR_IR8G8B8].fill_surface = cast(SurfOps->fill_surface) fill_surface_ir8g8b8,
        [SF_IDR_IR8G8B8A8].fill_surface = cast(SurfOps->fill_surface) fill_surface_ir8g8b8a8,
        [SF_IDR_IR8G8B8A8].fill_surface = cast(SurfOps->fill_surface) fill_surface_ia8r8g8b8,
        [SF_IDR_F24].fill_surface = cast(SurfOps->fill_surface) fill_surface_f24,
        [SF_IDR_F32].fill_surface = cast(SurfOps->fill_surface) fill_surface_f32,
        [SF_IDR_FR32G32B32].fill_surface = cast(SurfOps->fill_surface) fill_surface_fr32g32b32,
        [SF_IDR_FR32G32B32A32].fill_surface = cast(SurfOps->fill_surface) fill_surface_fr32g32b32a32,

        [SF_IDR_I8].gen_pix = cast(SurfOps->gen_pix) nullptr,
        [SF_IDR_IR5G6B5].gen_pix = cast(SurfOps->gen_pix) rgb_to_ir5g6b5,
        [SF_IDR_IR8G8B8].gen_pix = cast(SurfOps->gen_pix) rgb_to_ir8g8b8,
        [SF_IDR_IR8G8B8A8].gen_pix = cast(SurfOps->gen_pix) rgb_to_ir8g8b8a8,
        [SF_IDR_F24].gen_pix = cast(SurfOps->gen_pix) float_to_f24,
        [SF_IDR_F32].gen_pix = cast(SurfOps->gen_pix) float_to_f32,
        [SF_IDR_FR10G11B10].gen_pix = cast(SurfOps->gen_pix) nullptr,
        [SF_IDR_FR10G10B10A2].gen_pix = cast(SurfOps->gen_pix) nullptr,
        [SF_IDR_FR32G32B32].gen_pix = cast(SurfOps->gen_pix) rgb_to_fr32g32b32,
        [SF_IDR_FR32G32B32A32].gen_pix = cast(SurfOps->gen_pix) rgb_to_fr32g32b32a32
};

struct surface *create_surface ( uint32_t width, uint32_t height, enum SURFACE_IDR idr )
{
        struct surface *s = alloc_fix ( sizeof *s, 1 );
        s->w = width;
        s->h = height;
        s->idr = idr;
        s->num_byte = width*height*SizeOfPixel[idr];
        s->p = width*SizeOfPixel[idr];
        s->s = alloc_fix ( SizeOfPixel[idr], width*height );
        s->ops = &SurfOps[idr];
        s->state = SURFACE_INUSE;
        return s;
}

void free_surface ( struct surface *s )
{
        free_fix ( s->s );
        memset ( s, 0, sizeof ( *s ) );
}

static void gen_pixel_surface ( void *value, pixf_generic *pix, struct surface *s )
{
        s->ops->gen_pix ( value, pix );
}

void rgb_to_ir5g6b5 ( struct float_color3 *c3, pixf_ir5g6b5 *c_out )
{
        c_out->p = ((uint16_t) (c3->b*31.0f) << 11) |
                   ((uint16_t) (c3->g*63.0f) << 6) |
                   ((uint16_t) (c3->r*31.0f));
}

void rgb_to_ir8g8b8 ( struct float_color3 *c3, pixf_ir8g8b8 *c_out )
{
        c_out->p = ((uint32_t) (c3->b*255.0f) << 24) |
                   ((uint32_t) (c3->g*255.0f) << 16) |
                   ((uint32_t) (c3->r*255.0f) << 8);
}

void rgb_to_ir8g8b8a8 ( struct float_color4 *c4, pixf_ir8g8b8a8 *c_out )
{
        c_out->p = ((uint32_t) (c4->b*255.0f) << 24) |
                   ((uint32_t) (c4->g*255.0f) << 16) |
                   ((uint32_t) (c4->r*255.0f) << 8) |
                   ((uint32_t) (c4->a*255.0f));
}

void rgb_to_ia8r8g8b8 ( struct float_color4 *c4, pixf_ia8r8g8b8 *c_out )
{
        c_out->p = ((uint32_t) (c4->a*255.0f) << 24) |
                   ((uint32_t) (c4->r*255.0f) << 16) |
                   ((uint32_t) (c4->g*255.0f) << 8) |
                   ((uint32_t) (c4->b*255.0f));
}

void rgb_to_fr32g32b32 ( struct float_color3 *c3, pixf_fr32g32b32 *c_out )
{
        memcpy ( c_out, c3, sizeof ( *c3 ) );
}

void rgb_to_fr32g32b32a32 ( struct float_color4 *c4, pixf_fr32g32b32a32 *c_out )
{
        memcpy ( c_out, c4, sizeof ( *c4 ) );
}

void float_to_f24 ( float *f, pixf_f24 *v )
{
        memcpy ( v, f, sizeof ( float ) );
}

void float_to_f32 ( float *f, pixf_f32 *v )
{
        memcpy ( v, f, sizeof ( float ) );
}

void float_to_f64 ( double *f, pixf_f64 *v )
{
        memcpy ( v, f, sizeof ( double ) );
}

#define fill_surface_pixel( _idr, _value, _s )	\
{ \
	untyped *_pix = (_s)->s; \
	untyped *_end = (_s)->s + (_s)->num_byte; \
	const uint32_t _size = SizeOfPixel[_idr]; \
	while ( _pix < _end ) { \
		memcpy ( _pix, _value, _size ); \
		_pix += _size; \
	} \
}

void surface_fill_color ( struct float_color3 *color, struct surface *s )
{
        pixf_generic value;
        gen_pixel_surface ( color, &value, s );
        fill_surface ( &value, s );
}

void surface_fill_float ( float value, struct surface *s )
{
        fill_surface ( &value, s );
}

void surface_fill_ubyte ( uint8_t value, struct surface *s )
{
        fill_surface ( &value, s );
}

void surface_get_dimension ( struct surface *s, int *w, int *h )
{
        *w = s->w;
        *h = s->h;
}
void *surface_get_addr ( struct surface *s )
{
        return s->s;
}

int surface_get_pixel_size ( struct surface *s )
{
        return SizeOfPixel[s->idr];
}

static void fill_surface ( pixf_generic *pix, struct surface *s )
{
        s->ops->fill_surface ( pix, s );
}

void fill_surface_i8 ( pixf_generic *pix, struct surface *s )
{
        fill_surface_pixel ( SF_IDR_I8, pix, s );
}

void fill_surface_ir5g6b5 ( pixf_generic *pix, struct surface *s )
{
        fill_surface_pixel ( SF_IDR_IR5G6B5, pix, s );
}

void fill_surface_ir8g8b8 ( pixf_generic *pix, struct surface *s )
{
        fill_surface_pixel ( SF_IDR_IR8G8B8, pix, s );
}

void fill_surface_ir8g8b8a8 ( pixf_generic *pix, struct surface *s )
{
        fill_surface_pixel ( SF_IDR_IR8G8B8A8, pix, s );
}

void fill_surface_ia8r8g8b8 ( pixf_generic *pix, struct surface *s )
{
        fill_surface_pixel ( SF_IDR_IA8R8G8B8, pix, s );
}

void fill_surface_f24 ( pixf_generic *pix, struct surface *s )
{
        fill_surface_pixel ( SF_IDR_F24, pix, s );
}

void fill_surface_f32 ( pixf_generic *pix, struct surface *s )
{
        fill_surface_pixel ( SF_IDR_F32, pix, s );
}

void fill_surface_fr32g32b32 ( pixf_generic *pix, struct surface *s )
{
        fill_surface_pixel ( SF_IDR_FR32G32B32, pix, s );
}

void fill_surface_fr32g32b32a32 ( pixf_generic *pix, struct surface *s )
{
        fill_surface_pixel ( SF_IDR_FR32G32B32A32, pix, s );
}

#undef fill_surface_pixel

void fill_pixel ( int x, int y, void *value, struct surface *s )
{
        s->ops->fill_pix ( x, y, value, s );
}

void fill_pix_i8 ( int x, int y, pixf_i8 *v, struct surface *s )
{
        *(pixf_i8 *) (s->s + x + y*s->p) = *v;
}

void fill_pix_ir5g6b5 ( int x, int y, pixf_ir5g6b5 *v, struct surface *s )
{
        *(pixf_ir5g6b5 *) (s->s + x + y*s->p) = *v;
}

void fill_pix_ir8g8b8 ( int x, int y, pixf_ir8g8b8 *v, struct surface *s )
{

        *(uint32_t *) &s->s[x + y*s->p] = 0XFF000000;
        *(uint32_t *) &s->s[x + y*s->p] |= v->p >> 8;
}

void fill_pix_ir8g8b8a8 ( int x, int y, pixf_ir8g8b8a8 *v, struct surface *s )
{
        *(pixf_ir8g8b8a8 *) (s->s + x + y*s->p) = *v;
}

void fill_pix_f24 ( int x, int y, pixf_f24 *v, struct surface *s )
{
        *(pixf_f24 *) (s->s + x + y*s->p) = *v;
}

void fill_pix_f32 ( int x, int y, pixf_f32 *v, struct surface *s )
{
        *(pixf_f32 *) (s->s + x + y*s->p) = *v;
}

void fill_pix_f64 ( int x, int y, pixf_f64 *v, struct surface *s )
{
        *(pixf_f64 *) (s->s + x + y*s->p) = *v;
}

void fill_pix_fr32g32b32 ( int x, int y, pixf_fr32g32b32 *v, struct surface *s )
{
        *(pixf_fr32g32b32 *) (s->s + x + y*s->p) = *v;
}

void fill_pix_fr32g32b32a32 ( int x, int y, pixf_fr32g32b32a32 *v, struct surface *s )
{
        *(pixf_fr32g32b32a32 *) (s->s + x + y*s->p) = *v;
}
