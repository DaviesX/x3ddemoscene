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
struct edit_activex;
struct activex_render_region;
struct activex_enity_list;

enum EDIT_ACTIVEX_IDR {
        EDIT_ACTIVEX_RENDER_REGION,
        EDIT_ACTIVEX_ENTITY_LIST,
        EDIT_ACTIVEX_FILE_LOADER
};

struct edit_ops {
        bool (*init_editor) ( int *argc, char ***argv );
        void (*editor_main_loop) ( void );
};

/*
 * functions' declaration
 */
bool init_editor_container ( int *argc, char ***argv );
void editor_import ( struct edit_ops *ops );
struct editor_container *editor_container_export ( void );
uuid_t editor_add ( char *name );
struct editor *editor_get_byname ( char *name );
struct editor *editor_get_byid ( uuid_t edit );
void editor_remove ( struct editor *editor );
void editor_update ( struct editor *editor );
void editor_update_all ( void );
void editor_add_activex ( char *name, void *_activex, struct editor *editor );
void editor_remove_activex ( enum EDIT_ACTIVEX_IDR type, char *name, struct editor *editor );
struct edit_activex *editor_find_activex (
        enum EDIT_ACTIVEX_IDR type, char *name, struct editor *editor );

struct activex_render_region *create_activex_render_region (
        enum PLATFORM_IDR type, void *handle, int x, int y, int w, int h );


#endif // EDITOR_MEDIA_H_INCLUDED
