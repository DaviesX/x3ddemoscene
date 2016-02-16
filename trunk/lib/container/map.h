#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#define map_templ(K, T)         \
struct {                        \
        struct {                \
                K*      keys;   \
                T*      content;\
        } *pairs;               \
        int*    bins;           \
        int*    next;           \
        int     num_elm;        \
}


#endif	// MAP_H_INCLUDED
