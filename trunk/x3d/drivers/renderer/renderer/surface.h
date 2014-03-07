#ifndef X3DSURFACE_H_INCLUDED
#define X3DSURFACE_H_INCLUDED

#include <x3d/common.h>

/*
 * Definitions
 */
#define SURFACE_UNUSE			0
#define SURFACE_INUSE			1
#define SURFACE_MAX_TYPE		10

enum SURFACE_IDR {
        SF_IDR_I8,
        SF_IDR_IR5G6B5,
        SF_IDR_IR8G8B8,
        SF_IDR_IR8G8B8A8,
        SF_IDR_IA8R8G8B8,
        SF_IDR_F24,
        SF_IDR_F32,
        SF_IDR_FR10G11B10,
        SF_IDR_FR10G10B10A2,
        SF_IDR_FR32G32B32,
        SF_IDR_FR32G32B32A32
};

/*
 * Constants
 */

/*
 * Types
 */

typedef struct {
        uint8_t p;
} pixf_i8;

typedef struct {
        uint16_t p;
} pixf_ir5g6b5;

typedef struct {
        uint32_t p;
} pixf_ir8g8b8;

typedef struct {
        uint32_t p;
} pixf_ir8g8b8a8, pixf_ia8r8g8b8;

typedef struct {
        float p;
} pixf_f24;

typedef struct {
        float p;
} pixf_f32;

typedef struct {
        double p;
} pixf_f64;

typedef struct {
        float p;
} pixf_fr10g11b10, pixf_fr10g10b10a2;

typedef struct {
        float p[3];
} pixf_fr32g32b32;

typedef struct {
        float p[4];
} pixf_fr32g32b32a32;

typedef struct {
        float p[4];
} pixf_generic;

/* typedef struct pixf_f256 pixf_generic; */

struct float_color3;
struct float_color4;


/*
 * Structures
 */

struct surface {
        struct surface_operations *ops;
        int state;
        enum SURFACE_IDR idr;
        int w, h;
        int p;
        int num_byte;
        untyped *s;
};


/*
 * Functions' declaration
 */

struct surface *create_surface ( uint32_t width, uint32_t height, enum SURFACE_IDR idr );
void free_surface ( struct surface *s );
void surface_fill_color ( struct float_color3 *color, struct surface *s );
void surface_fill_float ( float value, struct surface *s );
void surface_fill_ubyte ( uint8_t value, struct surface *s );
void surface_get_dimension ( struct surface *s, int *w, int *h );
void *surface_get_addr ( struct surface *s );
int surface_get_pixel_size ( struct surface *s );

void rgb_to_ir5g6b5 ( struct float_color3 *c3, pixf_ir5g6b5 *c_out );
void rgb_to_ir8g8b8 ( struct float_color3 *c3, pixf_ir8g8b8 *c_out );
void rgb_to_ir8g8b8a8 ( struct float_color4 *c4, pixf_ir8g8b8a8 *c_out );
void rgb_to_ia8r8g8b8 ( struct float_color4 *c4, pixf_ia8r8g8b8 *c_out );
void rgb_to_fr32g32b32 ( struct float_color3 *c3, pixf_fr32g32b32 *c_out );
void rgb_to_fr32g32b32a32 ( struct float_color4 *c4, pixf_fr32g32b32a32 *c_out );
void float_to_f24 ( float *f, pixf_f24 *v );
void float_to_f32 ( float *f, pixf_f32 *v );
void float_to_f64 ( double *f, pixf_f64 *v );


#endif // X3DSURFACE_H_INCLUDED
