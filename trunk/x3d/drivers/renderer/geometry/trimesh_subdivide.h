#ifndef X3DTESSELATION_H_INCLUDED
#define X3DTESSELATION_H_INCLUDED


/*
 * Functions' declaration
 */

// Tesselate triangle faces and store the tesselated one to a new mesh
struct triangle_mesh_shape *LoopTesselateTriangleMesh ( struct triangle_mesh_shape *triangleMesh,
		int numberSubdivision );


#endif // X3DREFINEMENT_H_INCLUDED
