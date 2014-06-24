#ifndef USR_EDITOR_HPP_INCLUDED
#define USR_EDITOR_HPP_INCLUDED

#include <usr/usr_x3d.hpp>
#include <usr/usr_kernel.hpp>

using namespace std;

namespace x3d
{

namespace usr
{

enum GUI_FONTEND_IDR {
        GUI_FONTEND_NONE,
        GUI_FONTEND_PURE_SDL,
        GUI_FONTEND_GTK,
        GUI_FONTEND_WIN32,
        GUI_FONTEND_QT,
        GUI_FONTEND_CUSTUM
};

enum EDIT_ACTIVEX_IDR {
        EDIT_ACTIVEX_RENDER_REGION,
        EDIT_ACTIVEX_RENDER_CONFIG,
        EDIT_ACTIVEX_ENTITY_LIST,
        EDIT_ACTIVEX_FILE_LOADER
};

const int c_NumActiveXType = 5;

/* state constants */
const string c_Renderer         = "renderer";
const string c_RedoStack        = "redo-stack";
const string c_Clipboard        = "clipboard";
const string c_WndHandle        = "window-handle";

class Editor;
class EditorFrontend;
class EditorActiveX;
class KernelEditor;


class __dlexport Editor
{
public:
        Editor ( void );
        ~Editor ( void );

        void update ( void );
        void dispatch_signal ( void );
        bool load_state ( char *filename );
        bool save_state ( char *filename );

        void add_activex ( EditorActiveX *activex );
        bool remove_activex ( EDIT_ACTIVEX_IDR type, char *name );
        EditorActiveX *find_activex ( EDIT_ACTIVEX_IDR type, char *name );

        KernelEnvironment *get_global_state ( void )
        {
                return &this->m_global_state;
        }

        uuid_t get_id ( void )
        {
                return m_id;
        }
private:
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
        EditorFrontend*          m_frontend;
        struct thr_task*        m_loop_task;
};

class RenderRegionActiveX;
class AssetListActiveX;
class EntityPropertyActiveX;
class RenderablePropertyActiveX;
class DynamicsPropertyActiveX;
class ViewControlActiveX;
class SelectorActiveX;
class RenderConfigActiveX;


class __dlexport EditorActiveX
{
public:
        EditorActiveX ( char *name, int size, EDIT_ACTIVEX_IDR type ) :
                m_size (size), m_type (type)
        {
                m_name = new char[strlen (name) + 1];
                strcpy ( m_name, name );
        };

        virtual ~EditorActiveX ()
        {
                delete m_name;
        }

        void notify_add ( Editor *e )
        {
                this->m_state   = e->get_global_state ();
                this->m_edit_id = e->get_id ();
        }

        const char *get_name ( void )
        {
                return m_name;
        }

        EDIT_ACTIVEX_IDR get_type ( void )
        {
                return m_type;
        }

        KernelEnvironment *get_state_buffer ( void )
        {
                return m_state;
        }

        virtual void update ( void ) {}
        virtual void dispatch ( void ) {}
        virtual void load ( struct serializer *s ) {}
        virtual void save ( struct serializer *s ) {}
private:
        int                     m_size;
        char*                   m_name;
        KernelEnvironment*      m_state;
        uuid_t                  m_edit_id;
        EDIT_ACTIVEX_IDR        m_type;
};


/* activex - render region */
typedef void (*f_Notify_idle) ( bool is_idle, void *handle, void *data );
typedef void (*f_Notify_resize) ( int width, int height, bool is_fullscreen,
                                  void *handle, void *data );

class __dlexport RenderRegionActiveX : public EditorActiveX
{
public:
        RenderRegionActiveX ( char *name, void *handle,
                              int x, int y, int w, int h );
        ~RenderRegionActiveX ();

        void update ( void );
        void dispatch ( void );
        void load ( struct serializer *s );
        void save ( struct serializer *s );

        void set_renderer ( struct renderer *renderer );
        void set_idle_state ( bool is_idle );
        void resize ( int x, int y, int w, int h, bool toggle_fullscreen );

        void bind_callback ( char *signal, f_Generic callback, void *data );
private:
        class RenderRegionInt;
        class RenderRegionInt *pimpl;
};


/* activex - render configurator */
typedef void (*f_Notify_Error) ( string message, RenderConfigActiveX *conf, void *data );

class __dlexport RenderConfigActiveX : public EditorActiveX
{
public:
        RenderConfigActiveX ( char *name );
        ~RenderConfigActiveX ( void );

        void update ( void );
        void dispatch ( void );
        void load ( struct serializer *s );
        void save ( struct serializer *s );

        void set_config_tab ( string tab_name );
        void set_config_value ( string value, void *data );
        bool apply ( void );
        void cancel ( void );
        void* checkout_value ( string tab_name, string value );
        void bind_callback ( char *signal, f_Generic callback, void *data );
private:
        class RenderConfigInt;
        class RenderConfigInt *pimpl;
};

}

}

#endif // USR_EDITOR_HPP_INCLUDED