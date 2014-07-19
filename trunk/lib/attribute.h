#ifndef ATTRIBUTE_H_INCLUDED
#define ATTRIBUTE_H_INCLUDED

enum ATTRI_IDR {
        ATTRI_INT,
        ATTRI_FLOAT
};

typedef void (*f_Attri_Lerp) ( void* v0, void* v1, float t, void* r );

struct attribute {
        int             offset[10];
        void*           src;
        void**          dest[10];
        int             n_comp;
        int             size;
        f_Attri_Lerp    lerp_op;
        enum ATTRI_IDR  format[10];
};

/*
 * functions' declaration
 */
void attr_init ( struct attribute* attr );
void attr_set_i ( struct attribute* attr, int i, enum ATTRI_IDR type );
void attr_finalize ( struct attribute* attr );
int attr_size ( struct attribute* attr );
int attr_n ( struct attribute* attr );
void attr_bind_dest ( struct attribute* attr, int i, void** dest );
void attr_bind_src ( struct attribute* attr, void* src );
void attr_link_individual ( struct attribute* attr );
void attr_link_block ( struct attribute* attr );
void attr_set_lerp_operator ( struct attribute* attr, f_Attri_Lerp op );
void attr_op_lerp ( struct attribute* attr, void* v0, void* v1, float t, void* r );
void attr_auto_lerp ( struct attribute* attr, void* v0, void* v1, float t, void* r );


#endif // ATTRIBUTE_H_INCLUDED
