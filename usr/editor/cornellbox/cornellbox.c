#include <stdlib.h>
#include <system/allocator.h>
#include "cornellbox.h"



static const struct vertex cVertices[] = {
        /* Floor */
        [0].position    = {552.8f, 0.0f, 0.0f},
        [0].normal      = {0.0f, 1.0f, 0.0f},
        [0].mater_ref   = WHITE,
        [1].position    = {0.0f, 0.0f, 0.0f},
        [1].normal      = {0.0f, 1.0f, 0.0f},
        [1].mater_ref   = WHITE,
        [2].position    = {0.0f, 0.0f, 559.2f},
        [2].normal      = {0.0f, 1.0f, 0.0f},
        [2].mater_ref   = WHITE,
        [3].position    = {549.6f, 0.0f, 559.2f},
        [3].normal      = {0.0f, 1.0f, 0.0f},
        [3].mater_ref   = WHITE,

        /* Ceiling */
        [4].position    = {556.0f, 548.8f, 0.0f},
        [4].normal      = {0.0f, -1.0f, 0.0f},
        [4].mater_ref   = WHITE,
        [5].position    = {556.0f, 548.8f, 559.2f},
        [5].normal      = {0.0f, -1.0f, 0.0f},
        [5].mater_ref   = WHITE,
        [6].position    = {0.0f, 548.8f, 559.2f},
        [6].normal      = {0.0f, -1.0f, 0.0f},
        [6].mater_ref   = WHITE,
        [7].position    = {0.0f, 548.8f, 0.0f},
        [7].normal      = {0.0f, -1.0f, 0.0f},
        [7].mater_ref   = WHITE,

        /* Back wall */
        [8].position    = {549.6f, 0.0f, 559.2f},
        [8].normal      = {0.0f, 0.0f, -1.0f},
        [8].mater_ref   = WHITE,
        [9].position    = {0.0f, 0.0f, 559.2f},
        [9].normal      = {0.0f, 0.0f, -1.0f},
        [9].mater_ref   = WHITE,
        [10].position   = {0.0f, 548.8f, 559.2f},
        [10].normal     = {0.0f, 0.0f, -1.0f},
        [10].mater_ref  = WHITE,
        [11].position   = {556.0f, 548.8f, 559.2f},
        [11].normal     = {0.0f, 0.0f, -1.0f},
        [11].mater_ref  = WHITE,

        /* Left wall */
        [12].position   = {0.0f, 0.0f, 559.2f},
        [12].normal     = {1.0f, 0.0f, 0.0f},
        [12].mater_ref  = GREEN,
        [13].position   = {0.0f, 0.0f, 0.0f},
        [13].normal     = {1.0f, 0.0f, 0.0f},
        [13].mater_ref  = GREEN,
        [14].position   = {0.0f, 548.8f, 0.0f},
        [14].normal     = {1.0f, 0.0f, 0.0f},
        [14].mater_ref  = GREEN,
        [15].position   = {0.0f, 548.8f, 559.2f},
        [15].normal     = {1.0f, 0.0f, 0.0f},
        [15].mater_ref  = GREEN,
        /* Right wall */
        [16].position   = {552.8f, 0.0f, 0.0f},
        [16].normal     = {-1.0f, 0.0f, 0.0f},
        [16].mater_ref  = RED,
        [17].position   = {549.6f, 0.0f, 559.2f},
        [17].normal     = {-1.0f, 0.0f, 0.0f},
        [17].mater_ref  = RED,
        [18].position   = {556.0f, 548.8f, 559.2f},
        [18].normal     = {-1.0f, 0.0f, 0.0f},
        [18].mater_ref  = RED,
        [19].position   = {556.0f, 548.8f, 0.0f},
        [19].normal     = {-1.0f, 0.0f, 0.0f},
        [19].mater_ref  = RED,

        /* Short block */
        [20].position   = {130.0f, 165.0f, 65.0f},
        [20].normal     = {0.0f, 1.0f, 0.0f},
        [20].mater_ref  = MIRROR,
        [21].position   = {82.0f, 165.0f, 225.0f},
        [21].normal     = {0.0f, 1.0f, 0.0f},
        [21].mater_ref  = MIRROR,
        [22].position   = {240.0f, 165.0f, 272.0f},
        [22].normal     = {0.0f, 1.0f, 0.0f},
        [22].mater_ref  = MIRROR,
        [23].position   = {290.0f, 165.0f, 114.0f},
        [23].normal     = {0.0f, 1.0f, 0.0f},
        [23].mater_ref  = MIRROR,

        [24].position   = {290.0f, 0.0f, 114.0},
        [24].normal     = {0.953400, 0.000000, 0.301709},
        [24].mater_ref  = MIRROR,
        [25].position   = {290.0f, 165.0f, 114.0f},
        [25].normal     = {0.953400, 0.000000, 0.301709},
        [25].mater_ref  = MIRROR,
        [26].position   = {240.0f, 165.0f, 272.0f},
        [26].normal     = {0.953400, 0.000000, 0.301709},
        [26].mater_ref  = MIRROR,
        [27].position   = {240.0f, 0.0f, 272.0f},
        [27].normal     = {0.953400, 0.000000, 0.301709},
        [27].mater_ref  = MIRROR,

        [28].position   = {130.0f, 0.0f, 65.0f},
        [28].normal     = {0.292826, 0.000000, -0.956166},
        [28].mater_ref  = MIRROR,
        [29].position   = {130.0f, 165.0f, 65.0f},
        [29].normal     = {0.292826, 0.000000, -0.956166},
        [29].mater_ref  = MIRROR,
        [30].position   = {290.0f, 165.0f, 114.0f},
        [30].normal     = {0.292826, 0.000000, -0.956166},
        [30].mater_ref  = MIRROR,
        [31].position   = {290.0f, 0.0f, 114.0f},
        [31].normal     = {0.292826, 0.000000, -0.956166},
        [31].mater_ref  = MIRROR,

        [32].position   = {82.0f, 0.0f, 225.0f},
        [32].normal     = {-0.957826, 0.000000, -0.287348},
        [32].mater_ref  = MIRROR,
        [33].position   = {82.0f, 165.0f, 225.0f},
        [33].normal     = {-0.957826, 0.000000, -0.287348},
        [33].mater_ref  = MIRROR,
        [34].position   = {130.0f, 165.0f, 65.0f},
        [34].normal     = {-0.957826, 0.000000, -0.287348},
        [34].mater_ref  = MIRROR,
        [35].position   = {130.0f, 0.0f, 65.0f},
        [35].normal     = {-0.957826, 0.000000, -0.287348},
        [35].mater_ref  = MIRROR
        /* Tall block */
};

static int cIndices[1024];

void cornell_box_make(struct vertex** vertex, int* n_vertex, int** index, int* n_index)
{
        int i;
        int j;
        for ( i = 0, j = 0; i < sizeof(cVertices)/sizeof(struct vertex); i += 4, j += 6 ) {
                cIndices[j + 0] = i + 0;
                cIndices[j + 1] = i + 1;
                cIndices[j + 2] = i + 2;
                cIndices[j + 3] = i + 0;
                cIndices[j + 4] = i + 2;
                cIndices[j + 5] = i + 3;
        }

        *vertex = malloc ( sizeof(cVertices) );
        memcpy ( *vertex, cVertices, sizeof(cVertices) );
        *n_vertex = sizeof(cVertices)/sizeof(struct vertex);

        *index = malloc ( sizeof(cIndices) );
        memcpy ( *index, cIndices, sizeof(cIndices) );
        *n_index = j;
}

void cornell_box_free(struct vertex* vertex, int* index)
{
        free(vertex);
        free(index);
}
