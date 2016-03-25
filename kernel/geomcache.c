#include <system/allocator.h>
#include <math/math.h>
#include <x3d/geomcache.h>


/*
 * <util_aos> public
 */
void u_aos_init(struct util_aos* self, enum UtilAttribute format)
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
        if (format & UtilAttriMatId) {
                self->aos[_AttriMatId].s_data = alloc_var(sizeof(int), 1);
                self->avail[_AttriMatId] = true;
                self->n_streams ++;
        }
        if (format & UtilAttriMatIdList) {
                self->aos[_AttriMatIdList].s_data = alloc_var(sizeof(int), 1);
                self->avail[_AttriMatIdList] = true;
                self->n_streams ++;
        }

        self->format = format;
}

void u_aos_free(struct util_aos* self)
{
        free_var(self->index);
        self->n_index = 0;
        self->n_vertex = 0;
        int i;
        for (i = 0; i < 10; i ++) {
                free_var(self->aos[i].s_data);
        }
        zero_obj(self);
}

void u_aos_flush(struct util_aos* self)
{
        alloc_var_flush(self->index);
        self->n_index = 0;
        self->n_vertex = 0;
        int i;
        for (i = 0; i < 10; i ++) {
                alloc_var_flush(self->aos[i].s_data);
        }
}

void u_aos_accumulate(struct util_aos* self, int* index, int num_index, int num_vertex, ...)
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
                self->aos[_AttriMatIdList].s_data = alloc_var_add(self->aos[_AttriMatIdList].s_data, num_vertex);
                int* matid = self->aos[_AttriMatIdList].s_data;
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

int u_aos_get_vertices(struct util_aos* self, void* vertex[], int* n_streams)
{
        int i;
        for(i = 0; i < 10; i ++) {
                vertex[i] = self->aos[i].s_data;
        }
        *n_streams = self->n_streams;
        return self->n_vertex;
}

void* u_aos_get_indices(struct util_aos* self, int* n_index)
{
        *n_index = self->n_index;
        return self->index;
}

bool* u_aos_get_availibility(struct util_aos* self)
{
        return self->avail;
}
