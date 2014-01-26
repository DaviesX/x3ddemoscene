#include "../Sdk/X3dRenderLibrary.h"
#include "main.h"


#ifndef USING_REAL_MAIN
int main ( int argc, char *argv[] )
{
//    int coreHandle = InitCoreContextX3d ();
//    int rendererId = 0XF35B44DF;
//    SetNewRendererIdX3d ( rendererId, coreHandle );
//    InitRendererX3d ( rendererId, X3D_RENDERER_RASTERIZE, coreHandle );

//	ResourcePilerTest ();
//	HashLookupTest ();
	LoadModelFromFileTest ();
//	AllocateVariableMemoryTest ();

//   ReleaseCoreContextX3d ( coreHandle );
	printf ( "Using test main\n" );
	return 1;

}// End Function main
#endif
