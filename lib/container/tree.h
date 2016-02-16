#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED


/*
 * <btree> decl
 */
#define btree_templ(T, STT)     \
struct STT {                    \
        T       value;          \
        STT*    parent;         \
        STT*    child[2];       \
}

#endif	// TREE_H_INCLUDED
