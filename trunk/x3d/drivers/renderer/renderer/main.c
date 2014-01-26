/* main.c: Unit tests to all rendering functions go here */
#include "main.h"

#if 0
// Test to draw things onto the frame surface
void DrawFrameSurfaceTest ( void )
{
	FRAME_SURFACE_PTR frameSurface =
		(FRAME_SURFACE_PTR) coreRenderContext[0].rendererContextList.
		rendererContexts[0].renderExit.outputMedia;
	unsigned int *surface = (unsigned int *) frameSurface->backSurface.surface;
	unsigned int width = frameSurface->backSurface.width;
	unsigned int height = frameSurface->backSurface.height;
	int a = coreRenderContext[0].frameStartTime/1000.0f;

	for ( int i = 0; i < height; i ++ ) {
		int r = abs ( (rand () - a)%((height + 1) - rand ()%(i + 1)) );
		for ( int j = 0; j < width; j ++ ) {
			int ca = a*a + 7*a;
			int cb = a*a - 5*a;
			int cc = a*a - 2*a;
			int i1;
			if ( i <= (int)height - r ) {
				i1 = i + r;
			} else {
				i1 = i;
			}
			int j1;
			if ( j <= (int)width - r ) {
				j1 = j + r;
			} else {
				j1 = j;
			}
			int cck = surface[j + i*width]/2 +
				  surface[j + i1*width]/3 +
				  surface[j1 + i*width]/5 +
				  surface[j1 + i1*width]/7;
			FRGB_COLOR fRgb = {(cck+ca)%256/256.0f,
					   (cck+ca)%256/256.0f,
					   (cck+ca)%256/256.0f
					  };
			FRGBToRGB32 ( &fRgb, surface[j1 + i1*width] );
		}// End For

	}// End For

}// End Function DrawFrameSurfaceTest



// Test to draw lines onto the frame surface
void DrawLineToFrameSurfaceTest ( void )
{
	struct point2d p0;
	struct point2d p1;
	FRAME_SURFACE_PTR frameSurface =
		(FRAME_SURFACE_PTR) coreRenderContext[0].rendererContextList.
		rendererContexts[0].renderExit.outputMedia;
	unsigned int width = frameSurface->backSurface.width;
	unsigned int height = frameSurface->backSurface.height;
	int x0 = rand ()%width;
	int y0 = rand ()%height;
	int x1 = rand ()%width;
	int y1 = rand ()%height;
	FRGB_COLOR color;
	InitColor ( (rand ()%255)/255.0f,
		    (rand ()%255)/255.0f,
		    (rand ()%255)/255.0f, &color );

	for ( int i = 0; i < 10000; i ++ ) {
		set_point2d ( x0, y0, &p0 );
		set_point2d ( x1, y1, &p1 );
		x0 = (x0 + x1)%width;
		y0 = (y0 + x0)%height;
		x1 = (x1 + y1)%width;
		y1 = (y1 + y0)%height;
		DrawLineFlat ( &p0, &p1, &color,
			       &frameSurface->backSurface );
	}// End For

}// End Function DrawLineToFrameSurfaceTest



// Test to draw primitives onto the frame surface
void DrawPrimitiveToFrameSurfaceTest ( void )
{
}// End Function DrawPrimitiveToFrameSurfaceTest
#endif
