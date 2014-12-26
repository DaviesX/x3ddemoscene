#ifndef USR_EDITOR_HPP_INCLUDED
#define USR_EDITOR_HPP_INCLUDED

#include <usr/usr_x3d.hpp>
#include <usr/usr_kernel.hpp>

using namespace std;

namespace x3d
{

namespace usr
{

const int c_NumActiveXType = 5;

/* state constants */
const string c_Renderer         = "renderer";
const string c_RedoStack        = "redo-stack";
const string c_Clipboard        = "clipboard";
const string c_WndHandle        = "window-handle";
const string c_World            = "world";
const string c_ViewControl      = "view-control";

class Editor;
class EditorFrontend;
class EditorActiveX;
class KernelEditor;

class RenderRegionActiveX;
class RenderConfigActiveX;

class AssetListActiveX;

class EntityPropertyActiveX;
class RenderablePropertyActiveX;
class DynamicsPropertyActiveX;
class LogicalPropertyActiveX;

class ViewControlActiveX;
class SelectorActiveX;

class WorldDataActiveX;
class BenchmarkActiveX;


class __dlexport EditorActiveX
{
        friend class RenderRegionActiveX;
        friend class RenderConfigActiveX;
        friend class WorldDataActiveX;
        friend class BenchmarkActiveX;
public:
        enum EDIT_ACTIVEX_IDR {
                EDIT_ACTIVEX_RENDER_REGION,
                EDIT_ACTIVEX_RENDER_CONFIG,
                EDIT_ACTIVEX_ENTITY_LIST,
                EDIT_ACTIVEX_FILE_LOADER,
                EDIT_ACTIVEX_BENCHMARK
        };

        EditorActiveX ( string name, int size, EDIT_ACTIVEX_IDR type );
        virtual ~EditorActiveX ();

        void                    notify_add ( Editor *e );
        string                  get_name ( void ) const;
        EDIT_ACTIVEX_IDR        get_type ( void ) const;
        KernelEnvironment*      get_state_buffer ( void ) const;
        bool                    is_dirty ( void ) const;

        virtual void on_adding ( void ) = 0;
        virtual void update ( void ) = 0;
        virtual void dispatch ( void ) = 0;
        virtual void load ( struct serializer *s ) = 0;
        virtual void save ( struct serializer *s ) = 0;
private:
        int                     m_size;
        string                  m_name;
        KernelEnvironment*      m_state;
        uuid_t                  m_edit_id;
        EDIT_ACTIVEX_IDR        m_type;

        /* front-back buffer utilities */
        int                     m_bufcount;
        struct x3d::thr_trap    m_block_driver;

        int     on_front_buf ( void ) const;
        int     on_back_buf  ( void ) const;
        void    swap_buf ( void );
        void    wait_for_update ( void );
        void    unwait ( void );

        /* dirt-mark utilities */
        bool                    m_dirty;

        void    mark_dirty ( void );
        void    unmark_dirty ( void );
};

/* unified callbacks */
typedef void (*f_Notify_idle) ( bool is_idle, EditorActiveX* ax, void *data );
typedef void (*f_Notify_resize) ( bool is_fullscreen, int width, int height, EditorActiveX* ax, void *data );
typedef void (*f_Notify_Error) ( string message, EditorActiveX *ax, void *data );

/* activex - render region */
class __dlexport RenderRegionActiveX : public EditorActiveX
{
public:
        RenderRegionActiveX ( string name, void *handle,
                              int x, int y, int w, int h );
        ~RenderRegionActiveX ();

        void on_adding ( void );
        void update ( void );
        void dispatch ( void );
        void load ( struct serializer *s );
        void save ( struct serializer *s );

        void set_renderer ( struct renderer *renderer );
        void set_idle_state ( bool is_idle );
        void resize ( int x, int y, int w, int h, bool toggle_fullscreen );

        void bind_callback ( string signal, f_Generic callback, void *data );

        const void* get_handle ( void ) const;
private:
        class RenderRegionInt;
        class RenderRegionInt *pimpl;
};

/* activex - render configurator */
class __dlexport RenderConfigActiveX : public EditorActiveX
{
public:
        RenderConfigActiveX ( string name );
        ~RenderConfigActiveX ( void );

        void on_adding ( void );
        void update ( void );
        void dispatch ( void );
        void load ( struct serializer *s );
        void save ( struct serializer *s );

        void set_config_tab ( string tab_name );
        void set_config_value ( string value, void *data );
        bool apply ( void );
        void cancel ( void );
        void* checkout_value ( string tab_name, string value );
        void bind_callback ( string signal, f_Generic callback, void *data );
private:
        class RenderConfigInt;
        class RenderConfigInt *pimpl;
};

class WorldDataActiveX : public EditorActiveX
{
        friend class EditorActiveX;
public:
        void on_adding ( void ) {}
        void update ( void ) {}
        void dispatch ( void ) {}
        void load ( struct serializer *s ) {}
        void save ( struct serializer *s ) {}
};


/* activex - benchmark scene launcher */
class BenchmarkActiveX : public EditorActiveX
{
public:
        enum BenchmarkData {
                Benchmark_CornellBox
        };

        BenchmarkActiveX ( string name );
        ~BenchmarkActiveX ( void );

        void on_adding ( void );
        void update ( void );
        void dispatch ( void );
        void load ( struct serializer *s );
        void save ( struct serializer *s );

        void run_benchmark ( enum BenchmarkData type );
        void run_benchmark ( string& filename );

        void bind_callback ( string signal, f_Generic callback, void *data );
private:
        class BenchmarkInt;
        class BenchmarkInt* pimpl;
};


class __dlexport Editor
{
public:
        Editor ( void );
        ~Editor ( void );

        void open ( void )
        {
                m_is_open = true;
        }
        void close ( void )
        {
                m_is_open = false;
        }
        bool is_open ( void )
        {
                return m_is_open;
        }

        void update ( void );
        void dispatch_signal ( void );
        bool load_state ( string filename );
        bool save_state ( string filename );

        void add_activex ( EditorActiveX *activex );
        bool remove_activex ( EditorActiveX::EDIT_ACTIVEX_IDR type, string name );
        EditorActiveX *find_activex ( EditorActiveX::EDIT_ACTIVEX_IDR type, string name );

        KernelEnvironment *get_global_state ( void )
        {
                return &this->m_global_state;
        }

        uuid_t get_id ( void )
        {
                return m_id;
        }
private:
        bool                    m_is_open;
        uuid_t                  m_id;
        list<EditorActiveX*>    m_activex[c_NumActiveXType];
        KernelEnvironment       m_global_state;
};

class EditorFrontend
{
public:
        virtual ~EditorFrontend () {}

        bool is_custum ( void )
        {
                return false;
        }

        virtual bool init ( int argc, char **argv,
                            Editor *editor, KernelEnvironment *env )
        {
                return true;
        }
        virtual bool end_init ( Editor *editor, KernelEnvironment *env )
        {
                return true;
        }
        virtual bool load ( Editor *editor, KernelEnvironment *env )
        {
                return true;
        }
        virtual void loop ( Editor *editor, KernelEnvironment *env )
        {
                return ;
        }
        virtual bool free ( Editor *editor, KernelEnvironment *env )
        {
                return true;
        }
};

class __dlexport KernelEditor : public KernelProxy
{
public:
        enum GUI_FONTEND_IDR {
                GUI_FONTEND_NONE,
                GUI_FONTEND_PURE_SDL,
                GUI_FONTEND_GTK,
                GUI_FONTEND_WIN32,
                GUI_FONTEND_QT,
                GUI_FONTEND_CUSTUM
        };

        KernelEditor ( Editor *e );
        ~KernelEditor ( void );

        int on_init ( int argc, char **argv, KernelEnvironment *env );
        int on_rest_init ( KernelEnvironment *env );
        int on_loop_init ( KernelEnvironment *env );
        int on_loop ( KernelEnvironment *env );
        int on_loop_free ( KernelEnvironment *env );
        int on_free ( KernelEnvironment *env );

        void register_gui_frontend ( GUI_FONTEND_IDR type, EditorFrontend *backend );
private:
        GUI_FONTEND_IDR         m_frontend_type;
        Editor*                 m_editor;
        EditorFrontend*         m_frontend;
        struct thr_task*        m_loop_task;
        const string            m_state_file = "./etc/editor/editor_state";
};


} // namespace usr

} // namespace x3d

#endif // USR_EDITOR_HPP_INCLUDED
