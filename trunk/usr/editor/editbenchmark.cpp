#include <usr/usr_editor.hpp>
#include <usr/usr_kernel.hpp>

using namespace x3d::usr;

static void construct_cornellbox ( KernelEnvironment* env )
{
}

void BenchmarkActiveX::run_benchmark ( enum BenchmarkData type )
{
        KernelEnvironment* env = this->get_state_buffer ();

        switch ( type ) {
        case Benchmark_CornellBox:
                {
                construct_cornellbox ( env );
                break;
                }
        }
}

void BenchmarkActiveX::run_benchmark ( string& filename )
{
}

void BenchmarkActiveX::bind_callback ( string signal, f_Generic callback, void *data )
{
}

void BenchmarkActiveX::on_adding ( void )
{
}

void BenchmarkActiveX::update ( void )
{
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
