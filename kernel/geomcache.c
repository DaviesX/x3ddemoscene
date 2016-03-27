#include <system/allocator.h>
#include <system/log.h>
#include <math/math.h>
#include <x3d/debug.h>
#include <x3d/geomcache.h>


/*
 * <util_aos> public
 */
void geomcache_init(struct geomcache* self, enum UtilAttribute format)
{
        zero_obj(self);

        self->index     = alloc_var(sizeof(int), 1);
        self->n_index   = 0;
        self->n_vertex = 0;

        if(format & UtilAttriVertex) {
                self->aos[_AttriVertex].s_data = alloc_var(sizeof(struct point3d), 1);
                self->avail[_AttriVertex] = true;
                self->n_streams ++;
        }
        if(format & UtilAttriNormal) {
                self->aos[_AttriNormal].s_data = alloc_var(sizeof(struct vector3d), 1);
                self->avail[_AttriNormal] = true;
                self->n_streams ++;
        }
        if(format & UtilAttriTangent) {
                self->aos[_AttriTangent].s_data = alloc_var(sizeof(struct vector3d), 1);
                self->avail[_AttriTangent] = true;
                self->n_streams ++;
        }
        if(format & UtilAttriUV) {
                self->aos[_AttriUV].s_data = alloc_var(sizeof(struct vector2d), 1);
                self->avail[_AttriUV] = true;
                self->n_streams ++;
        }
        if (format & UtilAttriMatId || format & UtilAttriMatIdList) {
                self->aos[_AttriMatId].s_data = alloc_var(sizeof(int), 1);
                self->avail[_AttriMatId] = true;
                self->n_streams ++;
        }

        self->format = format;
}

void geomcache_free(struct geomcache* self)
{
        if (self == nullptr) return;

        free_var(self->index);
        self->n_index = 0;
        self->n_vertex = 0;
        int i;
        for (i = 0; i < 10; i ++) {
                free_var(self->aos[i].s_data);
        }
        zero_obj(self);
}

void geomcache_flush(struct geomcache* self)
{
        alloc_var_flush(self->index);
        self->n_index = 0;
        self->n_vertex = 0;
        int i;
        for (i = 0; i < 10; i ++) {
                alloc_var_flush(self->aos[i].s_data);
        }
}

void geomcache_accumulate(struct geomcache* self, int* index, int num_index, int num_vertex, ...)
{
        /* copy vertex attributes */
        va_list v_arg;
        va_start(v_arg, num_vertex);

        enum UtilAttribute format = self->format;
        if(format & UtilAttriVertex) {
                struct point3d* src = va_arg(v_arg, struct point3d*);
                self->aos[_AttriVertex].s_data =
                        alloc_var_add(self->aos[_AttriVertex].s_data, num_vertex);
                struct point3d* v = self->aos[_AttriVertex].s_data;
                memcpy(&v[self->n_vertex], src, num_vertex*sizeof(*v));
        }
        if(format & UtilAttriNormal) {
                struct vector3d* src = va_arg(v_arg, struct vector3d*);
                self->aos[_AttriNormal].s_data =
                        alloc_var_add(self->aos[_AttriNormal].s_data, num_vertex);
                struct vector3d* n = self->aos[_AttriNormal].s_data;
                memcpy(&n[self->n_vertex], src, num_vertex*sizeof(*n));
        }
        if(format & UtilAttriTangent) {
                struct vector3d* src = va_arg(v_arg, struct vector3d*);
                self->aos[_AttriTangent].s_data =
                        alloc_var_add(self->aos[_AttriTangent].s_data, num_vertex);
                struct vector3d* t = self->aos[_AttriTangent].s_data;
                memcpy(&t[self->n_vertex], src, num_vertex*sizeof(*t));
        }
        if(format & UtilAttriUV) {
                struct vector2d* src = va_arg(v_arg, struct vector2d*);
                self->aos[_AttriUV].s_data =
                        alloc_var_add(self->aos[_AttriUV].s_data, num_vertex);
                struct vector2d* uv = self->aos[_AttriUV].s_data;
                memcpy(&uv[self->n_vertex], src, num_vertex*sizeof(*uv));
        }
        if (format & UtilAttriMatId) {
                // material id is set uniformly for each accumulation
                int src = va_arg(v_arg, int);
                self->aos[_AttriMatId].s_data = alloc_var_add(self->aos[_AttriMatId].s_data, num_vertex);
                int* matid = self->aos[_AttriMatId].s_data;
                int i;
                for (i = self->n_index; i < self->n_index + num_vertex; i ++) {
                        matid[i] = src;
                }
        }
        if (format & UtilAttriMatIdList) {
                int* src = va_arg(v_arg, int*);
                self->aos[_AttriMatId].s_data = alloc_var_add(self->aos[_AttriMatId].s_data, num_vertex);
                int* matid = self->aos[_AttriMatId].s_data;
                memcpy(&matid[self->n_vertex], src, num_vertex*sizeof(*matid));
        }

        va_end(v_arg);

        /* copy indices */
        self->index = alloc_var_add(self->index, num_index);
        int* d_index = &self->index[self->n_index];
        int i;
        for(i = 0; i < num_index; i ++) {
                d_index[i] = index[i] + self->n_vertex;
        }

        self->n_index += num_index;
        self->n_vertex += num_vertex;
}

struct box3d* geomcache_export_simplexes(struct geomcache* self, int* n_objects)
{
        *n_objects = geomcache_num_index(self)/3;
        struct box3d* simplexes = alloc_fix(sizeof(struct box3d), *n_objects);
        int i;
        for (i = 0; i < *n_objects; i ++) {
                init_box3d(&simplexes[i]);
                int* face = &self->index[i*3];
                int j;
                for (j = 0; j < 3; j ++) {
                        struct point3d* p = geomcache_vertex_at(self, face[j]);
                        union_box3d_pu(&simplexes[i], p);
                }
        }
        return simplexes;
}

int geomcache_get_vertices(struct geomcache* self, void* vertex[], int* n_streams)
{
        int i;
        for(i = 0; i < 10; i ++) {
                vertex[i] = self->aos[i].s_data;
        }
        *n_streams = self->n_streams;
        return self->n_vertex;
}

void* geomcache_get_indices(struct geomcache* self, int* n_index)
{
        *n_index = self->n_index;
        return self->index;
}

bool* geomcache_get_availibility(struct geomcache* self)
{
        return self->avail;
}

/*
 * <geomcache> test cases
 */
struct vertex {
        float   position[3];
        float   normal[3];
        int     mater_ref;
};
static const struct vertex cVertices[] = {
        /* Floor */
        [0].position    = {552.8f, 0.0f, 0.0f},
        [0].normal      = {0.0f, 1.0f, 0.0f},
        [0].mater_ref   = WHITE,
        [1].position    = {0.0f, 0.0f, 0.0f},
        [1].normal      = {0.0f, 1.0f, 0.0f},
        [1].mater_ref   = WHITE,
        [2].position    = {0.0f, 0.0f, 559.2f},
        [2].normal      = {0.0f, 1.0f, 0.0f},
        [2].mater_ref   = WHITE,
        [3].position    = {549.6f, 0.0f, 559.2f},
        [3].normal      = {0.0f, 1.0f, 0.0f},
        [3].mater_ref   = WHITE,

        /* Ceiling */
        [4].position    = {556.0f, 548.8f, 0.0f},
        [4].normal      = {0.0f, -1.0f, 0.0f},
        [4].mater_ref   = WHITE,
        [5].position    = {556.0f, 548.8f, 559.2f},
        [5].normal      = {0.0f, -1.0f, 0.0f},
        [5].mater_ref   = WHITE,
        [6].position    = {0.0f, 548.8f, 559.2f},
        [6].normal      = {0.0f, -1.0f, 0.0f},
        [6].mater_ref   = WHITE,
        [7].position    = {0.0f, 548.8f, 0.0f},
        [7].normal      = {0.0f, -1.0f, 0.0f},
        [7].mater_ref   = WHITE,

        /* Back wall */
        [8].position    = {549.6f, 0.0f, 559.2f},
        [8].normal      = {0.0f, 0.0f, -1.0f},
        [8].mater_ref   = WHITE,
        [9].position    = {0.0f, 0.0f, 559.2f},
        [9].normal      = {0.0f, 0.0f, -1.0f},
        [9].mater_ref   = WHITE,
        [10].position   = {0.0f, 548.8f, 559.2f},
        [10].normal     = {0.0f, 0.0f, -1.0f},
        [10].mater_ref  = WHITE,
        [11].position   = {556.0f, 548.8f, 559.2f},
        [11].normal     = {0.0f, 0.0f, -1.0f},
        [11].mater_ref  = WHITE,

        /* Left wall */
        [12].position   = {0.0f, 0.0f, 559.2f},
        [12].normal     = {1.0f, 0.0f, 0.0f},
        [12].mater_ref  = GREEN,
        [13].position   = {0.0f, 0.0f, 0.0f},
        [13].normal     = {1.0f, 0.0f, 0.0f},
        [13].mater_ref  = GREEN,
        [14].position   = {0.0f, 548.8f, 0.0f},
        [14].normal     = {1.0f, 0.0f, 0.0f},
        [14].mater_ref  = GREEN,
        [15].position   = {0.0f, 548.8f, 559.2f},
        [15].normal     = {1.0f, 0.0f, 0.0f},
        [15].mater_ref  = GREEN,
        /* Right wall */
        [16].position   = {552.8f, 0.0f, 0.0f},
        [16].normal     = {-1.0f, 0.0f, 0.0f},
        [16].mater_ref  = RED,
        [17].position   = {549.6f, 0.0f, 559.2f},
        [17].normal     = {-1.0f, 0.0f, 0.0f},
        [17].mater_ref  = RED,
        [18].position   = {556.0f, 548.8f, 559.2f},
        [18].normal     = {-1.0f, 0.0f, 0.0f},
        [18].mater_ref  = RED,
        [19].position   = {556.0f, 548.8f, 0.0f},
        [19].normal     = {-1.0f, 0.0f, 0.0f},
        [19].mater_ref  = RED,

        /* Short block */
        [20].position   = {130.0f, 165.0f, 65.0f},
        [20].normal     = {0.0f, 1.0f, 0.0f},
        [20].mater_ref  = MIRROR,
        [21].position   = {82.0f, 165.0f, 225.0f},
        [21].normal     = {0.0f, 1.0f, 0.0f},
        [21].mater_ref  = MIRROR,
        [22].position   = {240.0f, 165.0f, 272.0f},
        [22].normal     = {0.0f, 1.0f, 0.0f},
        [22].mater_ref  = MIRROR,
        [23].position   = {290.0f, 165.0f, 114.0f},
        [23].normal     = {0.0f, 1.0f, 0.0f},
        [23].mater_ref  = MIRROR,

        [24].position   = {290.0f, 0.0f, 114.0},
        [24].normal     = {0.953400, 0.000000, 0.301709},
        [24].mater_ref  = MIRROR,
        [25].position   = {290.0f, 165.0f, 114.0f},
        [25].normal     = {0.953400, 0.000000, 0.301709},
        [25].mater_ref  = MIRROR,
        [26].position   = {240.0f, 165.0f, 272.0f},
        [26].normal     = {0.953400, 0.000000, 0.301709},
        [26].mater_ref  = MIRROR,
        [27].position   = {240.0f, 0.0f, 272.0f},
        [27].normal     = {0.953400, 0.000000, 0.301709},
        [27].mater_ref  = MIRROR,

        [28].position   = {130.0f, 0.0f, 65.0f},
        [28].normal     = {0.292826, 0.000000, -0.956166},
        [28].mater_ref  = MIRROR,
        [29].position   = {130.0f, 165.0f, 65.0f},
        [29].normal     = {0.292826, 0.000000, -0.956166},
        [29].mater_ref  = MIRROR,
        [30].position   = {290.0f, 165.0f, 114.0f},
        [30].normal     = {0.292826, 0.000000, -0.956166},
        [30].mater_ref  = MIRROR,
        [31].position   = {290.0f, 0.0f, 114.0f},
        [31].normal     = {0.292826, 0.000000, -0.956166},
        [31].mater_ref  = MIRROR,

        [32].position   = {82.0f, 0.0f, 225.0f},
        [32].normal     = {-0.957826, 0.000000, -0.287348},
        [32].mater_ref  = MIRROR,
        [33].position   = {82.0f, 165.0f, 225.0f},
        [33].normal     = {-0.957826, 0.000000, -0.287348},
        [33].mater_ref  = MIRROR,
        [34].position   = {130.0f, 165.0f, 65.0f},
        [34].normal     = {-0.957826, 0.000000, -0.287348},
        [34].mater_ref  = MIRROR,
        [35].position   = {130.0f, 0.0f, 65.0f},
        [35].normal     = {-0.957826, 0.000000, -0.287348},
        [35].mater_ref  = MIRROR
        /* Tall block */
};
static int cIndices[1024];

struct geomcache* geomcache_build_test_sample()
{
        int n_vertices = sizeof(cVertices)/sizeof(struct vertex);

        // prepare indices
        int i, j;
        for (i = 0, j = 0; i < n_vertices; i += 4, j += 6) {
                cIndices[j + 0] = i + 0;
                cIndices[j + 1] = i + 1;
                cIndices[j + 2] = i + 2;
                cIndices[j + 3] = i + 0;
                cIndices[j + 4] = i + 2;
                cIndices[j + 5] = i + 3;
        }
        int n_indices = j;

        // prepare vertices
        struct point3d* vertices = alloc_fix(sizeof(struct point3d), n_vertices);
        for (j = 0; j < n_vertices; j ++) {
                point3d_comps(vertices[j].p[i] = cVertices[j].position[i]*c_Proportion);
        }
        // prepare normals
        struct vector3d* normals = alloc_fix(sizeof(struct vector3d), n_vertices);
        for (j = 0; j < n_vertices; j ++) {
                vector3d_comps(normals[j].v[i] = cVertices[j].normal[i]);
        }
        // prepare material references
        int* mater_refs = alloc_fix(sizeof(int), n_vertices);
        for (j = 0; j < n_vertices; j ++) {
                mater_refs[j] = cVertices[j].mater_ref;
        }

        struct geomcache* gc = alloc_obj(gc);
        geomcache_init(gc, UtilAttriVertex | UtilAttriNormal | UtilAttriMatIdList);
        geomcache_accumulate(gc, cIndices, n_indices, n_vertices, vertices, normals, mater_refs);

        free_fix(vertices);
        free_fix(normals);
        free_fix(mater_refs);

        return gc;
}

void geomcache_test_init(struct alg_var_set* envir) {};
void geomcache_test_free(struct alg_var_set* envir) {};
enum DebugPosition* geomcache_test_pos(struct alg_var_set* envir, int* n_pos, int* num_run, bool* is_skipped)
{
        static enum DebugPosition pos[] = {
                Debug_KernelStart
        };
        *n_pos = sizeof(pos)/sizeof(enum DebugPosition);
        *num_run = 1;
        *is_skipped = true;
        return pos;
}
void geomcache_test(struct alg_var_set* envir)
{
        struct geomcache* gc = geomcache_build_test_sample();
        if (!gc) {
                log_severe_err("failed to build geomcache sample");
        }
        geomcache_free(gc);
        free_fix(gc);
}
