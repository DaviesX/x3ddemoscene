#ifndef USR_KERNEL_HPP_INCLUDED
#define USR_KERNEL_HPP_INCLUDED

#include <usr/usr_x3d.hpp>

namespace x3d
{
namespace usr
{

using namespace std;

class KernelEnvironment
{
public:
        template <typename t_Var> bool declare ( string name, t_Var *ptr )
        {
                struct Data data;
                data.name = name;
                data.ptr = (void *) malloc ( sizeof(t_Var) );
                *(t_Var *) data.ptr = *ptr;

                /* check if the variable has been declared */
                std::set<Data>::iterator it;
                it = m_data_set.find ( data );
                if ( it == m_data_set.end () ) {
                        /* the value hasn't been declared yet */
                        m_data_set.insert ( data );
                } else {
                        /* the value has been declared */
                        Data declared = *it;
                        delete (t_Var*) declared.ptr;
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

        template <typename t_Var> t_Var *use ( string name )
        {
                struct Data data;
                data.name = name;
                std::set<Data>::iterator it = m_data_set.find ( data );
                if ( it != m_data_set.end () ) {
                        Data ret = *it;
                        return (t_Var *) ret.ptr;
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
};

class __dlexport Kernel
{
public:
        Kernel ( void );
        ~Kernel ( void );
        void run ( void );
        void stop ( void );
        bool register_proxy ( KernelProxy *proxy );
        bool unregister_proxy ( string m_name );
public:
        list<KernelProxy>       m_proxy;
        bool                    is_running;
};

}
}

#endif // USR_KERNEL_HPP_INCLUDED
