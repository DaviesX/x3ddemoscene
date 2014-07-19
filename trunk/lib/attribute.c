/* attribute.c: attribute utilities */
#include <math/math.h>
#include <memory.h>
#include <attribute.h>
/*
static const int cSize[2] = {
        [ATTRI_INT]   = sizeof(int),
        [ATTRI_FLOAT] = sizeof(float)
};

void attr_init ( struct attribute* attr )
{
        zero_obj ( attr );
}

void attr_set_i ( struct attribute* attr, int i, enum ATTRI_IDR type )
{
        attr->n_comp = max(attr->n_comp, i);
        attr->offset[i] = cSize[type];
}

void attr_finalize ( struct attribute* attr )
{
        attr->size = 0;
        int i;
        for ( i = 0; i < attr->n_comp; i ++ ) {
                int tsize = attr->offset[i];
                attr->offset[i] = attr->size;
                attr->size += tsize;
        }
}

int attr_size ( struct attribute* attr )
{
        return attr->size;
}

int attr_n ( struct attribute* attr )
{
        return attr->n_comp;
}

void attr_bind_dest ( struct attribute* attr, int i, void** dest )
{
        attr->dest[i] = dest;
}

void attr_bind_src ( struct attribute* attr, void* src )
{
        attr->src = src;
}

void attr_link_individual ( struct attribute* attr )
{
        int i;
        for ( i = 0; i < attr->n_comp; i ++ )
                *attr->dest[i] = ((untyped*) attr->src) + attr->offset[i];
}

void attr_link_block ( struct attribute* attr )
{
        *attr->dest[0] = attr->src;
}

void attr_set_lerp_operator ( struct attribute* attr, f_Attri_Lerp op )
{
        attr->lerp_op = op;
}

void attr_op_lerp ( struct attribute* attr, void* v0, void* v1, float t, void* r )
{
        attr->lerp_op ( v0, v1, t, r );
}

void attr_auto_lerp ( struct attribute* attr, void* v0, void* v1, float t, void* r )
{
        int i;
        for ( i = 0; i < attr->n_comp; i ++ ) {
                switch ( attr->format[i] ) {
                case ATTRI_INT:
                        *(int*) r = *(int*) v0 + (*(int*) v1 - *(int*) v0)*t;
                        break;
                case ATTRI_FLOAT:
                        *(float*) r = *(float*) v0 + (*(float*) v1 - *(float*) v0)*t;
                        break;
                }
        }
}
*/
