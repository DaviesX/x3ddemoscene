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
#include <logout.h>
#include <algorithm.h>
#include <allocator.h>
#include <thread.h>
#include <serializer.h>
#include <math/math.h>
#include <x3d/common.h>
#include <x3d/init.h>
#include <x3d/projectionprobe.h>
#include <x3d/renderer.h>
#include <x3d/renderable.h>
#include <x3d/renderableaggregate.h>
}

namespace usr
{

class CoreResource {
public:
        void* get_core_resource() {return nullptr;}
};

}

}


#endif // USR_X3D_HPP_INCLUDED
