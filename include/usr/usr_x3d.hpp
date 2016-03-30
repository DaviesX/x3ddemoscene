#ifndef USR_X3D_HPP_INCLUDED
#define USR_X3D_HPP_INCLUDED


#include <string>
#include <algorithm>
#include <set>
#include <list>
#include <vector>

using namespace std;

namespace x3d
{

extern "C"
{
#include <misc.h>
#include <x3d/common.h>
#include <system/log.h>
#include <system/allocator.h>
#include <system/thread.h>
#include <system/panic.h>
#include <codecs/serializer.h>
#include <math/math.h>
#include <x3d/init.h>
#include <x3d/display.h>
#include <x3d/projectionprobe.h>
#include <x3d/renderer.h>
#include <x3d/renderable.h>
#include <x3d/renderableaggregate.h>
#include <x3d/rendernodeoutput.h>
#include <x3d/rendernodelayer.h>
#include <x3d/rendernodehdr.h>
#include <x3d/rendernoderadiance.h>
#include <x3d/rendernoderdaloader.h>
#include <x3d/renderenvconsts.h>
}

namespace usr
{

template<class T> class CoreResource {
public:
        virtual T*  get_core_resource()
        {
                log_critical_err_dbg("pending for core resources access, but accessor does not exists");
                return nullptr;
        }
        virtual void set_core_resource(T* t)
        {
                log_critical_err_dbg("pending for core resources mutation, but mutator does not exists");
        }
};

}

}


#endif // USR_X3D_HPP_INCLUDED
