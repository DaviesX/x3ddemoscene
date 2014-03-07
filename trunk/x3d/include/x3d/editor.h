#ifndef EDITOR_MEDIA_H_INCLUDED
#define EDITOR_MEDIA_H_INCLUDED

#include <math/math.h>
#include <algorithm.h>
#include <x3d/renderer.h>


enum EDIT_MODE {
        EDIT_RUN_EDITOR,
        EDIT_RUN_DEMO
};

enum PLATFORM_HANDLE {
        PLAT_HANDLE_NULL,
        PLAT_HANDLE_GTK,
        PLAT_HANDLE_WIN32,
        PLAT_HANDLE_QT
};


struct alg_named_params;
struct editor;

struct info_bridge {
        struct alg_named_params params;
};

struct render_region {
        enum PLATFORM_HANDLE type;
        void *handle;
        struct irectangle2d rect;
        renderer_handle_t rend_bind;
};

struct edit_ops {
        bool (*init_editor) ( int *argc, char ***argv, enum EDIT_MODE mode );
        void (*editor_main_loop) ( void );
};

/*
 * functions' declaration
 */
bool init_editor ( int *argc, char ***argv );
void import_editor ( struct edit_ops *ops );
struct editor *export_editor ( void );
struct render_region *get_first_render_region ( int32_t *pos );
struct render_region *get_next_render_region ( int32_t *pos );
void update_editor ( void );

/*
 * RI interface
 */
struct info_bridge *get_info_bridge ( void );
void render_region_add ( struct render_region *rr );
struct render_region *render_region_find ( struct render_region *rr );
void render_region_remove ( struct render_region *rr );
void render_region_flush ( void );


#endif // EDITOR_MEDIA_H_INCLUDED
