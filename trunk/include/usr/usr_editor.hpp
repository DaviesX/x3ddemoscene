#ifndef USR_EDITOR_HPP_INCLUDED
#define USR_EDITOR_HPP_INCLUDED

#include <usr/usr_x3d.hpp>
#include <usr/usr_kernel.hpp>
#include <usr/usr_renderer.hpp>
#include <usr/usr_projectionprobe.hpp>
#include <usr/usr_world.hpp>

using namespace std;

namespace x3d
{

namespace usr
{

/* state constants */
const string c_RenderRegion     = "render-region";
const string c_RenderConfig     = "render-config";
const string c_Selector         = "selector";
const string c_Clipboard        = "clipboard";
const string c_WorldData        = "world-data";

class Editor;
class EditorFrontend;
class EditorActiveX;
class EditorBackend;

class RenderRegionActiveX;
class RenderConfigActiveX;

class AssetListActiveX;

class EntityPropertyActiveX;
class RenderablePropertyActiveX;
class DynamicsPropertyActiveX;
class LogicalPropertyActiveX;

class SelectorActiveX;

class WorldDataActiveX;
class BenchmarkActiveX;


class EditorActiveX
{
        friend class RenderRegionActiveX;
        friend class RenderConfigActiveX;
        friend class WorldDataActiveX;
        friend class BenchmarkActiveX;
public:
        enum EDIT_ACTIVEX_IDR {
                EDIT_ACTIVEX_ENTITY_LIST,
                EDIT_ACTIVEX_FILE_LOADER,
                EDIT_ACTIVEX_BENCHMARK,
                EDIT_ACTIVEX_RENDER_CONFIG,
                EDIT_ACTIVEX_WORLD_DATA,
                EDIT_ACTIVEX_RENDER_REGION,
                EDIT_ACTIVEX_NUMBER
        };

        static const int c_NumActiveXType = EDIT_ACTIVEX_NUMBER;

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
typedef void (*f_Notify_Idle)   (bool is_idle, EditorActiveX* ax, void *data);
typedef void (*f_Notify_Error)  (string message, EditorActiveX *ax, void *data);

/* activex - render region - manage the physical screen output */
class __dlexport RenderRegionActiveX : public EditorActiveX
{
public:
        enum ViewMode {
                PerspectiveMode,
                OrthogonalMode,
                QuadOrthogonalViewMode
        };

        RenderRegionActiveX(string name, OutputMethod method, void* handle, int x, int y, int w, int h);
        ~RenderRegionActiveX();

        void                    on_adding();
        void                    update();
        void                    dispatch();
        void                    load(struct serializer* s);
        void                    save(struct serializer* s);

        void                    resize(int x, int y, int w, int h, bool toggle_fullscreen);
        void                    bind_callback(string signal, f_Generic callback, void* data);

        ProjectionProbe*        get_probe() const;
        void                    moveto(int x, int y);
        void                    magnify(int delta);
        void                    set_view_mode(ViewMode mode);
private:
        void                    set_idle_state(bool is_idle);

        class RenderRegionInt;
        class RenderRegionInt*  pimpl;
};

/* activex - render configurator - configurates the renderer */
class RenderConfigActiveX : public EditorActiveX
{
public:
        RenderConfigActiveX(string name);
        ~RenderConfigActiveX();

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

        Renderer* get_renderer();
private:
        class RenderConfigInt;
        class RenderConfigInt *pimpl;
};

/* activex - world data */
class __dlexport WorldDataActiveX : public EditorActiveX
{
public:
        WorldDataActiveX ( string name );
        ~WorldDataActiveX ( void );

        void on_adding ( void );
        void update ( void );
        void dispatch ( void );
        void load ( struct serializer *s );
        void save ( struct serializer *s );

        World& get_world();
private:
        class WorldDataInt;
        class WorldDataInt*     pimpl;
};


/* activex - benchmark scene launcher */
class __dlexport BenchmarkActiveX : public EditorActiveX
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
        list<EditorActiveX*>    m_activex[EditorActiveX::c_NumActiveXType];
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

class __dlexport EditorBackend : public KernelProxy
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

        EditorBackend ( Editor *e );
        ~EditorBackend ( void );

        void register_gui_frontend ( GUI_FONTEND_IDR type, EditorFrontend *backend );
private:
        int on_init ( int argc, char **argv, KernelEnvironment *env );
        int on_rest_init ( KernelEnvironment *env );
        int on_loop_init ( KernelEnvironment *env );
        int on_loop ( KernelEnvironment *env );
        int on_loop_free ( KernelEnvironment *env );
        int on_free ( KernelEnvironment *env );

        GUI_FONTEND_IDR         m_frontend_type;
        Editor*                 m_editor;
        EditorFrontend*         m_frontend;
        struct thr_task*        m_loop_task;
        const string            m_state_file = "./etc/editor/editor_state";
};


} // namespace usr

} // namespace x3d


#endif // USR_EDITOR_HPP_INCLUDED
