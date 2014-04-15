#include <logout.h>
#include <algorithm.h>
#include <thread.h>
#include <x3d/init.h>
#include <editor/editor.h>
#include <x3d/renderer.h>
#include <x3d/editor.h>

#define MAX_ACTIVEX             2


struct edit_activex_ops {
        void (*update) ( struct edit_activex *activex );
        void (*free) ( struct edit_activex *activex );
};

struct edit_activex {
        char *name;
        bool state;
        uuid_t edit_id;
        struct edit_activex_ops *ops;
        enum EDIT_ACTIVEX_IDR type;
};

struct activex_render_region {
        struct edit_activex activex;
        enum PLATFORM_IDR type;
        void *handle;
        struct irectangle2d rect;
        uuid_t rend_bind;
};

struct activex_entity_list {
        struct edit_activex activex;
};

struct editor {
        char *name;
        uuid_t name_id;
        uuid_t id;
        struct alg_llist activex[MAX_ACTIVEX];
        int n_activex[MAX_ACTIVEX];
};

struct editor_container {
        struct alg_llist editor;
};
static const int c_sizeof_activex[] = {
        [EDIT_ACTIVEX_RENDER_REGION] = sizeof(struct activex_render_region),
        [EDIT_ACTIVEX_ENTITY_LIST]   = sizeof(struct activex_entity_list)
};
static const struct edit_activex_ops c_activex_ops[] = {
};
static struct editor_container g_edit_cont = {0};
static struct edit_ops g_edit_ops = {
        .init_editor = cast(g_edit_ops.init_editor)             load_editor,
        .editor_main_loop = cast(g_edit_ops.editor_main_loop)   edit_main_loop
};
static struct thr_task *g_main_loop_task = nullptr;
static f_Editor_Loop g_loop_func = nullptr;
static void *g_info_ptr = nullptr;

static void init_editor ( char *name, struct editor *edit );
static void free_editor ( struct editor *edit );
static void edit_activex_init ( enum EDIT_ACTIVEX_IDR type, struct edit_activex *activex );


bool init_editor_container ( int *argc, char ***argv )
{
        create_alg_llist ( &g_edit_cont.editor, sizeof ( struct editor ) );
        if ( !g_edit_ops.init_editor ( argc, argv ) ) {
                goto fail;
        }
        return true;
fail:
        kernel_panic ();
        return false;
}

struct editor_container *editor_container_export ( void )
{
        return &g_edit_cont;
}

void editor_import ( struct edit_ops *ops )
{
        memcpy ( &g_edit_ops, ops, sizeof ( *ops ) );
}

static void init_editor ( char *name, struct editor *edit )
{
        memset ( edit, 0, sizeof *edit );
        int i;
        for ( i = 0; i < MAX_ACTIVEX; i ++ ) {
                create_alg_llist (
                        &edit->activex[i], c_sizeof_activex[i] );
        }
        edit->id = alg_gen_uuid ();
        edit->name_id = alg_hash_str_uuid ( name );
        edit->name = alloc_fix ( 1, strlen ( name ) + 1 );
        strcpy ( edit->name, name );
}

static void free_editor ( struct editor *edit )
{
        int i;
        for ( i = 0; i < MAX_ACTIVEX; i ++ ) {
                free_alg_llist ( &edit->activex[i] );
        }
        free_fix ( edit->name );
        memset ( edit, 0, sizeof *edit );
}

uuid_t editor_add ( char *name )
{
        struct editor *new_edit = alg_llist_new ( &g_edit_cont.editor );
        init_editor ( name, new_edit );
        return new_edit->id;
}

struct nameidinfo {
        uuid_t name_id;
        const char *name;
};
#define cmp_editor_name( _info, _elm )         ((_info)->name_id == (_elm)->name_id &&\
                                               (!strcmp ( (_info)->name, (_elm)->name )))
struct editor *editor_get_byname ( char *name )
{
        struct nameidinfo info = {
                .name_id = alg_hash_str_uuid ( name ),
                .name = name
        };
        struct editor *curr_edit;
        alg_llist_find ( &g_edit_cont.editor, &info, &curr_edit, cmp_editor_name );
        return curr_edit;
}
#undef cmp_editor_name

#define cmp_editor_id( _info, _elm )       (*(_info) == (_elm)->id)
struct editor *editor_get_byid ( uuid_t edit )
{
        struct editor *curr_edit;
        alg_llist_find ( &g_edit_cont.editor, &edit, &curr_edit, cmp_editor_id );
        return curr_edit;
}

void editor_remove ( struct editor *edit )
{
        struct editor *curr_edit;
        alg_llist_remove ( &g_edit_cont.editor, &edit->id, &curr_edit, cmp_editor_id );
        free_editor ( edit );
}
#undef cmp_editor_id

void editor_update ( struct editor *edit )
{
        int j;
        for ( j = 0; edit->n_activex[j] > 0; j ++ ) {
                int k;
                struct edit_activex *curr_ax = alg_llist_first (
                                                       &edit->activex[j], &k );
                while ( curr_ax != nullptr ) {
                        if ( curr_ax->state ) {
                                curr_ax->ops->update ( curr_ax );
                        }
                        alg_llist_next ( &edit->activex[j], &k );
                }
        }
}

void editor_update_all ( void )
{
        struct alg_llist *editors = &g_edit_cont.editor;
        int i;
        struct editor *curr_ed = alg_llist_first ( editors, &i );
        while ( curr_ed != nullptr ) {
                editor_update ( curr_ed );
                curr_ed = alg_llist_next ( editors, &i );
        }
}

static void edit_activex_init ( enum EDIT_ACTIVEX_IDR type, struct edit_activex *activex )
{
        activex->type = type;
}

static void edit_activex_add (
        struct edit_activex *activex, char *name, struct editor *edit )
{
        activex->ops = cast(activex->ops) &c_activex_ops[activex->type];
        activex->name = alloc_fix ( 1, strlen(name) + 1 );
        activex->edit_id = edit->id;
        strcpy ( activex->name, name );
        edit->n_activex[activex->type] ++;
}

static void free_activex ( struct edit_activex *activex )
{
        struct editor *curr_ed = editor_get_byid ( activex->edit_id );
        activex->ops->free ( activex );
        free_fix ( activex->name );
        memset ( activex, 0, sizeof *activex );
        curr_ed->n_activex[activex->type] --;
        if ( curr_ed->n_activex[activex->type] < 0 ) {
                log_mild_err_dbg ( "no activex to be freed, type: %d", activex->type );
        }
}

#define sort_activex( _type, _f_Func, _p ) \
{ \
        switch ( _type ) { \
        case EDIT_ACTIVEX_RENDER_REGION: { \
                struct activex_render_region *_ptr = cast(_ptr) _p; \
                _f_Func; \
                _p = cast(_p) _ptr; \
                break; \
        } \
        case EDIT_ACTIVEX_ENTITY_LIST: { \
                struct activex_entity_list *_ptr = cast(_ptr) _p; \
                _f_Func; \
                _p = cast(_p) _ptr; \
                break; \
        } \
        case EDIT_ACTIVEX_FILE_LOADER: { \
                break; \
        } \
        } \
}

void editor_add_activex ( char *name, void *_activex, struct editor *editor )
{
        struct editor *curr_ed = editor;
        struct edit_activex *activex = _activex;
        edit_activex_add ( activex, name, curr_ed );
        sort_activex (
                activex->type,
                alg_llist_add ( _ptr, &curr_ed->activex[activex->type] ),
                activex );
}

#define cmp_activex( _info, _elm )      (!strcmp ( *(_info), ((struct edit_activex *) (_elm))->name ))
void editor_remove_activex ( enum EDIT_ACTIVEX_IDR type, char *name, struct editor *editor )
{
        struct editor *curr_ed = editor;
        struct alg_llist *activex = &curr_ed->activex[type];
        struct edit_activex *curr_ax = nullptr;
        sort_activex (
                type,
                alg_llist_remove ( activex, &name, &_ptr, cmp_activex ),
                curr_ax );
        if ( curr_ax != nullptr ) {
                free_activex ( curr_ax );
        } else {
                log_mild_err_dbg ( "couldn't find such activex of the type: %d with its name: %s", type, name );
                return ;
        }
}

struct edit_activex *editor_find_activex (
        enum EDIT_ACTIVEX_IDR type, char *name, struct editor *editor )
{
        struct editor *curr_ed = editor;
        struct alg_llist *activex = &curr_ed->activex[type];
        struct edit_activex *curr_ax = nullptr;
        sort_activex (
                type,
                alg_llist_find ( activex, &name, &_ptr, cmp_activex ),
                curr_ax );
        if ( curr_ax == nullptr ) {
                log_mild_err_dbg ( "couldn't find such activex of the type: %d with its name: %s", type, name );
        }
        return curr_ax;
}
#undef cmp_activex

void editor_reg_custum_loop ( f_Editor_Loop func, void *info_ptr )
{
        g_loop_func = func;
        g_info_ptr = info_ptr;
}

void editor_enter_loop ( bool use_custom )
{
        if ( use_custom ) {
                g_main_loop_task = thr_run_task (
                                           (f_Thread_Handler) g_loop_func, g_info_ptr, nullptr );
        } else {
                g_main_loop_task = thr_run_task (
                                           (f_Thread_Handler) g_edit_ops.editor_main_loop,
                                           nullptr, nullptr );
        }
}

struct activex_render_region *create_activex_render_region (
        enum PLATFORM_IDR type, void *handle, int x, int y, int w, int h )
{
        struct activex_render_region *rr = alloc_fix ( sizeof *rr, 1 );
        edit_activex_init ( EDIT_ACTIVEX_RENDER_REGION, cast(&rr->activex) rr );
        rr->type = type;
        rr->handle = handle;
        rr->rect.x0 = x;
        rr->rect.y0 = y;
        rr->rect.x1 = x + w;
        rr->rect.y1 = y + h;
        rr->rend_bind = renderer_container_add ( nullptr );
        return rr;
}
