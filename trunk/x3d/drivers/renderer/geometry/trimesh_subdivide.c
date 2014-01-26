/* trimesh_subdivide.c: triangle mesh subdivide algorithm */
#include <math/math.h>
#include <renderer/geometry.h>
#include "shape_common.h"
#include "trimesh_subdivide.h"


#define MAX_VALENCE					256
#define REGULAR_VALENCE					6
#define REGULAR_VALENCE_BOUNDRY				4

struct tesselate_face;
struct tesselate_vertex;

// Triangle vertex structure for triangle tesselation
struct tesselate_vertex {
	struct point3d vertex;
	struct tesselate_face *startFace;		/** FIXME: Uses index instead of pointer **/
	struct tesselate_vertex *childVertex;
	uint16_t valence;
	uint8_t regular;
	uint8_t boundary;
};

// Triangle face structure for trianglge tesselation
struct tesselate_face {
	struct tesselate_vertex *tessVertex[3];
	struct tesselate_face *tessFace[3];
	struct tesselate_face *childFace[4];
};

// Stores an edge that shared by two faces for triangle tesselation
struct tesselate_edge {
	struct tesselate_vertex *tessVertex[2];
	struct tesselate_face *tessFace[2];
	int f0EdgeIndex;
};


void InitTesselateGeometry ( struct triangle_mesh_shape *triangleMesh,
			     struct tesselate_vertex **_tessVertex,
			     struct tesselate_face **_tessFace );
struct triangle_mesh_shape *ConvertTessToTriangleMesh ( struct tesselate_vertex *tessVertex, int numberVertex,
		struct tesselate_face *tessFace, int numberTriangle,
		struct vector3d *normals, struct triangle_mesh_shape *oldTriangleMesh );
struct tesselate_edge *FindEdge ( struct tesselate_edge * cTessEdge, struct tesselate_edge * tessEdges, int numberEdge );
void AddEdge ( struct tesselate_edge *newEdge, struct tesselate_edge **tessEdges, int *numberEdge );
void DeleteEdge ( struct tesselate_edge *sEdge, struct tesselate_edge *tessEdges, int *numberEdge );


#define NextVert(vert)		(((vert) + 1)%3)
#define PrevVert(vert)		(((vert) + 2)%3)

#define GetNextFaceCCW(face, nextFace, edgeIndex)	\
{\
	struct tesselate_face * startFace = (face);\
\
	/* Restore edge index */\
	int iSurrVert;\
\
	for ( iSurrVert = 0; iSurrVert < 3; iSurrVert ++ )\
	{\
		if ( startFace->tessVertex[iSurrVert] ==\
			currTessVertex )\
			break;\
\
	}/* End For ( Each surrounding vertex ) */\
\
	(edgeIndex) = iSurrVert;\
	(nextFace) = startFace->tessFace[PrevVert ( iSurrVert )];\
}

#define GetNextFaceCW(face, nextFace, edgeIndex)	\
{\
	struct tesselate_face * startFace = (face);\
\
	/* Restore edge index */\
	int iSurrVert;\
\
	for ( iSurrVert = 0; iSurrVert < 3; iSurrVert ++ )\
	{\
		if ( startFace->tessVertex[iSurrVert] ==\
			currTessVertex )\
			break;\
\
	}/* End For ( Each surrounding vertex ) */\
\
	(edgeIndex) = iSurrVert;\
	(nextFace) = startFace->tessFace[iSurrVert];\
}

#define GetRegularBeta(beta)	\
{\
	(beta) = 1.0f/16.0f;\
}

#define GetIrregularBeta(beta, valence)	\
{\
	if ( (valence) == 3 )\
		(beta) = 3.0f/16.0f;\
	else\
		(beta) = 3.0f/(8.0f*(valence));\
}

#define GetBoundaryBeta(beta)	\
{\
	(beta) = 1.0f/8.0f;\
}

#define GetOtherVertex(face, v0, v1, otherVert)	\
{\
	int _iVertex;\
	for ( _iVertex = 0; _iVertex < 3; _iVertex ++ )\
	{\
		struct tesselate_vertex * currTessVert =\
			(face)->tessVertex[_iVertex];\
\
		/* Vertex other than v0 and v1 ? */\
		if ( currTessVert != (v0) &&\
			currTessVert != (v1) )\
		{\
			(otherVert) = currTessVert;\
			break;\
\
		}/* End If */\
\
	}/* End For ( Each vertex ) */\
}

#define GetFaceVertexIndex(face, vertex, index)	\
{\
	int _iVertex;\
	for ( _iVertex = 0; _iVertex < 3; _iVertex ++ )\
	{\
		if ( (face)->tessVertex[_iVertex] == (vertex) )\
		{\
			(index) = _iVertex;\
			break;\
\
		}/* End If ( Same vertex ? ) */\
\
	}/* End For ( Each surrounding vertices ) */\
}\

#define GetGamma(valence, gamma)	\
{\
	/* gamma = 1/(valence + 3/(8*beta)) */\
	if ( valence == 3 )\
		gamma = 1.0f/2.0f;\
	else\
		gamma = 1.0f/(2.0f*valence);\
}


// Tesselate triangle faces and store the tesselated one to a new mesh
struct triangle_mesh_shape *LoopTesselateTriangleMesh ( struct triangle_mesh_shape *triangleMesh, int numberSubdivision )
{
	struct tesselate_vertex * tessVertex;
	struct tesselate_face * tessFace;

	InitTesselateGeometry ( triangleMesh, &tessVertex, &tessFace );
	int numberVertex = triangleMesh->num_vertex;
	int numberFace = triangleMesh->num_tri;

	// Loop to tesselate
	int iSubdiv;
	for ( iSubdiv = 0; iSubdiv < numberSubdivision; iSubdiv ++ ) {
		struct tesselate_vertex *childTessVertex =
			alloc_var ( sizeof ( *childTessVertex ), 0 );

		struct tesselate_face *childTessFace =
			alloc_var ( sizeof ( *childTessFace ), 0 );

		int numberNewVertex = 0;

		// Initialize next levels' vertices
		int iVertex;
		for ( iVertex = 0; iVertex < numberVertex; iVertex ++ ) {
			childTessVertex = add_var ( childTessVertex, 1 );
			struct tesselate_vertex *currTessVertex = &tessVertex[iVertex];
			struct tesselate_vertex *childVertex = get_var_last ( childTessVertex );

			// The property of a tesselated vertex
			// will be identical to the current one
			childVertex->boundary = currTessVertex->boundary;
			childVertex->regular = currTessVertex->regular;
			childVertex->valence = currTessVertex->valence;
			currTessVertex->childVertex = childVertex;

		}// End For ( Each vertex )

		// Initialize next levels' faces
		int iFace;
		for ( iFace = 0; iFace < numberFace; iFace ++ ) {
			struct tesselate_face * currTessFace = &tessFace[iFace];
			int iChildFace;
			for ( iChildFace = 0; iChildFace < 4; iChildFace ++ ) {
				childTessFace = add_var ( childTessFace, 1 );
				currTessFace->childFace[iChildFace] = get_var_last ( childTessFace );
			}

		}// End For ( Each face )

		// Compute even ( original ) vertices' new position
		for ( iVertex = 0; iVertex < numberVertex; iVertex ++ ) {
			struct tesselate_vertex * currTessVertex = &tessVertex[iVertex];
			float vertBeta;
			struct point3d newVert;

			if ( !currTessVertex->boundary ) {
				if ( currTessVertex->regular ) {
					GetRegularBeta ( vertBeta );
				} else {
					GetIrregularBeta ( vertBeta, currTessVertex->valence );

				}// End If ( Get beta )

				// V' = (1 - n*beta)*V0 + V1*beta + ... + Vn*beta
				// where n = valence - 1
				// V0 itself, (1 - n*beta)*V0
				scale_point3d ( 1.0f - currTessVertex->valence*vertBeta,
						&currTessVertex->vertex, &newVert );

				// Then the rest of the surrounding vertices
				struct tesselate_face * face = currTessVertex->startFace;
				struct tesselate_face * nextFace;
				int edgeIndex;
				GetFaceVertexIndex ( face, currTessVertex, edgeIndex );

				while ( 1 ) {
					// beta*V1 + beta*V2 + ... + beta*Vn
					struct point3d * surrVert =
						&face->tessVertex[PrevVert ( edgeIndex )]->vertex;
					struct point3d tempPoint;
					scale_point3d ( vertBeta, surrVert, &tempPoint );
					add_point3d_u ( &newVert, &tempPoint );

					if ( nextFace == currTessVertex->startFace ) {
						break;
					}

					GetNextFaceCCW ( face, nextFace, edgeIndex );
					face = nextFace;

				}// End While ( Loop through all neighboring faces )
			} else {
				GetBoundaryBeta ( vertBeta );

				// V' = (1 - 2*beta)*V0 + beta*V1 + beta*V2
				scale_point3d ( 1.0f - 2.0f*vertBeta, &currTessVertex->vertex, &newVert );

				// Find V1
				struct tesselate_face * face = currTessVertex->startFace;
				struct tesselate_face * nextFace;
				int edgeIndex;

				while ( 1 ) {
					GetNextFaceCCW ( face, nextFace, edgeIndex );
					if ( !nextFace ) {
						break;
					}
					face = nextFace;
				}// End While ( Loop to the margin )

				// beta*V1
//				struct point3d tempPoint;
				sscale_add_point3d_u ( &newVert, vertBeta,
						       &face->tessVertex[PrevVert ( edgeIndex )]->vertex );

				// Find V2
				while ( 1 ) {
					GetNextFaceCW ( face, nextFace, edgeIndex );
					if ( !nextFace ) {
						break;
					}
					face = nextFace;
				}// End While ( Loop to the margin )

				// beta*V1
				sscale_add_point3d_u ( &newVert, vertBeta,
						       &face->tessVertex[NextVert ( edgeIndex )]->vertex );
			}// End If ( Boundary ? )

			// Update child vertex with newly computed even vertex
			copy_point3d ( &newVert, &currTessVertex->childVertex[iVertex].vertex );

		}// End For ( Each even vertex )

		// Compute odd vertices' vertex position
		int numberEdge = 0;
		struct tesselate_edge *tessEdge;
		alloc_var ( sizeof ( *tessEdge ), 0 );

		for ( iFace = 0; iFace < numberFace; iFace ++ ) {
			struct tesselate_face * currTessFace = &tessFace[iFace];
			int iEdge;
			for ( iEdge = 0; iEdge < 3; iEdge ++ ) {
				struct tesselate_vertex *currTessVertex =
							tessFace->tessVertex[iEdge];

				// Construct current edge
				/* int *iVertex = &iEdge; */
				struct tesselate_edge currEdge;
				currEdge.tessVertex[0] =
					min ( currTessFace->tessVertex[iEdge],
					      currTessFace->tessVertex[NextVert ( iEdge )] );
				currEdge.tessVertex[1] =
					max ( currTessFace->tessVertex[iEdge],
					      currTessFace->tessVertex[NextVert ( iEdge )] );

				if ( !FindEdge ( &currEdge, tessEdge, numberEdge ) ) {
					AddEdge ( &currEdge, &tessEdge, &numberEdge );

					// Tesselated new face will always be regular,
					// and it is sure that the central face should be its start face,
					// and we let the fourth one be the central face
					struct tesselate_vertex newVertex;
					newVertex.regular = 1;
					newVertex.boundary = currTessVertex->boundary;
					newVertex.startFace = currTessFace->tessFace[3];

					struct point3d newVert;

					if ( newVertex.boundary ) {
						// 1/2*V0 + 1/2*V1
						scale_point3d ( 1.0f/2.0f, &currEdge.tessVertex[0]->vertex,
								&newVert );
						struct point3d tempPoint;
						scale_point3d ( 1.0f/2.0f, &currEdge.tessVertex[1]->vertex,
								&tempPoint );
						add_point3d_u ( &newVert, &tempPoint );

						newVertex.valence = REGULAR_VALENCE_BOUNDRY;
					} else {
						// 3/8*V0 + 3/8*V1 + 1/8*V2 + 1/8*V3
						// where V2, V3 are the other vertices of different direction
						// of the current processing edge
						scale_point3d ( 3.0f/8.0f, &currEdge.tessVertex[0]->vertex,
								&newVert );
						struct point3d tempPoint;
						scale_point3d ( 3.0f/8.0f, &currEdge.tessVertex[1]->vertex,
								&tempPoint );
						add_point3d_u ( &newVert, &tempPoint );

						struct tesselate_vertex *vert2;
						struct tesselate_vertex *vert3;
						GetOtherVertex ( currTessFace,
								 currEdge.tessVertex[0],
								 currEdge.tessVertex[1],
								 vert2 );
						GetOtherVertex ( currTessFace->childFace[iEdge],
								 currEdge.tessVertex[0],
								 currEdge.tessVertex[1],
								 vert3 );

						// 1/8*V2 + 1/8*V3
						dscale_add_point3d ( 1.0f/8.0f, &vert2->vertex,
								     1.0f/8.0f, &vert3->vertex, &newVert );
						newVertex.valence = REGULAR_VALENCE;
					}// End If ( Boundary ? )

					// Update odd vertex
					copy_point3d ( &newVert, &newVertex.vertex );
					childTessVertex = push_var ( &newVert, childTessVertex );
					numberNewVertex ++;

					// Update the central triangle too
					currTessFace->childFace[3]->tessVertex[iEdge] = &childTessVertex[numberNewVertex];
				} else {
					// Although this odd vertex has already been computed,
					// we still have to update the vertex of the central triangle
					// so we can fix the other child faces' vertex in the subsequent process
					// Restore neighbor's vertex index
					int iNeighborIndex;
					struct tesselate_face * neighborFace =
								currTessFace->tessFace[iEdge];

					GetFaceVertexIndex ( neighborFace,
							     currTessFace->tessVertex[iEdge],
							     iNeighborIndex );

					currTessFace->childFace[3]->tessVertex[iEdge] =
						neighborFace->childFace[3]->tessVertex[iNeighborIndex];

				}// End If ( Never seen before ? )

			}// End For ( Each child face )

		}// End For ( Each face )

		// Update new mesh's topology information
		// Update even vertices' face pointer
		for ( iVertex = 0; iVertex < numberVertex; iVertex ++ ) {
			struct tesselate_vertex * currTessVertex = &tessVertex[iVertex];
			struct tesselate_face * vStartFace = currTessVertex->startFace;

			// Restore vertex index
			int iEdge;
			GetFaceVertexIndex ( vStartFace, currTessVertex, iEdge );

			currTessVertex->childVertex->startFace =
				vStartFace->childFace[iEdge];

		}// End For ( Each vertex )

		// Update faces neighboring information
		for ( iFace = 0; iFace < numberFace; iFace ++ ) {
			struct tesselate_face * currTessFace = &tessFace[iFace];
			int iEdge;
			for ( iEdge = 0; iEdge < 3; iEdge ++ ) {
				// Handle the central one
				currTessFace->childFace[3]->tessFace[iEdge] =
					currTessFace->childFace[NextVert ( iEdge )];

				// Dhandle the rest of three sub-face
				struct tesselate_face * childFace = currTessFace->childFace[iEdge];
				childFace->tessFace[NextVert ( iEdge )] =
					currTessFace->childFace[3];

				if ( currTessFace->tessFace[iEdge] ) {
					// Restore neighbor's vertex index
					int iNeighborIndex;
					struct tesselate_face * neighborFace =
								currTessFace->tessFace[iEdge];

					GetFaceVertexIndex ( neighborFace,
							     currTessFace->tessVertex[iEdge],
							     iNeighborIndex );
					childFace->tessFace[iEdge] =
						currTessFace->tessFace[iEdge]->childFace[iNeighborIndex];
				} else {
					childFace->tessFace[iEdge] = nullptr;

				}// End If ( Boundary ? )

				if ( currTessFace->tessFace[PrevVert ( iEdge )] ) {
					// Restore neighbor's vertex index
					int iNeighborIndex;
					struct tesselate_face * neighborFace =
						currTessFace->tessFace[PrevVert ( iEdge )];

					GetFaceVertexIndex ( neighborFace,
							     currTessFace->tessVertex[PrevVert ( iEdge )],
							     iNeighborIndex );
					childFace->tessFace[PrevVert ( iEdge )] =
						currTessFace->tessFace[PrevVert ( iEdge )]->childFace[iNeighborIndex];
				} else {
					childFace->tessFace[PrevVert ( iEdge )] = nullptr;

				}// End If ( Boundary ? )

			}// End For ( End edge )

		}// End For ( Each face )

		// Update faces' vertices
		for ( iFace = 0; iFace < numberFace; iFace ++ ) {
			struct tesselate_face * currTessFace = &tessFace[iFace];
			int iChild;
			for ( iChild = 0; iChild < 3; iChild ++ ) {
				// Don't care about odd vertices, they have all been stored
				// in the central triangle. For each of the child ( Except the central one ),
				// would certainly have two vertices that stick with the central one
				// We define first vertex as iChild, second as NextVert ( iChild ), and so on
				// Current child's first vertex
				int iChildVertex =
					currTessFace->tessVertex[iChild] - tessVertex;
				int iCurrVertex = iChild;
				currTessFace->childFace[iChild]->tessVertex[iCurrVertex] =
					&childTessVertex[iChildVertex];

				// Current Child's second vertex
				iCurrVertex = NextVert ( iCurrVertex );
				currTessFace->childFace[iChild]->tessVertex[iCurrVertex] =
					currTessFace->childFace[3]->tessVertex[iChild];

				// Current Child's third vertex
				iCurrVertex = NextVert ( iCurrVertex );
				currTessFace->childFace[iChild]->tessVertex[iCurrVertex] =
					currTessFace->childFace[3]->tessVertex[iCurrVertex];

			}// End For ( Each child face )

		}// End For ( Each face )

		// Release last level's
		free_var ( tessVertex );
		free_var ( tessFace );

		// Recursively, put current level's as the
		// source information for next level's tesselation
		tessVertex = childTessVertex;
		tessFace = childTessFace;
		numberVertex += numberNewVertex;
		numberFace *= 4;

	}// End For ( Each level of subdivision )

	// Triangles have all been tesselated for certain times,
	// Now we are going to fix their normal, tangents, uvs ...
	// Points on limit surface
	struct point3d *limSurfVert;
	limSurfVert = alloc_fix ( sizeof ( struct point3d ), numberVertex );

	int iVertex;
	for ( iVertex = 0; iVertex < numberVertex; iVertex ++ ) {
		struct tesselate_vertex * currTessVertex = &tessVertex[iVertex];
		struct point3d * limPoint = &limSurfVert[iVertex];

		if ( currTessVertex->boundary ) {
			// V' = 3/5*V0 + 1/5*( V1 + V2 )
			// V0 is the central vertex, and V1 and V2 is
			// the other vertices on the same edge as V0
			scale_point3d ( 3.0f/5.0f, &currTessVertex->vertex, limPoint );

			// Find out the boundary vertices
			struct tesselate_face * vFace = currTessVertex->startFace;
			struct tesselate_face * nextFace;
			int edgeIndex;

			while ( 1 ) {
				GetNextFaceCCW ( vFace, nextFace, edgeIndex );
				if ( !nextFace ) {
					break;
				}
				vFace = nextFace;
			}// End While

			struct point3d *v1Vertex =
				&vFace->tessVertex[NextVert ( edgeIndex )]->vertex;
//			struct point3d tempPoint;
			sscale_add_point3d_u ( limPoint, 1.0f/5.0f, v1Vertex );

			while ( 1 ) {
				GetNextFaceCW ( vFace, nextFace, edgeIndex );
				if ( !nextFace ) {
					break;
				}
				vFace = nextFace;
			}// End While

			struct point3d *v2Vertex =
						&vFace->tessVertex[edgeIndex]->vertex;
			sscale_add_point3d_u ( limPoint, 1.0f/5.0f, v2Vertex );
		} else {
			// V' = gamma*(V0 + ... + Vn) (n = valence - 1)
			float gamma;
			GetGamma ( currTessVertex->valence, gamma );

			copy_point3d ( &currTessVertex->vertex, limPoint );

			// Loop through the one ring
			struct tesselate_face * vFace = currTessVertex->startFace;
			struct tesselate_face * nextFace;
			int edgeIndex;

			while ( 1 ) {
				GetNextFaceCCW ( vFace, nextFace, edgeIndex );

				struct point3d * vnVertex = &vFace->tessVertex[edgeIndex]->vertex;
				add_point3d_u ( limPoint, vnVertex );

				if ( currTessVertex->startFace == nextFace ) {
					break;
				}
				vFace = nextFace;
			}// End While

			scale_point3d_u ( gamma, limPoint );

		}// End If ( Boundary ? )

	}// End For ( Each vertex )

	for ( iVertex = 0; iVertex < numberVertex; iVertex ++ ) {
		copy_point3d ( &limSurfVert[iVertex], &tessVertex[iVertex].vertex );
	}

	// Recycle the memory for normals ^_^
	struct vector3d *normals = limSurfVert;

	// Compute tangent vectors and normals
	for ( iVertex = 0; iVertex < numberVertex; iVertex ++ ) {
		struct tesselate_vertex * currTessVertex = &tessVertex[iVertex];

		// Across tangent and transverse tangent
		struct vector3d acrossTan, transTan;
		int valence = currTessVertex->valence;

		// Loop to the beginning
		struct tesselate_face * face = tessVertex->startFace;
		struct tesselate_face * nextFace;
		int edgeIndex;

		while ( 1 ) {
			GetNextFaceCCW ( face, nextFace, edgeIndex );
			if ( !nextFace ||
			     nextFace != tessVertex->startFace ) {
				break;
			}
			face = nextFace;
		}// End While ( 1 )

		if ( !currTessVertex->boundary ) {
			// acrossTan = (V0 - o)*A0 + (V1 - o)*A1 + ... + (Vi - o)*Ai (i = 0, 1, ... n - 1)
			// where o is the origin of the coordinate, Ai = cos((2*pi/n)*i), and n = valence
			// transTan = (V0 - o)*B0 + (V1 - o)*B1 + ... + (Vi - o)*Bi (i = 0, 1, ... n - 1)
			// where Bi = sin((2*pi/n)*i)
			// The current processing point weighted zero here
			float invValence = 1.0f/valence;
			float i = 0.0f;
			init_vector3d ( &acrossTan );
			init_vector3d ( &transTan );

			while ( 1 ) {
				float aWeight = cosf ( 2.0f*PI*invValence*i );
				float bWeight = sinf ( 2.0f*PI*invValence*i );

				// acrossTan = (V0 - o)*A0 + (V1 - o)*A1 + ... + (Vi - o)*Ai
//				struct vector3d tempVector;
				sscale_add_vector3d_u ( &acrossTan, aWeight,
							&face->tessVertex[edgeIndex]->vertex );

				// transTan = (V0 - o)*B0 + (V1 - o)*B1 + ... + (Vi - o)*Bi
				sscale_add_point3d_u ( &transTan, bWeight,
						       &face->tessVertex[edgeIndex]->vertex );

				if ( nextFace != tessVertex->startFace ) {
					break;
				}

				GetNextFaceCW ( face, nextFace, edgeIndex );
				face = nextFace;
				i ++;
			}// End While ( 1 )
		} else {
			// acrossTan = Vn - V1 ( where n = valence ), V0 is the current processing point
			// valence = 2, transTan = (-2)*V0 + 1*V1 + 1*V2
			// valence = 3, transTan = (-1)*V0 + 0*V1 + 1*V2 + 0*V3
			// valence = 4, transTan = (-2)*V0 + (-1)*V1 + 2*V2 + 2*V3 + (-1)*V4
			// valence > 4, transTan = 0*V0 + sin(θ)(V1 + Vn) + ∑Vi*(2(cos(θ) - 1))*sin(θi)
			// where θ = pi/(n - 1), n = valence, and (i = 1, 2, ... n - 1)

			switch ( valence ) {
			case 2: {
				struct vector3d * v1 = &face->tessVertex[PrevVert ( edgeIndex )]->vertex;
				struct vector3d * v2 = &face->tessVertex[edgeIndex]->vertex;

				// acrossTan = V2 - V1
				sub_vector3d ( v2, v1, &acrossTan );

				// transTan = (-2)*V0 + V1 + V2
				sscale_add_vector3d ( v1, -2.0f, &currTessVertex->vertex, &transTan );
				add_vector3d_u ( &transTan, v2 );
				break;
			}

			case 3: {
				struct vector3d * v1 = &face->tessVertex[PrevVert ( edgeIndex )]->vertex;
				GetNextFaceCW ( face, nextFace, edgeIndex );
				struct vector3d * v2 = &face->tessVertex[PrevVert ( edgeIndex )]->vertex;
				struct vector3d * v3 = &face->tessVertex[edgeIndex]->vertex;

				// acrossTan = v3 - v1
				sub_vector3d ( v3, v1, &acrossTan );

				// transTan = (-1)*V0 + V2
				sub_vector3d ( v2, &currTessVertex->vertex, &transTan );
				break;
			}

			case 4: {
				struct vector3d * v1 = &face->tessVertex[PrevVert ( edgeIndex )]->vertex;
				GetNextFaceCW ( face, nextFace, edgeIndex );
				struct vector3d * v2 = &face->tessVertex[PrevVert ( edgeIndex )]->vertex;
				GetNextFaceCW ( face, nextFace, edgeIndex );
				struct vector3d * v3 = &face->tessVertex[PrevVert ( edgeIndex )]->vertex;
				struct vector3d * v4 = &face->tessVertex[edgeIndex]->vertex;

				// acrossTan = v4 - v1
				sub_vector3d ( v4, v1, &acrossTan );

				// transTan = -2*V0 + (-1)*V1 + 2*V2 + 2*V3 + (-1)*V4
//				struct vector3d tempVector;
				scale_vector3d ( -2.0f, &currTessVertex->vertex, &transTan );
				sscale_add_point3d_u ( &transTan, -1.0f, v1 );
				sscale_add_point3d_u ( &transTan, 2.0f, v2 );
				sscale_add_point3d_u ( &transTan, 2.0f, v3 );
				sscale_add_point3d_u ( &transTan, -1.0f, v4 );
				break;
			}

			default: {
				struct vector3d * v1 = &face->tessVertex[PrevVert ( edgeIndex )]->vertex;
				init_vector3d ( &transTan );
				float i = 0.0f;
				float theta = PI/(float)(valence - 1);
				float vWeight = 2.0f*cosf ( theta ) - 2.0f;
				struct vector3d tempVector;

				while ( 1 ) {
					GetNextFaceCW ( face, nextFace, edgeIndex );

					// ∑Vi*(2(cos(θ) - 1))*sin(θi)
					float weight = vWeight*sinf ( theta*i );
					struct point3d *vi = &face->tessVertex[PrevVert ( edgeIndex )]->vertex;
					sscale_add_point3d_u ( &transTan, weight, vi );

					if ( !nextFace ) {
						break;
					}
					i ++;

				}// End While

				struct point3d * vn = &face->tessVertex[edgeIndex]->vertex;

				// acrossTan = vn - v1
				sub_vector3d ( vn, v1, &acrossTan );

				// sin(θ)*(v1 + vn) + ∑ vSum
				add_vector3d ( v1, vn, &tempVector );
				sscale_add_point3d_u ( &transTan, sinf ( theta ), &tempVector );
				break;
			}

			}// End Switch ( Valence )

			scale_vector3d_u ( -1.0f, &transTan );
		}// End If ( Boundary ? )

		struct vector3d tempNormal;
		cross_vector3d ( &acrossTan, &transTan, &tempNormal );
		normalize_vector3d ( &tempNormal, &normals[iVertex] );

	}// End For ( Each vertex )

	// Create triangle mesh
	struct triangle_mesh_shape *newTriangleMesh =
		ConvertTessToTriangleMesh ( tessVertex, numberVertex,
					    tessFace, numberVertex,
					    normals, triangleMesh );

	// Release last level's
	free_fix ( normals );
	free_var ( tessVertex );
	free_var ( tessFace );

	return newTriangleMesh;

}// End Function LoopTesselateTriangleMesh



// Construct all needed geometry information  ( majorly neighboring information )
void InitTesselateGeometry ( struct triangle_mesh_shape *triangleMesh,
			     struct tesselate_vertex **_tessVertex,
			     struct tesselate_face **_tessFace )
{
	// Initialize Tesselating structures
	struct tesselate_vertex *tessVertex =
		alloc_var ( sizeof ( *tessVertex ), 0 );

	struct tesselate_face *tessFace =
		alloc_var ( sizeof ( *tessFace ), 0 );

	// Copy Vertices
	int iVertex;
	for ( iVertex = 0; iVertex < triangleMesh->num_vertex; iVertex ++ ) {
		tessVertex = add_var ( tessVertex, 1 );
		copy_point3d ( &triangleMesh[iVertex].v[iVertex],
			       &tessVertex[iVertex].vertex );
	}

	// And faces too
	int iFace;
	for ( iFace = 0; iFace < triangleMesh->num_tri; iFace ++ ) {
		tessFace = add_var ( tessFace, 1 );

		struct tesselate_face * currTessFace = &tessFace[iFace];
		struct triangle_shape *currFace = &triangleMesh->tri[iFace];
		int baseIndex = iFace*3;

		for ( iVertex = 0; iVertex < 3; iVertex ++ ) {
			struct tesselate_vertex *currTessVertex =
						&tessVertex[currFace->i[baseIndex + iVertex]];
			currTessFace->tessVertex[iVertex] = currTessVertex;
			currTessVertex->startFace = currTessFace;

			int iEdge = iVertex;
			currTessFace->tessFace[iEdge] = nullptr;
		}// End For ( Each Vertex )

	}// End For ( Each face )

	// Find neighboring edges
	struct tesselate_edge *tessEdge =
		alloc_var ( sizeof ( *tessEdge ), 0 );
	int numberEdge = 0;

	for ( iFace = 0; iFace < triangleMesh->num_tri; iFace ++ ) {
		struct tesselate_face *currTessFace = &tessFace[iFace];

		for ( iVertex = 0; iVertex < 3; iVertex ++ ) {
			// Construct an edge
			struct tesselate_edge currTessEdge;
			currTessEdge.tessVertex[0] =
				min ( currTessFace->tessVertex[iVertex],
				      currTessFace->tessVertex[NextVert ( iVertex )] );
			currTessEdge.tessVertex[1] =
				max ( currTessFace->tessVertex[iVertex],
				      currTessFace->tessVertex[NextVert ( iVertex )] );

			// Find face that share the current edge
			struct tesselate_edge * foundTessEdge =
				FindEdge ( &currTessEdge, tessEdge, numberEdge );

			const int iEdge = iVertex;
			if ( !foundTessEdge ) {
				currTessEdge.f0EdgeIndex = iEdge;
				currTessEdge.tessFace[0] = currTessFace;
				AddEdge ( &currTessEdge, &tessEdge, &numberEdge );
			} else {
				// Have them point to each other
				struct tesselate_face * tessNeighbor =
							foundTessEdge->tessFace[0];
				currTessFace->tessFace[iEdge] =
					tessNeighbor;
				tessNeighbor->tessFace[foundTessEdge->f0EdgeIndex] =
					currTessFace;

				// Remove it from the edge set
				DeleteEdge ( foundTessEdge, tessEdge, &numberEdge );

			}// End If

		}// End For ( Each edge )

	}// End For ( Each face )

	free_var ( tessEdge );

	// Recognize regular and extaodinary vertices,
	// its valence and geometry margin
	for ( iVertex = 0; iVertex < triangleMesh->num_tri; iVertex ++ ) {
		struct tesselate_vertex * currTessVertex = &tessVertex[iVertex];

		// Loop through all neighboring faces
		int valence = 0;
		struct tesselate_face *face = currTessVertex->startFace;
		struct tesselate_face *nextFace;
		const struct tesselate_face *lastFace = nextFace;

		// Loop faces counterclockwise
		while ( 1 ) {
			int edgeIndex;
			GetNextFaceCCW ( face, nextFace, edgeIndex );
			valence ++;

			if ( !nextFace ||
			     nextFace == currTessVertex->startFace ) {
				break;
			}

		}// End While

		if ( !lastFace ) {
			// Handle boundry vertex
			currTessVertex->boundary = 1;

			// Loop through other side to get all adjacent vertex
			face = currTessVertex->startFace;

			// Loop faces clockwise
			while ( 1 ) {
				int edgeIndex;
				GetNextFaceCW ( face, nextFace, edgeIndex );
				valence ++;

				if ( !nextFace ||
				     nextFace == currTessVertex->startFace ) {
					break;
				}

			}// End While

			currTessVertex->valence = valence - 1;

			if ( valence - 1 == REGULAR_VALENCE_BOUNDRY ) {
				currTessVertex->regular = 1;
			} else {
				currTessVertex->regular = 0;
			}
		} else if ( lastFace == currTessVertex->startFace ) {
			currTessVertex->boundary = 0;
			currTessVertex->valence = valence;

			if ( valence == REGULAR_VALENCE ) {
				currTessVertex->regular = 1;
			} else {
				currTessVertex->regular = 0;
			}

		}// End If ( Boundary ? )

	}// End For ( Each vertex )

	*_tessVertex = tessVertex;
	*_tessFace = tessFace;

}// End Function InitTesselateGeometry

#undef NextVert
#undef PrevVert
#undef LoopFaceCounterClockWise
#undef LoopFaceClockWise



// Convert tesselated mesh into triangle mesh
struct triangle_mesh_shape *ConvertTessToTriangleMesh ( struct tesselate_vertex *tessVertex, int numberVertex,
		struct tesselate_face *tessFace, int numberTriangle,
		struct vector3d *normals, struct triangle_mesh_shape *oldTriangleMesh )
{
	struct point3d *vertices = alloc_fix ( sizeof ( *vertices ), numberVertex );
	int *indices = alloc_fix ( sizeof ( int ), numberTriangle );
	int iVertex;
	for ( iVertex = 0; iVertex < numberVertex; iVertex ++ ) {
		copy_point3d ( &tessVertex[iVertex].vertex, &vertices[iVertex] );
	}
	int iTriangle;
	for ( iTriangle = 0; iTriangle < numberTriangle; iTriangle ++ ) {
		struct tesselate_face * currTessFace = &tessFace[iTriangle];
		int base = iTriangle*3;

		for ( iVertex = 0; iVertex < 3; iVertex ++ ) {
			int indexOffset =
				currTessFace->tessVertex[iVertex] - tessVertex;
			indices[base + iVertex] = indexOffset;
		}// End For ( Each vertex )
	}// End For ( Each triangle )

	struct triangle_mesh_shape *new_mesh = alloc_fix ( sizeof ( *new_mesh ), 1 );
	memcpy ( new_mesh, oldTriangleMesh, sizeof ( *new_mesh ) );
	build_triangle_mesh ( vertices, numberVertex, indices, numberTriangle,
			      normals, nullptr, nullptr,
			      nullptr, nullptr, nullptr, 0, new_mesh );
	set_transform_triangle_mesh ( &oldTriangleMesh->prop.t_world, &oldTriangleMesh->prop.t_object, new_mesh );
	transform_to_world_triangle_mesh ( new_mesh );
	return new_mesh;
}



#define IsLesserEdge(edge0, edge1)	\
{\
	((edge0).tessVertex[0] == (edge1).tessVertex[0]) ?\
	((edge0).tessVertex[1] < (edge1).tessVertex[1]) :\
		((edge0).tessVertex[0] < (edge1).tessVertex[0])\
}

// Find an identical edge from the edge set if possible
struct tesselate_edge * FindEdge ( struct tesselate_edge * cTessEdge, struct tesselate_edge * tessEdges, int numberEdge )
{
	/// TODO: Implements a O(log n) searching algorithm
	int iEdge;
	for ( iEdge = 0; iEdge < numberEdge; iEdge ++ ) {
		if ( cTessEdge->tessVertex[0] == tessEdges[iEdge].tessVertex[0] &&
		     cTessEdge->tessVertex[1] == tessEdges[iEdge].tessVertex[1] ) {
			return &tessEdges[iEdge];
		}

	}// End While

	return nullptr;

}// End Function FindEdge



// Add a new edge into the set
void AddEdge ( struct tesselate_edge *newEdge, struct tesselate_edge **tessEdges, int *numberEdge )
{
	/// TODO: Implements a O(log n) searching algorithm
	*tessEdges = push_var ( newEdge, *tessEdges );
	(*numberEdge) ++;
}



// Delete the specified edge from the set
void DeleteEdge ( struct tesselate_edge * sEdge, struct tesselate_edge * tessEdges, int *numberEdge )
{
	/// TODO: Implements a O(log n) searching algorithm
	int totalEdges = *numberEdge;
	int iEdge;
	for ( iEdge = 0; iEdge < totalEdges; iEdge ++ ) {
		if ( sEdge->tessVertex[0] == tessEdges[iEdge].tessVertex[0] &&
		     sEdge->tessVertex[1] == tessEdges[iEdge].tessVertex[1] ) {
			int movePos;
			for ( movePos = iEdge + 1; movePos < totalEdges; movePos ++ ) {
				memcpy ( &tessEdges[iEdge], &tessEdges[movePos],
					 sizeof ( struct tesselate_edge ) );

			}// End For

			dec_var ( sEdge, 1 );
			(*numberEdge) --;
			break;

		}// End If

	}// End For ( Each Edge )

}// End Function DeleteEdge


#undef IsLesserEdge
