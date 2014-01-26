/* out.c: render output interface */
#include <renderer/out.h>
#include "screen_out.h"
#include "image_out.h"
#include "stream_out.h"
#include "binary_out.h"

/* FIXME (davis#3#): <out> test this module */

struct render_out_ops {
	void (*free) ( struct render_out *ro );
	void (*run) ( struct render_out *ro );
	void (*retrieve) ( void *data, int size, struct render_out *ro );
};

static const struct render_out_ops OutOps[] = {
	[OUT_IDR_SCREEN].free = ro_screen_free,
	[OUT_IDR_SCREEN].run = ro_screen_run,
	[OUT_IDR_SCREEN].retrieve = ro_screen_retrieve
};

void init_render_out ( enum OUT_IDR method, enum OUT_IMPL_IDR impl,
		       struct render_out *ro )
{
	memset ( ro, 0, sizeof *ro );
	ro->idr = method;
	ro->impl = impl;
}

void free_render_out ( struct render_out *ro )
{
	OutOps[ro->idr].free ( ro );
	memset ( ro, 0, sizeof *ro );
}

void render_out_run ( struct render_out *ro )
{
	OutOps[ro->idr].run ( ro );
}

void render_out_retrieve ( void *data, int size, struct render_out *ro )
{
	OutOps[ro->idr].retrieve ( data, size, ro );
}
