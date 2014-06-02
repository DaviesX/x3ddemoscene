#include <logout.h>
#include <algorithm.h>
#include <thread.h>
#include <serializer.h>
#include <x3d/init.h>
#include <editor/editor.h>
#include <x3d/renderer.h>
#include <x3d/editor.h>

#define MAX_ACTIVEX             2


struct edit_activex_ops {
        void (*update) ( struct edit_activex *activex );
        void (*dispatch) ( struct edit_activex *activex );
        void (*load) ( struct serializer *s, struct edit_activex *activex );
        void (*save) ( struct serializer *s, struct edit_activex *activex );
        void (*free) ( struct edit_activex *activex );
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

static const struct edit_activex_ops c_ax_ops[] = {
        [EDIT_ACTIVEX_RENDER_REGION].update = cast(c_ax_ops->update)      ax_render_region_update,
        [EDIT_ACTIVEX_RENDER_REGION].dispatch = cast(c_ax_ops->dispatch)  ax_render_region_dispatch,
        [EDIT_ACTIVEX_RENDER_REGION].free = cast(c_ax_ops->free)          ax_render_region_free
};
static struct editor_container g_edit_cont = {0};
static struct edit_ops g_edit_ops = {
        .editor_init =          editor_init,
        .editor_end_init =      editor_end_init,
        .editor_load =          editor_load,
        .editor_loop =          editor_loop,
        .editor_free =          editor_free
};
static struct thr_task *g_main_loop_task = nullptr;
static f_Editor_Loop g_loop_func = nullptr;
static void *g_info_ptr = nullptr;

static void init_editor ( char *name, struct editor *edit );
static void free_editor ( struct editor *edit );
static void edit_activex_add (
        struct edit_activex *activex, char *name, struct editor *edit );
static void free_activex ( struct edit_activex *activex );


bool editor_kernel_preinit ( int *argc, char ***argv )
{
        if ( !g_edit_ops.editor_init ( argc, argv ) ) {
                return false;
        }
        return true;
}

bool editor_kernel_init ( void )
{
        create_alg_llist ( &g_edit_cont.editor, sizeof ( struct editor ) );
        if ( !g_edit_ops.editor_load () ) {
                goto fail;
        }
        return true;
fail:
        kernel_panic ();
        return false;
}

void editor_kernel_done_init ( void )
{
        g_edit_ops.editor_end_init ();
}

void editor_kernel_free ( void )
{
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
                        &edit->activex[i], sizeof (struct edit_activex *) );
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
                int j;
                for ( j = 0; j < edit->n_activex[i]; j ++ ) {
                        struct edit_activex **ax = nullptr;
                        alg_llist_i ( &edit->activex[i], j, &ax );
                        free_activex ( *ax );
                }
                free_alg_llist ( &edit->activex[i] );
        }
        free_fix ( edit->name );
        memset ( edit, 0, sizeof *edit );
}

__dlexport uuid_t editor_add ( char *name )
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
__dlexport struct editor *editor_get_byname ( char *name )
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
__dlexport struct editor *editor_get_byid ( uuid_t edit )
{
        struct editor *curr_edit;
        alg_llist_find ( &g_edit_cont.editor, &edit, &curr_edit, cmp_editor_id );
        return curr_edit;
}

__dlexport void editor_remove ( struct editor *edit )
{
        struct editor *curr_edit;
        alg_llist_remove ( &g_edit_cont.editor, &edit->id, &curr_edit,
                           cmp_editor_id );
        free_editor ( edit );
}
#undef cmp_editor_id

__dlexport void editor_update ( struct editor *edit )
{
        int j;
        for ( j = 0; j < MAX_ACTIVEX; j ++ ) {
                if ( edit->n_activex[j] == 0 )
                        continue;
                int k;
                struct edit_activex **curr_pos =
                        alg_llist_first ( &edit->activex[j], &k );
                struct edit_activex *curr_ax = *curr_pos;
                while ( curr_ax != nullptr ) {
                        if ( curr_ax->state ) {
                                curr_ax->ops->update ( curr_ax );
                        }
                        alg_llist_next ( &edit->activex[j], &k );
                }
        }
}

__dlexport void editor_update_all ( void )
{
        struct alg_llist *editors = &g_edit_cont.editor;
        int i;
        struct editor *curr_ed = alg_llist_first ( editors, &i );
        while ( curr_ed != nullptr ) {
                editor_update ( curr_ed );
                curr_ed = alg_llist_next ( editors, &i );
        }
}

void edit_activex_init ( enum EDIT_ACTIVEX_IDR type, struct edit_activex *activex )
{
        activex->type = type;
}

static void edit_activex_add (
        struct edit_activex *activex, char *name, struct editor *edit )
{
        activex->ops = cast(activex->ops) &c_ax_ops[activex->type];
        activex->name = alloc_fix ( 1, strlen(name) + 1 );
        activex->edit_id = edit->id;
        strcpy ( activex->name, name );
        edit->n_activex[activex->type] ++;
        alg_llist_add ( &activex, &edit->activex[activex->type] );
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

__dlexport void editor_add_activex ( char *name, void *_activex, struct editor *editor )
{
        struct editor *curr_ed = editor;
        struct edit_activex *activex = _activex;
        edit_activex_add ( activex, name, curr_ed );
}

#define cmp_activex( _info, _elm )      (!strcmp ( *(_info), ((struct edit_activex *) *(_elm))->name ))
__dlexport void editor_remove_activex (
        enum EDIT_ACTIVEX_IDR type, char *name, struct editor *editor )
{
        struct editor *curr_ed = editor;
        struct alg_llist *activex = &curr_ed->activex[type];
        struct edit_activex **found_pos = nullptr;
        alg_llist_remove ( activex, &name, &found_pos, cmp_activex );
        struct edit_activex *found_ax = *found_pos;

        if ( found_ax != nullptr ) {
                free_activex ( found_ax );
        } else {
                log_mild_err_dbg ( "couldn't find such activex of the type: %d with its name: %s", type, name );
                return ;
        }
}

__dlexport struct edit_activex *editor_find_activex (
        enum EDIT_ACTIVEX_IDR type, char *name, struct editor *editor )
{
        struct editor *curr_ed = editor;
        struct alg_llist *activex = &curr_ed->activex[type];
        struct edit_activex **found_pos = nullptr;
        alg_llist_find ( activex, &name, &found_pos, cmp_activex );
        struct edit_activex *found_ax = *found_pos;

        if ( found_ax == nullptr ) {
                log_mild_err_dbg ( "couldn't find such activex of the type: %d with its name: %s", type, name );
        }
        return found_ax;
}
#undef cmp_activex

__dlexport void editor_reg_custum_loop ( f_Editor_Loop func, void *info_ptr )
{
        g_loop_func = func;
        g_info_ptr = info_ptr;
}

__dlexport void editor_run ( bool use_custom )
{
        if ( use_custom ) {
                g_main_loop_task = thr_run_task (
                                           (f_Thread_Handler) g_loop_func, g_info_ptr, nullptr );
        } else {
                g_main_loop_task = thr_run_task (
                                           (f_Thread_Handler) g_edit_ops.editor_loop,
                                           nullptr, nullptr );
        }
}

__dlexport void editor_dispatch_signal ( struct editor *editor )
{
}

__dlexport void editor_load_state ( char *filename, struct editor *editor )
{
}

__dlexport void editor_save_state ( char *filename, struct editor *editor )
{
}
