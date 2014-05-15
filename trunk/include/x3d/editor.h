#ifndef EDITOR_MEDIA_H_INCLUDED
#define EDITOR_MEDIA_H_INCLUDED

#include <math/math.h>
#include <algorithm.h>
#include <x3d/renderer.h>


enum PLATFORM_IDR {
        PLATFORM_DEFAULT,
        PLATFORM_GTK,
        PLATFORM_WIN32,
        PLATFORM_QT
};

struct editor;
struct editor_container;

struct activex_render_region;
struct activex_asset_list;
struct activex_view_control;
struct activex_selector;
struct activex_entity_property;
struct activex_renderable_property;
struct activex_dynamics_property;

enum EDIT_ACTIVEX_IDR {
        EDIT_ACTIVEX_RENDER_REGION,
        EDIT_ACTIVEX_ENTITY_LIST,
        EDIT_ACTIVEX_FILE_LOADER
};

struct edit_ops {
        bool (*editor_init) ( int *argc, char ***argv );
        bool (*editor_end_init) ( void );
        bool (*editor_load) ( void );
        void (*editor_loop) ( void *info );
        bool (*editor_free) ( void );
};

struct edit_activex {
        int size;
        char *name;
        bool state;
        uuid_t edit_id;
        struct edit_activex_ops *ops;
        enum EDIT_ACTIVEX_IDR type;
};

typedef bool (*f_Editor_Loop) ( void *info );

/*
 * functions' declaration
 */
bool editor_kernel_preinit ( int *argc, char ***argv );
bool editor_kernel_init ( void );
void editor_kernel_done_init ( void );
void editor_kernel_free ( void );
void editor_import ( struct edit_ops *ops );
struct editor_container *editor_container_export ( void );
__dlexport uuid_t editor_add ( char *name );
__dlexport struct editor *editor_get_byname ( char *name );
__dlexport struct editor *editor_get_byid ( uuid_t edit );
__dlexport void editor_remove ( struct editor *editor );
__dlexport void editor_update ( struct editor *editor );
__dlexport void editor_update_all ( void );
__dlexport void editor_add_activex ( char *name, void *_activex, struct editor *editor );
__dlexport void editor_remove_activex ( enum EDIT_ACTIVEX_IDR type, char *name, struct editor *editor );
__dlexport struct edit_activex *editor_find_activex (
        enum EDIT_ACTIVEX_IDR type, char *name, struct editor *editor );
__dlexport void editor_reg_custum_loop ( f_Editor_Loop func, void *info_ptr );
__dlexport void editor_run ( bool use_custum );
__dlexport void editor_dispatch_signal ( struct editor *editor );
void edit_activex_init ( enum EDIT_ACTIVEX_IDR type, struct edit_activex *activex );

/* activex - render region */
__dlexport struct activex_render_region *create_ax_render_region (
        enum PLATFORM_IDR type, void *handle, int x, int y, int w, int h );
void ax_render_region_update ( struct activex_render_region *activex );
void ax_render_region_free ( struct activex_render_region *activex );


#endif // EDITOR_MEDIA_H_INCLUDED
