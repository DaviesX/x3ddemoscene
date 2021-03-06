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
const string c_RenderFrame      = "render-frame";
const string c_RenderConfig     = "render-config";
const string c_Selector         = "selector";
const string c_Clipboard        = "clipboard";
const string c_WorldData        = "world-data";
const string c_PowersaveSwitch  = "powersave-switch";

class EditorBackend;
class EditorFrontend;
class EditorBackendActiveX;
class KernelEditor;

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


class EditorBackendActiveX
{
        friend class RenderRegionActiveX;
        friend class RenderConfigActiveX;
        friend class RenderFrameActiveX;
        friend class WorldDataActiveX;
        friend class BenchmarkActiveX;
public:
        enum EditActiveXType {
                EditActiveXEntityEditor,
                EditActiveXBenchmark,
                EditActiveXRenderConfig,
                EditActiveXWorldData,
                EditActiveXRenderRegion,
                EditActiveXRenderFrame,
                EditActiveXPowersaveSwitch,
                c_EditActiveXNumber
        };

        static const int c_NumActiveXType = c_EditActiveXNumber;

        EditorBackendActiveX(string name, int size, EditActiveXType type);
        virtual ~EditorBackendActiveX ();

        void                    notify_add(EditorBackend *e);
        string                  get_name() const;
        EditActiveXType         get_type() const;
        KernelEnvironment*      get_state_buffer() const;
        bool                    is_dirty() const;

        bool                    operator==(const EditorBackendActiveX& rhs);
        bool                    operator!=(const EditorBackendActiveX& rhs);

        virtual void            on_adding() = 0;        // upon adding the activex to the editor
        virtual void            preupdate() {};   // any activesx-independent orderless action should be taken here
        virtual void            update() = 0;           // update for current loop
        virtual void            dispatch() = 0;         // inform editor backends of signals
        virtual void            load(struct serializer *s) = 0; // load status from the serial sequence
        virtual void            save(struct serializer *s) = 0; // save status to the serial sequence
private:
        int                     m_size;
        string                  m_name;
        KernelEnvironment*      m_state;
        uuid_t                  m_edit_id;
        EditActiveXType         m_type;

        /* front-back buffer utilities */
        int                     m_bufcount;
        struct x3d::thread_trap m_block_driver;

        int     on_front_buf() const;
        int     on_back_buf () const;
        void    swap_buf();
        void    wait_for_update();
        void    unwait();

        /* dirt-mark utilities */
        bool                    m_dirty;

        void    mark_dirty();
        void    unmark_dirty();
};

/* unified callbacks */
typedef void (*f_Notify_Idle)   (bool is_idle, EditorBackendActiveX* ax, void *data);
typedef void (*f_Notify_Error)  (string message, EditorBackendActiveX *ax, void *data);

/* activex - render region - manage the multi-frame real-time physical screen output */
class RenderRegionActiveX : public EditorBackendActiveX
{
public:
        enum ViewMode {
                PerspectiveMode,
                OrthogonalMode,
                QuadOrthogonalViewMode
        };

        RenderRegionActiveX(string name, void* handle, int x, int y, int w, int h);
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

        void                    request_idle_state(bool is_idle);
private:
        class RenderRegionInt;
        class RenderRegionInt*  pimpl;
};

/* activex - render frame - manage single frame offline/real-time output */
class RenderFrameActiveX : public EditorBackendActiveX
{
public:
        RenderFrameActiveX(string name);
        ~RenderFrameActiveX();

        typedef void (*f_Notify_Error)  (std::string message, RenderFrameActiveX& ax, void *data);
        typedef void (*f_Notify_Finish) (std::string stats, RenderFrameActiveX& ax, void* data);
        typedef void (*f_Notify_In_Progress) (std::string stats, RenderFrameActiveX& ax, void* data);

        void    on_adding();
        void    preupdate();
        void    update();
        void    dispatch();
        void    load(struct serializer* s);
        void    save(struct serializer* s);

        void    bind_callback(string signal, f_Generic callback, void *data);
        bool    run_frame_renderer(bool to_run);
private:
        class RenderFrameInt;
        class RenderFrameInt*        pimpl;
};

/* activex - render configurator - configurates the renderer */
class RenderConfigActiveX : public EditorBackendActiveX
{
public:
        RenderConfigActiveX(string name);
        ~RenderConfigActiveX();

        void on_adding();
        void update();
        void dispatch();
        void load(struct serializer *s);
        void save(struct serializer *s);

        void set_config_tab(string tab_name);
        void set_config_value(string value, void *data);
        bool apply();
        void cancel();
        void* checkout_value(string tab_name, string value);
        void bind_callback(string signal, f_Generic callback, void *data);

	enum RenderTreeConfigType {
		RenderTreeInteractive,
		RenderTreeStatic,
                RenderTreeBake,
                c_NumRenderTree
	};
        RenderTree* get_render_tree(RenderTreeConfigType type) const;
private:
        class RenderConfigInt;
        class RenderConfigInt *pimpl;
};

/* activex - world data */
class WorldDataActiveX : public EditorBackendActiveX
{
public:
        enum ActionType {
                ActionChangeDataSet,
                ActionAddObject,
                ActionRemoveObject,
        };

        enum ObjectType {
                ObjectDataSet,
                ObjectRenderable,
        };

        struct ActionInfo {
                ActionType      action_type;
                ObjectType      object_type;
                std::string     object_id;
                std::string     object_name;
        };

        typedef void (*f_Notify_Error)  (string message, WorldDataActiveX& ax, void *data);
        typedef void (*f_Notify_Action) (std::vector<ActionInfo>& info, WorldDataActiveX& ax, void* data);

        WorldDataActiveX(std::string name);
        WorldDataActiveX(std::string name, std::string data_set_name, std::string storage_path);
        ~WorldDataActiveX();

        void            on_adding();
        void            preupdate();
        void            update();
        void            dispatch();
        void            load(struct serializer *s);
        void            save(struct serializer *s);

        void            bind_callback(string signal, f_Generic callback, void *data);

        void            new_data_set(std::string data_set_name, std::string storage_path);
        void            open_data_set(std::string storage_path);
        void            close_data_set();

        std::string     get_data_set_name();
        std::string     get_storage_path();

        World&          get_world();
        bool            has_data_set();
private:
        class WorldDataInt;
        class WorldDataInt*     pimpl;
};


/* activex - benchmark scene launcher */
class __dlexport BenchmarkActiveX : public EditorBackendActiveX
{
public:
        enum BenchmarkData {
                Benchmark_CornellBox
        };

        BenchmarkActiveX(string name);
        ~BenchmarkActiveX();

        void on_adding();
        void update();
        void dispatch();
        void load(struct serializer *s);
        void save(struct serializer *s);

        void run_benchmark(enum BenchmarkData type);
        void run_benchmark(string& filename);

        void bind_callback(string signal, f_Generic callback, void *data);
private:
        class BenchmarkInt;
        class BenchmarkInt* pimpl;
};

/* activex - powersave switch */
class PowersaveSwitchActiveX : public EditorBackendActiveX
{
public:
        PowersaveSwitchActiveX(string name);
        ~PowersaveSwitchActiveX();

        void on_adding();
        void update();
        void dispatch();
        void load(struct serializer *s);
        void save(struct serializer *s);

        void switch_on_performance();
        void switch_on_powersave();
private:
        class PowersaveSwitchInt;
        class PowersaveSwitchInt* pimpl;
};

class __dlexport EditorBackend
{
public:
        EditorBackend();
        ~EditorBackend();

        void open()
        {
                m_is_open = true;
        }
        void close()
        {
                m_is_open = false;
        }
        bool is_open()
        {
                return m_is_open;
        }

        void update();
        void dispatch_signal();
        bool load_state(string filename);
        bool save_state(string filename);

        bool add_activex(EditorBackendActiveX *activex);
        bool remove_activex(EditorBackendActiveX::EditActiveXType type, string name);
        EditorBackendActiveX *find_activex(EditorBackendActiveX::EditActiveXType type, string name);

        KernelEnvironment *get_global_state()
        {
                return &this->m_global_state;
        }

        uuid_t get_id()
        {
                return m_id;
        }
private:
        bool                            m_is_open;
        uuid_t                          m_id;
        list<EditorBackendActiveX*>     m_activex[EditorBackendActiveX::c_NumActiveXType];
        KernelEnvironment               m_global_state;
};

class EditorFrontend
{
public:
        virtual ~EditorFrontend () {}

        bool is_custum()
        {
                return false;
        }

        virtual bool init(int argc, char **argv,
                            EditorBackend *editor, KernelEnvironment *env)
        {
                return true;
        }
        virtual bool end_init(EditorBackend *editor, KernelEnvironment *env)
        {
                return true;
        }
        virtual bool load(EditorBackend *editor, KernelEnvironment *env)
        {
                return true;
        }
        virtual void loop(EditorBackend *editor, KernelEnvironment *env)
        {
                return ;
        }
        virtual bool free(EditorBackend *editor, KernelEnvironment *env)
        {
                return true;
        }
};

class __dlexport KernelEditor : public KernelProxy
{
public:
        enum GUIFrontendType {
                GUIFrontendNone,
                GUIFrontendPureSDL,
                GUIFrontendGTK,
                GUIFrontendWin32,
                GUIFrontendQT,
                GUIFrontendCUSTOM
        };

        KernelEditor();
        ~KernelEditor();

        void register_editor_frontend(GUIFrontendType type, EditorFrontend *backend);
        void register_editor_backend(EditorBackend* backend);
        void close();
private:
        int on_init(int argc, char **argv, KernelEnvironment *env);
        int on_rest_init(KernelEnvironment *env);
        int on_loop_init(KernelEnvironment *env);
        int on_loop(KernelEnvironment *env);
        int on_loop_free(KernelEnvironment *env);
        int on_free(KernelEnvironment *env);

        GUIFrontendType         m_frontend_type;
        EditorBackend*          m_backend;
        EditorFrontend*         m_frontend;
        struct thread_task*     m_loop_task;
        const string            m_state_file = "./etc/editor/editor_state";
};


} // namespace usr

} // namespace x3d


#endif // USR_EDITOR_HPP_INCLUDED
