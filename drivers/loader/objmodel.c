/* obj_model.c: Obj model file loading functions all go here */
#include <misc.h>
#include <math/math.h>
#include <container/staging.h>
#include <system/log.h>
#include <x3d/resloader.h>
#include "modelloader.h"
#include "objmodel.h"

/* @fixme (davis#1#): change to current <resloader> api */


#define VERT_ATTR_POSITION		0X1
#define VERT_ATTR_UV			0X2
#define VERT_ATTR_NORMAL		0X4
#define MAX_VERT_ATTRS			8

#define MAX_LINE_LEN			256

const int objVertAttr[MAX_VERT_ATTRS] = {
        VERT_ATTR_POSITION, VERT_ATTR_UV, VERT_ATTR_NORMAL, 0, 0, 0, 0, 0
};

struct GLOBAL_VERT_ATTR {
        struct point4d *vPositions;
        struct vector4d *vNormals;
        struct vector3d *vUVs;
        int nVerts;
};

struct PRIMITIVE {
        int *vIndices;
        int nVerts;
        int matIndex;
};

struct MESH {
        int vertAttr;
        int nVertAttrs;
        struct PRIMITIVE *primitives;
        int nPrimitives;
        int indexOffset;
};

/* Intermediate structure that helps convert to standard data sequence */
struct OBJ_INTER_FILE {
        char *fileContent;
        int nLines;
        struct GLOBAL_VERT_ATTR gbVertAttr;
        struct MESH *meshes;
        int nMeshes;
};

int StripRedundant ( char *line );
int FetchToMem ( FILE *file, struct OBJ_INTER_FILE *objInterFile );
void CleanFetchToMemPass ( struct OBJ_INTER_FILE *objInterFile );
int GetVertAttrInfo ( char *faceLine, int *attrState );
int GetVertNum ( char *faceLine );
void PutVertAttrIndex ( char *faceLine, int nAttrs, int *vIndices );
void InitMesh ( struct MESH *mesh );
void ReleaseMesh ( struct MESH *mesh );
int PrimitiveScanPass ( struct OBJ_INTER_FILE *objInterFile );
void CleanPrimitiveScanPass ( struct OBJ_INTER_FILE *objInterFile );
void CreateGlobalVertAttr ( struct GLOBAL_VERT_ATTR *gbVertAttr );
void ReleaseGlobalVertAttr ( struct GLOBAL_VERT_ATTR *gbVertAttr );
int MakeAttribute ( char *vLine, void *attribute );
void VertexAttrScanPass ( struct OBJ_INTER_FILE *objInterFile );
void CleanVertAttrScanPass ( struct OBJ_INTER_FILE *objInterFile );
int SelectVertAttr ( struct MESH *mesh, uint8_t *filter );
void PushVertIndex ( struct MESH *mesh, struct res_comp *mesh_comp );
void GatherOutput ( struct OBJ_INTER_FILE *objInterFile, struct res_task *task );

// Strip away '\n', '\r', '\a'
int StripRedundant ( char *line )
{
        char *temp = line;
        while ( *line != 0 ) {
                if ( *line == '\n' ||
                     *line == '\r' ||
                     *line == '\a' ) {
                        *line = 0;
                        break;
                }
                line ++;
        }// End While

        return line - temp;

}// End Function StripRedundant

// Fetch whole file content to memory
int FetchToMem ( FILE *file, struct OBJ_INTER_FILE *objInterFile )
{
        objInterFile->fileContent = alloc_var ( sizeof ( char ), 0 );
        char tmpBuffer[MAX_LINE_LEN];
        int nLines = 0;
        int usedByte = 0;

        while ( fgets ( tmpBuffer, MAX_LINE_LEN, file ) ) {
                int actualLen = StripRedundant ( tmpBuffer ) + 1;
                objInterFile->fileContent = alloc_var_add ( objInterFile->fileContent, actualLen );
                memcpy ( &objInterFile->fileContent[usedByte], tmpBuffer, actualLen );
                usedByte += actualLen;
                nLines ++;
        }// End While

        objInterFile->nLines = nLines;
        return nLines;

}// End Function FetchToMem

void CleanFetchToMemPass ( struct OBJ_INTER_FILE *objInterFile )
{
        free_var ( objInterFile->fileContent );

}// End Function CleanFetchToMemPass

#define GetNextStr( _strLine, _separator, _terminator, _len )	\
{\
	while ( *(_strLine) == (_separator) ) { (_strLine) ++; }\
	char *_tmp = (_strLine);\
	while ( *(_tmp) != (_separator) && *(_tmp) != (_terminator)) { (_tmp) ++; }\
	(_len) = _tmp - (_strLine);\
\
}// End Function GetNextStr

#define GetNextStr2( _strLine, _separator, _terminator0, _terminator1, _len )	\
{\
	while ( *(_strLine) == (_separator) ) { (_strLine) ++; }\
	char *_tmp = (_strLine);\
	while ( *(_tmp) != (_separator) &&\
			*(_tmp) != (_terminator0) &&\
			*(_tmp) != _terminator1) { (_tmp) ++; }\
	(_len) = _tmp - (_strLine);\
\
}// End Function GetNextStr2

#define GetNextLine( _strLine, _cLineEnd )	\
{\
	while ( *(_strLine) != (_cLineEnd) ) { (_strLine) ++; }\
	(_strLine) ++;\
\
}// End Function GetNextLine

int GetVertAttrInfo ( char *faceLine, int *attrState )
{
        int nAttrs = 0;
        int iAttr = 0;
        int tAttrState = 0;
        uint8_t hasAttr = 0;
        while ( 1 ) {
                if ( *faceLine != '/' &&
                     *faceLine != ' ' &&
                     *faceLine != 0 ) {
                        hasAttr = 1;
                } else {
                        if ( hasAttr ) {
                                tAttrState |= objVertAttr[iAttr];
                                nAttrs ++;
                                hasAttr = 0;
                        }
                        iAttr ++;
                }
                if ( *faceLine == 0 ||
                     *faceLine == ' ' ) {
                        break;
                }

                faceLine ++;
        }
        *attrState = tAttrState;
        return nAttrs;

}// End Function GetVertAttrInfo

int GetVertNum ( char *faceLine )
{
        int nVerts = 0;
        int len;
        GetNextStr ( faceLine, ' ', 0, len );
        while ( len != 0 ) {
                nVerts ++;
                faceLine += len;
                GetNextStr ( faceLine, ' ', 0, len );
        }// End While

        return nVerts;

}// End Function GetVertNum

void PutVertAttrIndex ( char *faceLine, int nAttrs, int *vIndices )
{
        int len;
        char *cLineSave = faceLine;
        GetNextStr2 ( cLineSave, '/', ' ', 0, len );
        int iAttr;
        for ( iAttr = 0; iAttr < nAttrs; iAttr ++ ) {
                strn_to_int ( faceLine, &vIndices[iAttr], len );
                vIndices[iAttr] -= 1;	// Stupid obj index starts from 1
                faceLine += len;
                GetNextStr2 ( faceLine, '/', ' ', 0, len );
        }// End For ( Each Attribute )

}// End Function PutVertAttrIndex

void InitMesh ( struct MESH *mesh )
{
        mesh->primitives = alloc_var ( sizeof ( struct PRIMITIVE ), 0 );
}

void ReleaseMesh ( struct MESH *mesh )
{
        free_var ( mesh->primitives );
        memset ( mesh, 0, sizeof ( struct MESH ) );
}

// Scan in primitive information
int PrimitiveScanPass ( struct OBJ_INTER_FILE *objInterFile )
{
        objInterFile->meshes = alloc_var ( sizeof ( struct MESH ), 0 );
        char *currentLine = objInterFile->fileContent;

        struct MESH *currMesh;
        struct PRIMITIVE *currPrimitives;
        int iMesh = 0;
        int nVertAttrs;
        int vertAttrState;
        int iPrimitive;
        int minIndex;
        uint8_t newMeshSwitch = 1;
        uint8_t isNewMeshFinish = 1;

        int contentLen;
        int iLines;
        for ( iLines = 0; iLines < objInterFile->nLines; iLines ++ ) {
                GetNextStr ( currentLine, ' ', 0, contentLen );
                if ( contentLen == 0 ) {
                        GetNextLine ( currentLine, 0 );
                        continue;
                }

                switch ( *currentLine ) {
                case 'g': {			// This means submesh group begins
                        if ( !isNewMeshFinish ) {
                                currMesh->nPrimitives = iPrimitive;
                                currMesh->indexOffset = minIndex;
                                isNewMeshFinish = 1;
                                iMesh ++;
                        }
                        if ( newMeshSwitch ) {
                                // Add and initialize mesh content
                                objInterFile->meshes = alloc_var_add ( objInterFile->meshes, 1 );
                                currMesh = &objInterFile->meshes[iMesh];
                                memset ( currMesh, 0, sizeof ( struct MESH ) );
                                InitMesh ( currMesh );

                                // Get read for current primitive
                                iPrimitive = 0;
                                minIndex = INT_MAX;
                                newMeshSwitch = 0;
                        }
                        break;
                }
                case 'f': {
                        // Skip the "f    "
                        currentLine += contentLen;
                        GetNextStr ( currentLine, ' ', 0, contentLen );

                        if ( !newMeshSwitch ) {
                                nVertAttrs = GetVertAttrInfo ( currentLine, &vertAttrState );
                                if ( !(vertAttrState & VERT_ATTR_POSITION) ) {	// At least, it got to have a vertex
                                        log_severe_err_dbg ( "PrimitiveScanPass",
                                                             "The model file doesn't have a vertex" );
                                        return 0;
                                }
                                currMesh->vertAttr = vertAttrState;
                                currMesh->nVertAttrs = nVertAttrs;
                                newMeshSwitch = 1;
                        }
                        isNewMeshFinish = 0;
                        currMesh->primitives = alloc_var_add ( currMesh->primitives, 1 );
                        currPrimitives = currMesh->primitives;

                        int nVerts = GetVertNum ( currentLine );
                        currPrimitives[iPrimitive].nVerts = nVerts;
                        currPrimitives[iPrimitive].vIndices = alloc_fix ( sizeof ( int ), nVerts*nVertAttrs );
                        int iVert;
                        for ( iVert = 0; iVert < nVerts; iVert ++ ) {
                                int *currVertIndex = &currPrimitives[iPrimitive].vIndices[iVert*nVertAttrs];
                                PutVertAttrIndex ( currentLine, nVertAttrs, currVertIndex );
                                if ( currVertIndex[0] < minIndex ) {	// Find out the min position index of all
                                        minIndex = currVertIndex[0];
                                }
                                currentLine += contentLen;
                                GetNextStr ( currentLine, ' ', 0, contentLen );
                        }// End For ( Each vertex )

                        iPrimitive ++;
                        break;
                }
                }// End Switch

                GetNextLine ( currentLine, 0 );
        }// End For ( Each line )

        if ( !isNewMeshFinish ) {
                currMesh->nPrimitives = iPrimitive;
                currMesh->indexOffset = minIndex;
                isNewMeshFinish = 1;
                iMesh ++;
        }
        objInterFile->nMeshes = iMesh;
        return 1;

}// End Function PrimitiveScanPass

void CleanPrimitiveScanPass ( struct OBJ_INTER_FILE *objInterFile )
{
        int iMesh;
        for ( iMesh = 0; iMesh < objInterFile->nMeshes; iMesh ++ ) {
                struct MESH *currMesh = &objInterFile->meshes[iMesh];
                int iPrimitive;
                for ( iPrimitive = 0; iPrimitive < currMesh->nPrimitives; iPrimitive ++ ) {
                        free_fix ( currMesh->primitives[iPrimitive].vIndices );
                }
                ReleaseMesh ( currMesh );
        }
        free_var ( objInterFile->meshes );

}// End Function CleanPrimitiveScanPass

void CreateGlobalVertAttr ( struct GLOBAL_VERT_ATTR *gbVertAttr )
{
        memset ( gbVertAttr, 0, sizeof ( struct GLOBAL_VERT_ATTR ) );
        gbVertAttr->vPositions = alloc_var ( sizeof ( struct point4d ), 0 );
        gbVertAttr->vNormals = alloc_var ( sizeof ( struct vector4d ), 0 );
        gbVertAttr->vUVs = alloc_var ( sizeof ( struct vector3d ), 0 );
}

void ReleaseGlobalVertAttr ( struct GLOBAL_VERT_ATTR *gbVertAttr )
{
        free_var ( gbVertAttr->vPositions );
        free_var ( gbVertAttr->vNormals );
        free_var ( gbVertAttr->vUVs );

}// End Function ReleaseGlobalVertAttr

int MakeAttribute ( char *vLine, void *attribute )
{
        float *attrComp = (float *) attribute;
        int len;
        GetNextStr ( vLine, ' ', 0, len );
        while ( len != 0 ) {
                strn_to_float ( vLine, attrComp, len );
                attrComp ++;
                vLine += len;
                GetNextStr ( vLine, ' ', 0, len );
        }// End While

        return attrComp - (float *) attribute;

}// End Function MakeAttribute

// Scans in vertex attributes
void VertexAttrScanPass ( struct OBJ_INTER_FILE *objInterFile )
{
        CreateGlobalVertAttr ( &objInterFile->gbVertAttr );
        char *currentLine = objInterFile->fileContent;
        int iVertex = 0;
        int contentLen;
        int iLine;
        for ( iLine = 0; iLine < objInterFile->nLines; iLine ++ ) {
                GetNextStr ( currentLine, ' ', 0, contentLen );
                if ( contentLen == 0 || *currentLine != 'v' ) {
                        GetNextLine ( currentLine, 0 );
                        continue;
                }
                switch ( *(currentLine + 1) ) {
                case ' ': {			// "v"
                        currentLine += contentLen;
                        GetNextStr ( currentLine, ' ', 0, contentLen );
                        objInterFile->gbVertAttr.vPositions =
                                alloc_var_add ( objInterFile->gbVertAttr.vPositions, 1 );
                        struct point4d *currVertPos = alloc_var_get_last ( objInterFile->gbVertAttr.vPositions );
                        int nComps = MakeAttribute ( currentLine, currVertPos );
                        if ( nComps == 3 ) {	// If the file does't specify a w component
                                currVertPos->w = 1.0f;
                        }
                        iVertex ++;
                        break;
                }
                case 'n': {			// "vn"
                        currentLine += contentLen;
                        GetNextStr ( currentLine, ' ', 0, contentLen );
                        objInterFile->gbVertAttr.vNormals =
                                alloc_var_add ( objInterFile->gbVertAttr.vNormals, 1 );
                        struct vector4d *currVertNor = alloc_var_get_last ( objInterFile->gbVertAttr.vNormals );
                        int nComps = MakeAttribute ( currentLine, currVertNor );
                        if ( nComps == 3 ) {	// If the file does't specify a w component
                                currVertNor->w = 1.0f;
                        }
                        break;
                }
                case 't': {			// "vt"
                        currentLine += contentLen;
                        GetNextStr ( currentLine, ' ', 0, contentLen );
                        objInterFile->gbVertAttr.vUVs = alloc_var_add ( objInterFile->gbVertAttr.vUVs, 1 );
                        struct vector3d *currVertUv = alloc_var_get_last ( objInterFile->gbVertAttr.vUVs );
                        int nComps = MakeAttribute ( currentLine, currVertUv );
                        if ( nComps == 2 ) {	// If the file does't specify a w component
                                currVertUv->z = 1.0f;
                        }
                        break;
                }
                }// End Switch

                GetNextLine ( currentLine, 0 );
        }// End For ( Each line )

        objInterFile->gbVertAttr.nVerts = iVertex;

}// End Function VertexAttrScanPass

void CleanVertAttrScanPass ( struct OBJ_INTER_FILE *objInterFile )
{
        ReleaseGlobalVertAttr ( &objInterFile->gbVertAttr );
        memset ( &objInterFile->gbVertAttr, 0, sizeof ( struct GLOBAL_VERT_ATTR ) );
}

int SelectVertAttr ( struct MESH *mesh, uint8_t *filter )
{
        int nVerts = 0;
        const int nVertAttrs = mesh->nVertAttrs;
        int iPrimitive;
        for ( iPrimitive = 0; iPrimitive < mesh->nPrimitives; iPrimitive ++ ) {
                struct PRIMITIVE *currPrimitive = &mesh->primitives[iPrimitive];
                int iVert;
                for ( iVert = 0; iVert < currPrimitive->nVerts; iVert ++ ) {
                        int iAttr;
                        for ( iAttr = 0; iAttr < nVertAttrs; iAttr ++ ) {
                                int currVertIndex = currPrimitive->vIndices[iVert*nVertAttrs + iAttr];

                                if ( !filter[currVertIndex*nVertAttrs + iAttr] ) {	// That means it has not been processed yet
                                        filter[currVertIndex] = 1;
                                        nVerts ++;
                                }
                        }// End For ( Each Attrs )
                }// End For ( Each vertex )
        }// End For ( Each primitive )

        return nVerts/nVertAttrs;

}// End Function SelectVertAttr

#define PushVertAttr( _attrList, _count, _nAttrs, _iAttrs, _filter, _arr ) \
{ \
	int _i; \
	for ( _i = 0; _i < (_count); _i ++ ) { \
		if ( ( _filter)[_i*(_nAttrs) + (_iAttrs)] ) { \
			push_data_stager ( &(_attrList)[_i], sizeof (_attrList)[_i], (_rec) ); \
		} \
	} \
}

void PushVertIndex ( struct MESH *mesh, struct res_comp *mesh_comp )
{
        /* allocate enough space for the first trunk of faces */
        int n_verts = mesh->primitives[0].nVerts;
        int n_face = 0;
        int *faces = res_comp_alloc_face ( n_verts, mesh->nPrimitives, mesh_comp );

        int indexOffset = mesh->indexOffset;
        const int nVertAttrs = mesh->nVertAttrs;
        int i;
        for ( i = 0; i < mesh->nPrimitives; i ++ ) {
                struct PRIMITIVE *currPrimitive = &mesh->primitives[i];
                if ( n_verts != currPrimitive->nVerts ) {
                        /* experiences a change in vertex count,
                         * break the current array and tell the exact primitive count
                         * and allocate a new piece for it */
                        res_comp_shrink ( mesh_comp, n_face );
                        n_verts = currPrimitive->nVerts;
                        n_face = 0;
                        faces = res_comp_alloc_face ( n_verts, mesh->nPrimitives, mesh_comp );
                }

                int j;
                for ( j = 0; j < currPrimitive->nVerts; j ++ ) {
                        int currVertIndex = currPrimitive->vIndices[j*nVertAttrs] - indexOffset;
                        faces[i + j] = currVertIndex;
                }
                n_face ++;
        }
        /* shrink for the last chunk */
        res_comp_shrink ( mesh_comp, n_face );
}

// Gather the information and standardize output to the resource piler
void GatherOutput ( struct OBJ_INTER_FILE *objInterFile, struct res_task *task )
{
        /* v... vn... vt... index... */
        uint8_t *filter = alloc_var ( sizeof ( uint8_t ), 0 );
        int iMesh;
        for ( iMesh = 0; iMesh < objInterFile->nMeshes; iMesh ++ ) {
                /* Begin mesh */
                struct MESH *currMesh = &objInterFile->meshes[iMesh];
                struct res_comp *mesh_comp = res_comp_alloc_unit ( task );

                /* Begin vertex attributes */
                int vertLen = objInterFile->gbVertAttr.nVerts;
                int filterLen = objInterFile->gbVertAttr.nVerts*currMesh->nVertAttrs;
                filter = alloc_var_expand ( filter, filterLen );
                memset ( filter, 0, filterLen );
                int nVerts = SelectVertAttr ( currMesh, filter );
                struct GLOBAL_VERT_ATTR *gbVertAttr = &objInterFile->gbVertAttr;

                int iAttr = 0;
                if ( currMesh->vertAttr & VERT_ATTR_POSITION ) {
                        /* push non-repeat vertices into the allocated array */
                        struct point4d *vert_arr = res_comp_alloc_vertex (
                                                           sizeof (struct point4d), nVerts, mesh_comp );
                        int i, j;
                        for ( i = 0, j = 0; i < vertLen; i ++ ) {
                                if ( filter[i*currMesh->nVertAttrs + iAttr] ) {
                                        vert_arr[j ++] = gbVertAttr->vPositions[i];
                                }
                        }
                        iAttr ++;
                }
                if ( currMesh->vertAttr & VERT_ATTR_NORMAL ) {
                        /* push non-repeat normals into the allocated array */
                        struct vector4d *norm_arr = res_comp_alloc_normal (
                                                            sizeof (struct vector4d), nVerts, mesh_comp );
                        int i, j;
                        for ( i = 0, j = 0; i < vertLen; i ++ ) {
                                if ( filter[i*currMesh->nVertAttrs + iAttr] ) {
                                        norm_arr[j ++] = gbVertAttr->vNormals[i];
                                }
                        }
                        iAttr ++;
                }
                if ( currMesh->vertAttr & VERT_ATTR_UV ) {
                        /* push non-repeat uvs into the allocated array */
                        struct vector3d *uv_arr = res_comp_alloc_uv (
                                                          sizeof (struct vector3d), nVerts, mesh_comp );
                        int i, j;
                        for ( i = 0, j = 0; i < vertLen; i ++ ) {
                                if ( filter[i*currMesh->nVertAttrs + iAttr] ) {
                                        uv_arr[j ++] = gbVertAttr->vUVs[i];
                                }
                        }
                        iAttr ++;
                }

                /* Begin vertex indices */
                PushVertIndex ( currMesh, mesh_comp );
        }
        free_var ( filter );
}

// Load a obj format model file
bool load_obj_model ( char *filename, struct res_task *task )
{
        int returnValue = 1;
        FILE *modelFile = fopen ( filename, "r" );
        if ( !modelFile ) {
                log_severe_err ( "Couldn't open the file, file name: %s", filename );
                returnValue = 0;
                goto FUNCTION_END;
        }// End If

        struct OBJ_INTER_FILE objInterFile;
        memset ( &objInterFile, 0, sizeof ( struct OBJ_INTER_FILE ) );

        int nLines;
        if ( !(nLines = FetchToMem ( modelFile, &objInterFile )) ) {
                log_severe_err ( "The model file is empty, file name: %s", filename );
                returnValue = 0;
                goto FUNCTION_END;
        }
        if ( !PrimitiveScanPass ( &objInterFile ) ) {
                log_severe_err ( "Failed at PrimitiveScanPass, file name: %s", filename );
                returnValue = 0;
                goto FUNCTION_END;
        }
        VertexAttrScanPass ( &objInterFile );
        GatherOutput ( &objInterFile, task );

FUNCTION_END:
        CleanVertAttrScanPass ( &objInterFile );
        CleanPrimitiveScanPass ( &objInterFile );
        CleanFetchToMemPass ( &objInterFile );
        fclose ( modelFile );
        return returnValue;

}// End Function LoadModelFileObj

bool free_obj_model ( struct res_task *task )
{
        return true;
}
