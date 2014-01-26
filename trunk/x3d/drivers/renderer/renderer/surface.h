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

void create_surface ( uint32_t width, uint32_t height, enum SURFACE_IDR idr, struct surface *s );
void free_surface ( struct surface *s );
void fill_surface ( pixf_generic *value, struct surface *s );
void fill_pixel ( int x, int y, void *value, struct surface *s );
void gen_pixel_surface ( void *value, pixf_generic *pix, struct surface *s );


#endif // X3DSURFACE_H_INCLUDED
