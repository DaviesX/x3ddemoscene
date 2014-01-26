#ifndef X3DTESTRENDERING_H_INCLUDED
#define X3DTESTRENDERING_H_INCLUDED


/*
 * Functions' declaration
 */
void dbg_renderer_add_all ( void );
void dbg_vertprocessor_add_all ( void );
void dbg_rasterizer_add_all ( void );

// Test to draw things onto the frame surface
void DrawFrameSurfaceTest ( void );

// Test to draw lines onto the frame surface
void DrawLineToFrameSurfaceTest ( void );

// Test to draw primitives onto the frame surface
void DrawPrimitiveToFrameSurfaceTest ( void );


#endif // X3DTESTRENDERING_H_INCLUDED
