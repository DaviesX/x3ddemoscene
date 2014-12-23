#ifndef CORNELLBOX_H_INCLUDED
#define CORNELLBOX_H_INCLUDED


#define RED     0
#define GREEN   1
#define BLUE    2
#define WHITE   3
#define MIRROR  4


struct vertex {
        float   position[3];
        float   normal[3];
        int     mater_ref;
};

void cornell_box_make ( struct vertex** vertex, int* n_vertex, int** index, int* n_index );
void cornell_box_free ( struct vertex* vertex, int* index );


#endif // CORNELLBOX_H_INCLUDED
