#include <misc.h>
#include <container/paramset.h>

/* @fixme (davis#9#): <paramset> fix everything */

struct alg_var {
        char* name;
        void* ptr;
};

void alg_var_set_init(struct alg_var_set* self)
{
        alg_init(llist, &self->var_set, sizeof(struct alg_var), 1);
}

void alg_var_set_free(struct alg_var_set* self)
{
        alg_free(llist, &self->var_set);
}

void alg_var_set_declare(struct alg_var_set *self, char* name, void* data, int size)
{
        struct alg_var var;
        var.name = name;
        var.ptr  = alloc_fix(size, 1);
        memcpy(var.ptr, data, size);
        alg_iter(struct alg_var) iter;
#define cmp_name( _data, _iter )         (!strcmp((_data).name, (_iter)->name))
        alg_find(llist, &self->var_set, var, iter, cmp_name);
        if (iter != nullptr) {
                struct alg_var* exist = &alg_access(iter);
                exist->ptr = var.ptr;
        } else {
                var.name = alg_alloc_string(name);
                alg_push_back(llist, &self->var_set, var);
        }
#undef cmp_name
}


void alg_var_set_undeclare(struct alg_var_set* self, char* name)
{
}

void* alg_var_set_use(struct alg_var_set* self, char* name)
{
        alg_iter(struct alg_var) iter;
#define cmp_name( _data, _iter )         (!strcmp(_data, (_iter)->name))
        alg_find(llist, &self->var_set, name, iter, cmp_name);
        if (iter != nullptr) {
                struct alg_var* exist = &alg_access(iter);
                return exist->ptr;
        } else {
                return nullptr;
        }
#undef cmp_name
}
