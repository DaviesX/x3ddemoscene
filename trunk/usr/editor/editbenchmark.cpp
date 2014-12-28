#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>
#include <usr/usr_kernel.hpp>

using namespace x3d;
using namespace x3d::usr;

class BenchmarkActiveX::BenchmarkInt
{
public:
        string          m_benchmark[2];
};

BenchmarkActiveX::BenchmarkActiveX ( string name ) :
        EditorActiveX ( name, sizeof(BenchmarkActiveX), EDIT_ACTIVEX_BENCHMARK )
{
        this->pimpl = new BenchmarkInt;
        for ( int i = 0; i < 2; i ++ ) {
                pimpl->m_benchmark[i].clear ();
        }
}

BenchmarkActiveX::~BenchmarkActiveX ( void )
{
        delete pimpl;
}

void BenchmarkActiveX::run_benchmark ( enum BenchmarkData type )
{
        switch ( type ) {
        case Benchmark_CornellBox:
                {
                pimpl->m_benchmark[on_front_buf()] = "cornell-box";
                break;
                }
        }
}

void BenchmarkActiveX::run_benchmark ( string& filename )
{
        pimpl->m_benchmark[on_front_buf()] = filename;
}

void BenchmarkActiveX::bind_callback ( string signal, f_Generic callback, void *data )
{
}

void BenchmarkActiveX::on_adding ( void )
{
}

#include "./cornellbox/cornellbox.h"

static void construct_cornellbox ( struct x3d::rda_context* ctx )
{
        static bool             has_constructed = false;
        static struct vertex*   vertices = nullptr;
        static int              n_vertices = 0;
        static int*             indices = nullptr;
        static int              n_indices = 0;

        static x3d::rda_instance* instance = nullptr;

        if ( !has_constructed ) {
                has_constructed = false;

                cornell_box_make ( &vertices, &n_vertices, &indices, &n_indices );

                x3d::point3d*  vert_arr = static_cast<x3d::point3d*>(alloc_fix ( sizeof(*vert_arr), n_vertices ));
                x3d::vector3d* norm_arr = static_cast<x3d::vector3d*>(alloc_fix ( sizeof(*norm_arr), n_vertices ));
                for ( int i = 0; i < n_vertices; i ++ ) {
                        x3d::set_point3d ( vertices[i].position[0],
                                           vertices[i].position[1],
                                           vertices[i].position[2],
                                           &vert_arr[i] );
                        x3d::set_vector3d ( vertices[i].normal[0],
                                            vertices[i].normal[1],
                                            vertices[i].normal[2],
                                            &norm_arr[i] );
                }
                x3d::rda_geometry* geo = x3d::rda_geometry_create ( const_cast<char*>("cornellbox"), 1.0f, false, 0 );
                x3d::rda_geometry_init_from_data ( geo, vert_arr, n_vertices, indices, n_indices,
                                                   norm_arr, nullptr, nullptr,
                                                   const_cast<x3d::matrix4x4*>(&x3d::IdentityMatrix4x4) );
                instance = x3d::rda_instance_create (
                                        (x3d::renderable*) geo,
                                        const_cast<x3d::matrix4x4*>(&x3d::IdentityMatrix4x4) );
                x3d::rda_context_add_instance2 ( ctx, instance );
        } else {
                x3d::rda_context_add_instance2 ( ctx, instance );
        }
}

void BenchmarkActiveX::update ( void )
{
        string benchmarkname;

        wait_for_update ();
        swap_buf ();
        benchmarkname = pimpl->m_benchmark[on_back_buf()];
        unwait ();

        KernelEnvironment* env = this->get_state_buffer ();
/* @fixme (davis#1#): shouldn't be <rda_context> here, should be <world> */
        struct x3d::rda_context* rda = static_cast<struct x3d::rda_context*>(env->use ( c_World ));

        if ( benchmarkname.empty () ) {
                // do nothing
        } else if ( benchmarkname == "cornell-box" ) {
                construct_cornellbox ( rda );
        }
}

void BenchmarkActiveX::dispatch ( void )
{
}

void BenchmarkActiveX::load ( struct serializer *s )
{
}

void BenchmarkActiveX::save ( struct serializer *s )
{
}
