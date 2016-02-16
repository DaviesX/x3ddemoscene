#ifndef SET_H_INCLUDED
#define SET_H_INCLUDED


#define set_templ(T)            \
struct {                        \
        T*      content;        \
        int*    bins;           \
        int*    next;           \
        int     num_elm;        \
}


#endif	// SET_H_INCLUDED
