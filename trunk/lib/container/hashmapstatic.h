#ifndef HASHTABLE_H_INCLUDED
#define HASHTABLE_H_INCLUDED

#include <container/container.h>

#pragma pack(4)

/* Provides a memory efficient hash table container
 * Some common O(1) operations are defined below */
struct alg_hashmap_static {
        void*   content;
        int*    hash_table;
        int     len;
};

#pragma pack()


#endif // HASHTABLE_H_INCLUDED
