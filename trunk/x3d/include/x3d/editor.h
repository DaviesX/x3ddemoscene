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
	void *(*get_first_render_region) ( struct render_region *rr );
	void *(*get_next_render_region) ( void *ptr, struct render_region *rr );
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
void add_render_region ( struct render_region *rr );
struct render_region *find_render_region ( struct render_region *rr );
void remove_render_region ( struct render_region *rr );
void flush_render_region ( void );


#endif // EDITOR_MEDIA_H_INCLUDED
