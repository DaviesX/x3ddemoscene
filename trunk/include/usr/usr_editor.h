#ifndef EDITOR_MEDIA_H_INCLUDED
#define EDITOR_MEDIA_H_INCLUDED

#include <math/math.h>
#include <algorithm.h>
#include <serializer.h>
#include <x3d/renderer.h>


enum PLATFORM_IDR {
        PLATFORM_DEFAULT,
        PLATFORM_GTK,
        PLATFORM_WIN32,
        PLATFORM_QT
};

struct editor;
struct editor_container;

struct ax_render_region;
struct ax_asset_list;
struct ax_entity_property;
struct ax_renderable_property;
struct ax_dynamics_property;
struct ax_view_control;
struct ax_selector;
struct ax_render_conf;


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
void edit_activex_init ( enum EDIT_ACTIVEX_IDR type, struct edit_activex *activex );
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
__dlexport void editor_load_state ( char *filename, struct editor *editor );
__dlexport void editor_save_state ( char *filename, struct editor *editor );

/* activex - render region */
void ax_render_region_dispatch ( struct ax_render_region *region );
void ax_render_region_update ( struct ax_render_region *region );
void ax_render_region_load_state ( struct serializer *s, struct ax_render_region *region );
void ax_render_region_save_state ( struct serializer *s, struct ax_render_region *region );
void ax_render_region_free ( struct ax_render_region *region );

void ax_render_region_set_renderer (
        struct renderer *rend, struct ax_render_region *region );
void ax_render_region_notify_idle ( bool is_idle, struct ax_render_region *region );

typedef void (*f_Notify_idle) ( bool is_idle, void *handle, void *info );
typedef void (*f_Notify_resize) ( int width, int height, bool is_fullscreen,
                                  void *handle, void *info );


__dlexport struct ax_render_region *create_ax_render_region (
        enum PLATFORM_IDR type, void *handle, int x, int y, int w, int h );
__dlexport void ax_render_region_set_renderer (
        struct renderer *rend, struct ax_render_region *region );
__dlexport void ax_render_region_resize ( int x, int y, int w, int h, bool toggle_fullscreen,
                                          struct ax_render_region *region );
__dlexport void ax_render_region_bind_signal ( char *signal, f_Generic func, void *info,
                                               struct ax_render_region *region );

/* activex - render configurator */
void ax_render_conf_dispatch ( struct ax_render_conf *conf );
void ax_render_conf_update ( struct ax_render_conf *conf );
void ax_render_conf_load_state ( struct serializer *s, struct ax_render_conf *conf );
void ax_render_conf_save_state ( struct serializer *s, struct ax_render_conf *conf );
void ax_render_conf_free ( struct ax_render_conf *conf );

typedef void (*f_Notify_Error) ( char *message, struct ax_render_conf *conf, void *info );


__dlexport struct ax_render_conf *create_ax_render_conf ( char *activate );
__dlexport void ax_render_conf_tab ( char *tab, struct ax_render_conf *conf );
__dlexport void ax_render_conf_value ( struct ax_render_conf *conf, char *target, ... );
__dlexport bool ax_render_conf_apply ( struct ax_render_conf *conf );
__dlexport void ax_render_conf_cancel ( struct ax_render_conf *conf );
__dlexport void *ax_render_conf_checkout ( char *target, char *type, struct ax_render_conf *conf );
__dlexport void ax_render_conf_bind_signal ( char *signal, f_Generic func, void *info,
                                             struct ax_render_conf *conf );


#endif // EDITOR_MEDIA_H_INCLUDED
