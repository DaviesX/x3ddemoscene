#include <usr/usr_x3d.hpp>
#include <usr/usr_editor.hpp>
#include <usr/usr_kernel.hpp>

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

static void construct_cornellbox ( struct x3d::rda_context* rda )
{
}

void BenchmarkActiveX::update ( void )
{
        wait_for_update ();
        swap_buf ();
        KernelEnvironment* env = this->get_state_buffer ();
        struct x3d::rda_context* rda = static_cast<struct x3d::rda_context*>(env->use ( c_World ));

        if ( pimpl->m_benchmark[on_back_buf()] == "cornell-box" ) {
                construct_cornellbox ( rda );
        }
        unwait ();
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
