#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>
#include <usr/usr_kernel.hpp>
#include <usr/usr_renderable.hpp>


namespace x3d
{
namespace usr
{

class BenchmarkActiveX::BenchmarkInt
{
public:
        string          m_benchmark[2];
};

BenchmarkActiveX::BenchmarkActiveX ( string name ) :
        EditorBackendActiveX ( name, sizeof(BenchmarkActiveX), EditActiveXBenchmark )
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
        case Benchmark_CornellBox: {
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

static void construct_cornellbox ( RenderAggregate* aggregate )
{
        static bool             has_constructed = false;
        static struct vertex*   vertices = nullptr;
        static int              n_vertices = 0;
        static int*             indices = nullptr;
        static int              n_indices = 0;

        static RenderableInstance* instance;

        if ( !has_constructed ) {
                has_constructed = true;

                cornell_box_make ( &vertices, &n_vertices, &indices, &n_indices );

                point3d*  vert_arr = static_cast<point3d*>(alloc_fix ( sizeof(*vert_arr), n_vertices ));
                vector3d* norm_arr = static_cast<vector3d*>(alloc_fix ( sizeof(*norm_arr), n_vertices ));
                for ( int i = 0; i < n_vertices; i ++ ) {
                        set_point3d ( vertices[i].position[0],
                                      vertices[i].position[1],
                                      vertices[i].position[2],
                                      &vert_arr[i] );
                        set_vector3d ( vertices[i].normal[0],
                                       vertices[i].normal[1],
                                       vertices[i].normal[2],
                                       &norm_arr[i] );
                }
                RenderableFactory fact;
                GeometryRenderable* geometry = (GeometryRenderable*) fact.create(RenderableFactory::Geometry, 
										 "cornellbox", false);
                geometry->set_importance(1.0f);
                geometry->set_material(0);
                geometry->init_from_data(vert_arr, n_vertices, indices, n_indices,
                                         norm_arr, nullptr, nullptr,
                                         nullptr);

                instance = geometry->make_instance(const_cast<matrix4x4*>(&IdentityMatrix4x4));
                aggregate->add_instance(instance);
                aggregate->add_renderable(geometry);
        } else {
                if (!aggregate->has_instance(instance)) {
                        aggregate->add_instance(instance);
                }
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
        WorldDataActiveX* worlddata = static_cast<WorldDataActiveX*>(env->use ( c_WorldData ));
        RenderAggregate* aggregate = worlddata->get_world().get_render_aggregate();

        if ( benchmarkname.empty () ) {
                // do nothing
        } else if ( benchmarkname == "cornell-box" ) {
                construct_cornellbox ( aggregate );
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

}// namespace usr

}// namesapce x3d
