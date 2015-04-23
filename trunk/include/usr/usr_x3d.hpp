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
#include <codecs/serializer.h>
#include <math/math.h>
#include <x3d/init.h>
#include <x3d/projectionprobe.h>
#include <x3d/renderer.h>
#include <x3d/renderable.h>
#include <x3d/renderableaggregate.h>
}

namespace usr
{

template<class T> class CoreResource {
public:
        virtual T*      get_core_resource() {return nullptr;}
        virtual void    set_core_resource(T* t) {}
};

}

}


#endif // USR_X3D_HPP_INCLUDED
