#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED


#include <container/container.h>

/* Utility macro functions */
#define init_cdecl_stack( _stack ) \
{ \
        (_stack)->stack_ptr = (_stack)->tmp_stack; \
        /* (_stack)->n_ref = 0; */ \
}
#define push_cdecl_stack( _stack, _data ) \
{ \
        *(typeof (_data) *)(_stack)->stack_ptr = (_data); \
        (_stack)->stack_ptr += sizeof ( typeof (_data) ); \
}
#define start_cdecl_stack			init_cdecl_stack
#define pop_cdecl_stack( _stack, _value ) \
{ \
        (_value) = *((typeof (_value) *) (_stack)->stack_ptr); \
        (_stack)->stack_ptr += sizeof ( typeof (_value) ); \
}
#define end_cdecl_stack				init_cdecl_stack

#define begin_tree_traversal( _id ) \
{ \
        TRAVERSE_BEGIN_##_id: /* begins */; \
}
#define end_tree_traversal( _id ) \
{ \
        goto TRAVERSE_BEGIN_##_id;\
        TRAVERSE_END_##_id: /* ends */; \
}
#define enter_tree_branch( _enter_code ) \
{\
        _enter_code\
}
#define backtrack_tree_branch( _stack, _id, _backtrack_code ) \
{ \
        if ( (_stack)->stack_ptr == (untyped *) (_stack) ) { \
                goto TRAVERSE_END_##_id; \
        } else { \
                _backtrack_code \
                goto TRAVERSE_BEGIN_##_id; \
        } \
}


/* stimulate an asm-free stack which
 * can be declared with custom settings */
#define declare_stack( _name, _size )	\
        struct {\
                untyped stack[_size];\
                untyped *stack_ptr;\
        } _name;

#define init_stack( _stack )                    ((_stack)->stack_ptr = (untyped *) (_stack))
#define push_stack( _stack, _a ) \
{ \
        *(typeof (_a) *) (_stack)->stack_ptr = _a; \
        (_stack)->stack_ptr += sizeof ( _a ); \
        assert ( (_stack)->stack_ptr <= (_stack)->stack + sizeof ( (_stack)->stack ) ); \
}

#define pop_stack( _stack, _a ) \
{ \
        (_stack)->stack_ptr -= sizeof ( _a ); \
        assert ( (_stack)->stack_ptr >= (_stack)->stack ); \
        _a = *(typeof (_a) *) (_stack)->stack_ptr; \
}

#define stack_is_empty( _stack )                ((_stack)->stack_ptr == (_stack)->stack)
#define stack_ptr( _stack )                     ((void*)(_stack)->stack_ptr)
#define stack_push_ptr( _stack, _ptr ) \
{ \
        ((_stack)->stack_ptr += sizeof *(_ptr)); \
        assert ( (_stack)->stack_ptr <= (_stack)->stack + sizeof ( (_stack)->stack ) ); \
}

#define stack_pop_ptr( _stack, _ptr ) \
{ \
        (_ptr) = (typeof(_ptr)) (_stack)->stack_ptr; \
        assert ( (_stack)->stack_ptr >= (_stack)->stack ); \
        (_stack)->stack_ptr -= sizeof *(_ptr); \
}

#define get_stack_last( _stack, _a ) \
{ \
        untyped *t = (_stack)->stack_ptr - sizeof ( _a ); \
        _a = *(typeof (_a) *) t; \
}

#define check_stack( _stack )                    ((_stack)->stack != (_stack)->stack_ptr)




#include <system/allocator.h>


/*
 * <stack> decl
 * heavy weight stack implementation, size of the stack is unlimited.
 */
#define stack_templ(T)                  \
struct {                                \
        T*              values;        \
        int             num_elm;        \
}

/*
 * <stack> public
 */
#define stack_init(_self, _init_count)                                                  \
{                                                                                       \
        zero_obj(_self);                                                                \
        if (_init_count < 0) {                                                          \
                (_self)->values = alloc_var(sizeof(*(_self)->values), 0);               \
        } else {                                                                        \
                (_self)->values = alloc_var(sizeof(*(_self)->values), _init_count);     \
        }                                                                               \
        (_self)->num_elm   = 0;                                                         \
}

#define stack_free(_self)               \
{                                       \
        free_var((_self)->values);      \
        zero_obj(_self);                \
}

#define stack_expand(_self, _count)                                     \
{                                                                       \
        (_self)->values = alloc_var_expand2((_self)->values, count);    \
}

#define stack_flush(_self)                      \
{                                               \
        alloc_var_flush((_self)->values);       \
        (_self)->num_elm = 0;                   \
}

/* Both lists must have the same element type */
#define stack_copy(_self, _to_copy)                                                             \
{                                                                                               \
        alloc_var_expand((_self)->values, (_to_copy)->num_elm);                                 \
        memcpy((_self)->values, _to_copy, (_to_copy)->num_elm*sizeof(*(_self)->values));        \
        (_self)->num_elm = (_to_copy)->num_elm;                                                 \
}

/* swaps the self's pointer respectively */
#define stack_swap(_self, _to_swap)                     \
{                                                       \
        void* p                 = (_self)->values;      \
        (_self)->values        = (_to_swap)->values;    \
        (_to_swap)->values     = p;                     \
        int t                   = (_self)->num_elm;     \
        (_self)->num_elm        = (_to_swap)->num_elm;  \
        (_to_swap)->num_elm     = t;                    \
}

#define stack_push_back(_self, _data)                                                   \
{                                                                                       \
        (_self)->values                        = alloc_var_add((_self)->values, 1);     \
        (_self)->values[(_self)->num_elm]      = (_data);                               \
        (_self)->num_elm ++;                                                            \
}

#define stack_pop_back(_self)                                   \
{                                                               \
        (_self)->values = alloc_var_dec((_self)->values, 1);    \
        (_self)->num_elm --;                                    \
}

#define stack_peek(_self, _data)                                \
{                                                               \
        (_data) = (_self)->values[(_self)->num_elm - 1];        \
}

#define stack_size(_self)                ((_self)->num_elm)

#define stack_to_array(_self)            ((_self)->values)

/*
 * <stack_iterator> decl
 */
#define stack_iter_templ(T)     \
struct {                        \
        T*      curr_pos;       \
        T*      last;           \
}
/*
 * <stack_iterator> public
 */
#define stack_iter_begin(_self, _list)                          \
{                                                               \
        (_self)->curr_pos = (_list)->values;                    \
        (_self)->last = &(_list)->values[(_list)->num_elm];     \
}

#define stack_iter_next(_self, _list)                           \
{                                                               \
        (_self)->curr_pos ++;                                   \
}

#define stack_iter_has_value(_self, _list)                      \
        ((_self)->curr_pos != (_self)->last)

#define stack_iter_deref(_self, _list)                          \
        (*(_self)->curr_pos)

#define stack_for_each(_self, _elm, CODE)                       \
{                                                               \
        stack_iter_templ(typeof(*(_self)->values)) _iterator;   \
        stack_iter_begin(&_iterator, _self);                    \
        while (stack_iter_has_value(&_iterator, _self)) {       \
                (_elm) = stack_iter_deref(&_iterator, _self);   \
                CODE;                                           \
                stack_iter_next(&_iterator, _self);             \
        }                                                       \
}

#endif // STACK_H_INCLUDED
