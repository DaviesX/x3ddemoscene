#ifndef USR_KERNEL_HPP_INCLUDED
#define USR_KERNEL_HPP_INCLUDED

#include <usr/usr_x3d.hpp>

namespace x3d
{
namespace usr
{

using namespace std;

class KernelProxy;
class Kernel;
class KernelEnvironment;


class KernelEnvironment
{
public:
        template <typename t_Var> t_Var* make ( t_Var val )
        {
                t_Var* x = new t_Var;
                *x = val;
                return x;
        }

        bool declare ( string name, void* ptr )
        {
                struct Data data;
                data.name = name;
                data.ptr  = ptr;

                /* check if the variable has been declared */
                std::set<Data>::iterator it;
                it = m_data_set.find ( data );
                if ( it == m_data_set.end () ) {
                        /* the value hasn't been declared yet */
                        m_data_set.insert ( data );
                } else {
                        /* the value has been declared */
                        Data declared = *it;
                        declared.ptr = data.ptr;
                }
                return true;
        }

        void undeclare ( string name )
        {
                struct Data data;
                data.name = name;
                m_data_set.erase ( data );
        }

        void* use ( string name )
        {
                struct Data data;
                data.name = name;
                std::set<Data>::iterator it = m_data_set.find ( data );
                if ( it != m_data_set.end () ) {
                        Data ret = *it;
                        return ret.ptr;
                } else
                        return nullptr;
        }

private:
        struct Data {
                string  name;
                void*   ptr;
        };
        struct Comparer : public std::binary_function<Data, Data, bool> {
                bool operator() ( Data x, Data y ) const
                {
                        return x.name < y.name;
                }
        };
        set<Data, Comparer>       m_data_set;
};


class KernelProxy
{
public:
        KernelProxy ( string name ) :
                m_name (name)
        {
        }
        virtual ~KernelProxy () {}

        void notify_add ( Kernel* kernel )
        {
                this->m_kernel = kernel;
        }
        void notify_remove ( void )
        {
                this->m_kernel = nullptr;
        }
        Kernel* get_subjected_kernel ( void )
        {
                return this->m_kernel;
        }

        virtual int on_init ( int argc, char **argv, KernelEnvironment *env )
        {
                return 0;
        }
        virtual int on_rest_init ( KernelEnvironment *env )
        {
                return 0;
        }
        virtual int on_loop_init ( KernelEnvironment *env )
        {
                return 0;
        }
        virtual int on_loop ( KernelEnvironment *env )
        {
                return 0;
        }
        virtual int on_loop_free ( KernelEnvironment *env )
        {
                return 0;
        }
        virtual int on_free ( KernelEnvironment *env )
        {
                return 0;
        }
public:
        string                  m_name;
        KernelEnvironment       m_env;
        Kernel*                 m_kernel;
};

class __dlexport Kernel
{
public:
        Kernel ( void );
        ~Kernel ( void );
        bool run ( void );
        void stop ( void );
        bool register_proxy ( KernelProxy *proxy );
        bool unregister_proxy ( string m_name );
public:
        list<KernelProxy*>      m_proxy;
        bool                    is_running;
};

}
}

#endif // USR_KERNEL_HPP_INCLUDED
