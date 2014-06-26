#include <usr/usr_kernel.hpp>

using namespace x3d;
using namespace x3d::usr;


static int on_init ( int argc, char **argv, void *env )
{
        Kernel* kern = (Kernel*) env;
        list<KernelProxy*>::iterator iter = kern->m_proxy.begin ();
        while ( iter != kern->m_proxy.end () ) {
                KernelProxy* curr_proxy = *iter;
                curr_proxy->on_init ( argc, argv, &curr_proxy->m_env );
                ++ iter;
        }
        return 0;
}

static int on_rest_init ( void *env )
{
        Kernel* kern = (Kernel*) env;
        list<KernelProxy*>::iterator iter = kern->m_proxy.begin ();
        while ( iter != kern->m_proxy.end () ) {
                KernelProxy* curr_proxy = *iter;
                curr_proxy->on_rest_init ( &curr_proxy->m_env );
                ++ iter;
        }
        return 0;
}

static int on_loop_init ( void *env )
{
        Kernel* kern = (Kernel*) env;
        list<KernelProxy*>::iterator iter = kern->m_proxy.begin ();
        while ( iter != kern->m_proxy.end () ) {
                KernelProxy* curr_proxy = *iter;
                curr_proxy->on_loop_init ( &curr_proxy->m_env );
                ++ iter;
        }
        return 0;
}

static int on_loop ( void *env )
{
        Kernel* kern = (Kernel*) env;
        list<KernelProxy*>::iterator iter = kern->m_proxy.begin ();
        while ( iter != kern->m_proxy.end () ) {
                KernelProxy* curr_proxy = *iter;
                curr_proxy->on_loop ( &curr_proxy->m_env );
                ++ iter;
        }
        return 0;
}

static int on_loop_free ( void *env )
{
        Kernel* kern = (Kernel*) env;
        list<KernelProxy*>::iterator iter = kern->m_proxy.begin ();
        while ( iter != kern->m_proxy.end () ) {
                KernelProxy* curr_proxy = *iter;
                curr_proxy->on_loop_free ( &curr_proxy->m_env );
                ++ iter;
        }
        return 0;
}

static int on_free ( void *env )
{
        Kernel* kern = (Kernel*) env;
        list<KernelProxy*>::iterator iter = kern->m_proxy.begin ();
        while ( iter != kern->m_proxy.end () ) {
                KernelProxy *curr_proxy = *iter;
                curr_proxy->on_free ( &curr_proxy->m_env );
                ++ iter;
        }
        return 0;
}

/* Kernel */
Kernel::Kernel ( void )
{
#ifdef X3D_DEBUG_MODE
        kernel_add_param ( (char*) "--so_path", (char*) "./libX3dCore_dbg.so" );
#else
        kernel_add_param ( (char*) "--so_path", (char*) "./libX3dCore.so" );
#endif
        kernel_reg_environment ( (void*) this );
        kernel_reg_signal ( (char*) "on-init", (f_Generic) on_init );
        kernel_reg_signal ( (char*) "on-rest-init", (f_Generic) on_rest_init );
        kernel_reg_signal ( (char*) "on-loop-init", (f_Generic) on_loop_init );
        kernel_reg_signal ( (char*) "on-loop", (f_Generic) on_loop );
        kernel_reg_signal ( (char*) "on-loop-free", (f_Generic) on_loop_free );
        kernel_reg_signal ( (char*) "on-free", (f_Generic) on_free );
        this->is_running = false;
}

Kernel::~Kernel ( void )
{
        kernel_shutdown ();
}

bool Kernel::run ( void )
{
        this->is_running = true;
        if ( !kernel_start () ) {
                this->is_running = false;
                return false;
        }
        kernel_loop ();
        this->is_running = false;
        return true;
}

void Kernel::stop ( void )
{
        kernel_halt ();
}

bool Kernel::register_proxy ( KernelProxy *proxy )
{
/* @todo (davis#1#): <usr_kernel> thread safe register  */
        if ( this->is_running )
                return false;
        else {
                proxy->notify_add ( this );
                this->m_proxy.push_back ( proxy );
                return true;
        }
}

bool Kernel::unregister_proxy ( string m_name )
{
/* @todo (davis#2#): <usr_kernel> thread safe unregister  */
        if ( this->is_running )
                return false;

        list<KernelProxy*>::iterator iter = this->m_proxy.begin ();
        while ( iter != this->m_proxy.end () ) {
                KernelProxy *curr_proxy = *iter;
                if ( curr_proxy->m_name == m_name ) {
                        curr_proxy->notify_remove ();
                        this->m_proxy.erase ( iter );
                        return true;
                }
                ++ iter;
        }
        return false;
}

/* KernelEnvironment */
