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


#endif // STACK_H_INCLUDED
