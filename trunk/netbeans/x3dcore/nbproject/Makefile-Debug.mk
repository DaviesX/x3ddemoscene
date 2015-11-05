#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/248117513/fbxmodel.o \
	${OBJECTDIR}/_ext/248117513/imageloader.o \
	${OBJECTDIR}/_ext/248117513/main.o \
	${OBJECTDIR}/_ext/248117513/modelloader.o \
	${OBJECTDIR}/_ext/248117513/objmodel.o \
	${OBJECTDIR}/_ext/248117513/pbrtmodel.o \
	${OBJECTDIR}/_ext/248117513/rendererconf.o \
	${OBJECTDIR}/_ext/248117513/x3dscene.o \
	${OBJECTDIR}/_ext/1186658598/bvh.o \
	${OBJECTDIR}/_ext/1186658598/cone_shape.o \
	${OBJECTDIR}/_ext/1186658598/cylinder_shape.o \
	${OBJECTDIR}/_ext/1186658598/disk_shape.o \
	${OBJECTDIR}/_ext/1186658598/geocache.o \
	${OBJECTDIR}/_ext/1186658598/geometry.o \
	${OBJECTDIR}/_ext/1186658598/grid.o \
	${OBJECTDIR}/_ext/1186658598/hyperboloid_shape.o \
	${OBJECTDIR}/_ext/1186658598/kdtree.o \
	${OBJECTDIR}/_ext/1186658598/main.o \
	${OBJECTDIR}/_ext/1186658598/octree.o \
	${OBJECTDIR}/_ext/1186658598/paraboloid_shape.o \
	${OBJECTDIR}/_ext/1186658598/primitive_parition.o \
	${OBJECTDIR}/_ext/1186658598/sphere_shape.o \
	${OBJECTDIR}/_ext/1186658598/trimesh_shape.o \
	${OBJECTDIR}/_ext/1186658598/trimesh_subdivide.o \
	${OBJECTDIR}/_ext/1118113710/light.o \
	${OBJECTDIR}/_ext/1561355151/material.o \
	${OBJECTDIR}/_ext/1698666324/volume.o \
	${OBJECTDIR}/_ext/1746317338/binary_out.o \
	${OBJECTDIR}/_ext/1746317338/image_out.o \
	${OBJECTDIR}/_ext/1746317338/out.o \
	${OBJECTDIR}/_ext/1746317338/screen_out.o \
	${OBJECTDIR}/_ext/1746317338/stream_out.o \
	${OBJECTDIR}/_ext/1122083400/fisheye.o \
	${OBJECTDIR}/_ext/1122083400/obj_probe.o \
	${OBJECTDIR}/_ext/1122083400/orthogonal.o \
	${OBJECTDIR}/_ext/1122083400/perspective.o \
	${OBJECTDIR}/_ext/1122083400/proj_probe.o \
	${OBJECTDIR}/_ext/1122083400/spatial_probe.o \
	${OBJECTDIR}/_ext/1122083400/spherical.o \
	${OBJECTDIR}/_ext/767442731/bufferedobject.o \
	${OBJECTDIR}/_ext/767442731/colorspectrum.o \
	${OBJECTDIR}/_ext/767442731/compositepipeline.o \
	${OBJECTDIR}/_ext/767442731/fbo.o \
	${OBJECTDIR}/_ext/767442731/main.o \
	${OBJECTDIR}/_ext/767442731/pmrenderer.o \
	${OBJECTDIR}/_ext/767442731/ptrenderer.o \
	${OBJECTDIR}/_ext/767442731/rasterization.o \
	${OBJECTDIR}/_ext/767442731/rasterizer.o \
	${OBJECTDIR}/_ext/767442731/rasterpass.o \
	${OBJECTDIR}/_ext/767442731/rasterpipeline.o \
	${OBJECTDIR}/_ext/767442731/raytracepipeline.o \
	${OBJECTDIR}/_ext/767442731/rendererinsmod.o \
	${OBJECTDIR}/_ext/767442731/rendertarget.o \
	${OBJECTDIR}/_ext/767442731/rtrenderer.o \
	${OBJECTDIR}/_ext/767442731/shaderformulate.o \
	${OBJECTDIR}/_ext/767442731/surface.o \
	${OBJECTDIR}/_ext/767442731/trianglebuffer.o \
	${OBJECTDIR}/_ext/767442731/vbo.o \
	${OBJECTDIR}/_ext/767442731/vertexprocessor.o \
	${OBJECTDIR}/_ext/501084237/basic_rt_shader.o \
	${OBJECTDIR}/_ext/501084237/main.o \
	${OBJECTDIR}/_ext/501084237/shader.o \
	${OBJECTDIR}/_ext/1109276195/debug.o \
	${OBJECTDIR}/_ext/1109276195/dynamics.o \
	${OBJECTDIR}/_ext/1109276195/init.o \
	${OBJECTDIR}/_ext/1109276195/modelentity.o \
	${OBJECTDIR}/_ext/1109276195/poi.o \
	${OBJECTDIR}/_ext/1109276195/projectionprobe.o \
	${OBJECTDIR}/_ext/1109276195/renderable.o \
	${OBJECTDIR}/_ext/1109276195/renderableaggregate.o \
	${OBJECTDIR}/_ext/1109276195/renderableaggregaterequest.o \
	${OBJECTDIR}/_ext/1109276195/renderablecontainerimpl.o \
	${OBJECTDIR}/_ext/1109276195/renderer.o \
	${OBJECTDIR}/_ext/1109276195/rendernodehdr.o \
	${OBJECTDIR}/_ext/1109276195/rendernodelayer.o \
	${OBJECTDIR}/_ext/1109276195/rendernodeoutput.o \
	${OBJECTDIR}/_ext/1109276195/rendernoderadiance.o \
	${OBJECTDIR}/_ext/1109276195/rendernoderdaloader.o \
	${OBJECTDIR}/_ext/1109276195/rendertree.o \
	${OBJECTDIR}/_ext/1109276195/resloader.o \
	${OBJECTDIR}/_ext/1109276195/strategy.o \
	${OBJECTDIR}/_ext/1047597177/serializer.o \
	${OBJECTDIR}/_ext/1450543479/compiler.o \
	${OBJECTDIR}/_ext/1450543479/emitter.o \
	${OBJECTDIR}/_ext/1450543479/error.o \
	${OBJECTDIR}/_ext/1450543479/lexer.o \
	${OBJECTDIR}/_ext/1450543479/main.o \
	${OBJECTDIR}/_ext/1450543479/parser.o \
	${OBJECTDIR}/_ext/1450543479/symbol.o \
	${OBJECTDIR}/_ext/1022623785/arraylist.o \
	${OBJECTDIR}/_ext/1022623785/balancedtree.o \
	${OBJECTDIR}/_ext/1022623785/hashmap.o \
	${OBJECTDIR}/_ext/1022623785/linkedlist.o \
	${OBJECTDIR}/_ext/1022623785/paramset.o \
	${OBJECTDIR}/_ext/1022623785/quicksort.o \
	${OBJECTDIR}/_ext/1022623785/stack.o \
	${OBJECTDIR}/_ext/1022623785/staging.o \
	${OBJECTDIR}/_ext/1445267685/main.o \
	${OBJECTDIR}/_ext/380693006/decimal.o \
	${OBJECTDIR}/_ext/380693006/fx.o \
	${OBJECTDIR}/_ext/380693006/geometry.o \
	${OBJECTDIR}/_ext/380693006/main.o \
	${OBJECTDIR}/_ext/380693006/matrix.o \
	${OBJECTDIR}/_ext/380693006/quat.o \
	${OBJECTDIR}/_ext/380693006/vector.o \
	${OBJECTDIR}/_ext/1445267685/misc.o \
	${OBJECTDIR}/_ext/579834215/allocator.o \
	${OBJECTDIR}/_ext/579834215/log.o \
	${OBJECTDIR}/_ext/579834215/panic.o \
	${OBJECTDIR}/_ext/579834215/symlib.o \
	${OBJECTDIR}/_ext/579834215/thread.o \
	${OBJECTDIR}/_ext/579834215/timing.o


# C Compiler Flags
CFLAGS=-m64 -Werror `pkg-config gtk+-3.0 --cflags` -fPIC  

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /home/davis/x3drenderlib-code/trunk/bin/libx3dcore.so

/home/davis/x3drenderlib-code/trunk/bin/libx3dcore.so: ${OBJECTFILES}
	${MKDIR} -p /home/davis/x3drenderlib-code/trunk/bin
	${LINK.c} -o /home/davis/x3drenderlib-code/trunk/bin/libx3dcore.so ${OBJECTFILES} ${LDLIBSOPTIONS} -ldl -pthread `pkg-config gtk+-3.0 --libs`  -shared -fPIC

${OBJECTDIR}/_ext/248117513/fbxmodel.o: ../../drivers/loader/fbxmodel.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/248117513
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/248117513/fbxmodel.o ../../drivers/loader/fbxmodel.c

${OBJECTDIR}/_ext/248117513/imageloader.o: ../../drivers/loader/imageloader.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/248117513
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/248117513/imageloader.o ../../drivers/loader/imageloader.c

${OBJECTDIR}/_ext/248117513/main.o: ../../drivers/loader/main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/248117513
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/248117513/main.o ../../drivers/loader/main.c

${OBJECTDIR}/_ext/248117513/modelloader.o: ../../drivers/loader/modelloader.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/248117513
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/248117513/modelloader.o ../../drivers/loader/modelloader.c

${OBJECTDIR}/_ext/248117513/objmodel.o: ../../drivers/loader/objmodel.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/248117513
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/248117513/objmodel.o ../../drivers/loader/objmodel.c

${OBJECTDIR}/_ext/248117513/pbrtmodel.o: ../../drivers/loader/pbrtmodel.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/248117513
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/248117513/pbrtmodel.o ../../drivers/loader/pbrtmodel.c

${OBJECTDIR}/_ext/248117513/rendererconf.o: ../../drivers/loader/rendererconf.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/248117513
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/248117513/rendererconf.o ../../drivers/loader/rendererconf.c

${OBJECTDIR}/_ext/248117513/x3dscene.o: ../../drivers/loader/x3dscene.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/248117513
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/248117513/x3dscene.o ../../drivers/loader/x3dscene.c

${OBJECTDIR}/_ext/1186658598/bvh.o: ../../drivers/renderer/geometry/bvh.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1186658598
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1186658598/bvh.o ../../drivers/renderer/geometry/bvh.c

${OBJECTDIR}/_ext/1186658598/cone_shape.o: ../../drivers/renderer/geometry/cone_shape.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1186658598
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1186658598/cone_shape.o ../../drivers/renderer/geometry/cone_shape.c

${OBJECTDIR}/_ext/1186658598/cylinder_shape.o: ../../drivers/renderer/geometry/cylinder_shape.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1186658598
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1186658598/cylinder_shape.o ../../drivers/renderer/geometry/cylinder_shape.c

${OBJECTDIR}/_ext/1186658598/disk_shape.o: ../../drivers/renderer/geometry/disk_shape.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1186658598
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1186658598/disk_shape.o ../../drivers/renderer/geometry/disk_shape.c

${OBJECTDIR}/_ext/1186658598/geocache.o: ../../drivers/renderer/geometry/geocache.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1186658598
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1186658598/geocache.o ../../drivers/renderer/geometry/geocache.c

${OBJECTDIR}/_ext/1186658598/geometry.o: ../../drivers/renderer/geometry/geometry.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1186658598
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1186658598/geometry.o ../../drivers/renderer/geometry/geometry.c

${OBJECTDIR}/_ext/1186658598/grid.o: ../../drivers/renderer/geometry/grid.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1186658598
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1186658598/grid.o ../../drivers/renderer/geometry/grid.c

${OBJECTDIR}/_ext/1186658598/hyperboloid_shape.o: ../../drivers/renderer/geometry/hyperboloid_shape.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1186658598
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1186658598/hyperboloid_shape.o ../../drivers/renderer/geometry/hyperboloid_shape.c

${OBJECTDIR}/_ext/1186658598/kdtree.o: ../../drivers/renderer/geometry/kdtree.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1186658598
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1186658598/kdtree.o ../../drivers/renderer/geometry/kdtree.c

${OBJECTDIR}/_ext/1186658598/main.o: ../../drivers/renderer/geometry/main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1186658598
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1186658598/main.o ../../drivers/renderer/geometry/main.c

${OBJECTDIR}/_ext/1186658598/octree.o: ../../drivers/renderer/geometry/octree.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1186658598
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1186658598/octree.o ../../drivers/renderer/geometry/octree.c

${OBJECTDIR}/_ext/1186658598/paraboloid_shape.o: ../../drivers/renderer/geometry/paraboloid_shape.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1186658598
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1186658598/paraboloid_shape.o ../../drivers/renderer/geometry/paraboloid_shape.c

${OBJECTDIR}/_ext/1186658598/primitive_parition.o: ../../drivers/renderer/geometry/primitive_parition.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1186658598
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1186658598/primitive_parition.o ../../drivers/renderer/geometry/primitive_parition.c

${OBJECTDIR}/_ext/1186658598/sphere_shape.o: ../../drivers/renderer/geometry/sphere_shape.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1186658598
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1186658598/sphere_shape.o ../../drivers/renderer/geometry/sphere_shape.c

${OBJECTDIR}/_ext/1186658598/trimesh_shape.o: ../../drivers/renderer/geometry/trimesh_shape.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1186658598
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1186658598/trimesh_shape.o ../../drivers/renderer/geometry/trimesh_shape.c

${OBJECTDIR}/_ext/1186658598/trimesh_subdivide.o: ../../drivers/renderer/geometry/trimesh_subdivide.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1186658598
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1186658598/trimesh_subdivide.o ../../drivers/renderer/geometry/trimesh_subdivide.c

${OBJECTDIR}/_ext/1118113710/light.o: ../../drivers/renderer/light/light.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1118113710
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1118113710/light.o ../../drivers/renderer/light/light.c

${OBJECTDIR}/_ext/1561355151/material.o: ../../drivers/renderer/material/material.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1561355151
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1561355151/material.o ../../drivers/renderer/material/material.c

${OBJECTDIR}/_ext/1698666324/volume.o: ../../drivers/renderer/misc/volume.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1698666324
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1698666324/volume.o ../../drivers/renderer/misc/volume.c

${OBJECTDIR}/_ext/1746317338/binary_out.o: ../../drivers/renderer/out/binary_out.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1746317338
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1746317338/binary_out.o ../../drivers/renderer/out/binary_out.c

${OBJECTDIR}/_ext/1746317338/image_out.o: ../../drivers/renderer/out/image_out.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1746317338
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1746317338/image_out.o ../../drivers/renderer/out/image_out.c

${OBJECTDIR}/_ext/1746317338/out.o: ../../drivers/renderer/out/out.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1746317338
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1746317338/out.o ../../drivers/renderer/out/out.c

${OBJECTDIR}/_ext/1746317338/screen_out.o: ../../drivers/renderer/out/screen_out.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1746317338
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1746317338/screen_out.o ../../drivers/renderer/out/screen_out.c

${OBJECTDIR}/_ext/1746317338/stream_out.o: ../../drivers/renderer/out/stream_out.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1746317338
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1746317338/stream_out.o ../../drivers/renderer/out/stream_out.c

${OBJECTDIR}/_ext/1122083400/fisheye.o: ../../drivers/renderer/probe/fisheye.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1122083400
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1122083400/fisheye.o ../../drivers/renderer/probe/fisheye.c

${OBJECTDIR}/_ext/1122083400/obj_probe.o: ../../drivers/renderer/probe/obj_probe.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1122083400
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1122083400/obj_probe.o ../../drivers/renderer/probe/obj_probe.c

${OBJECTDIR}/_ext/1122083400/orthogonal.o: ../../drivers/renderer/probe/orthogonal.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1122083400
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1122083400/orthogonal.o ../../drivers/renderer/probe/orthogonal.c

${OBJECTDIR}/_ext/1122083400/perspective.o: ../../drivers/renderer/probe/perspective.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1122083400
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1122083400/perspective.o ../../drivers/renderer/probe/perspective.c

${OBJECTDIR}/_ext/1122083400/proj_probe.o: ../../drivers/renderer/probe/proj_probe.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1122083400
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1122083400/proj_probe.o ../../drivers/renderer/probe/proj_probe.c

${OBJECTDIR}/_ext/1122083400/spatial_probe.o: ../../drivers/renderer/probe/spatial_probe.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1122083400
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1122083400/spatial_probe.o ../../drivers/renderer/probe/spatial_probe.c

${OBJECTDIR}/_ext/1122083400/spherical.o: ../../drivers/renderer/probe/spherical.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1122083400
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1122083400/spherical.o ../../drivers/renderer/probe/spherical.c

${OBJECTDIR}/_ext/767442731/bufferedobject.o: ../../drivers/renderer/renderer/bufferedobject.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/bufferedobject.o ../../drivers/renderer/renderer/bufferedobject.c

${OBJECTDIR}/_ext/767442731/colorspectrum.o: ../../drivers/renderer/renderer/colorspectrum.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/colorspectrum.o ../../drivers/renderer/renderer/colorspectrum.c

${OBJECTDIR}/_ext/767442731/compositepipeline.o: ../../drivers/renderer/renderer/compositepipeline.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/compositepipeline.o ../../drivers/renderer/renderer/compositepipeline.c

${OBJECTDIR}/_ext/767442731/fbo.o: ../../drivers/renderer/renderer/fbo.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/fbo.o ../../drivers/renderer/renderer/fbo.c

${OBJECTDIR}/_ext/767442731/main.o: ../../drivers/renderer/renderer/main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/main.o ../../drivers/renderer/renderer/main.c

${OBJECTDIR}/_ext/767442731/pmrenderer.o: ../../drivers/renderer/renderer/pmrenderer.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/pmrenderer.o ../../drivers/renderer/renderer/pmrenderer.c

${OBJECTDIR}/_ext/767442731/ptrenderer.o: ../../drivers/renderer/renderer/ptrenderer.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/ptrenderer.o ../../drivers/renderer/renderer/ptrenderer.c

${OBJECTDIR}/_ext/767442731/rasterization.o: ../../drivers/renderer/renderer/rasterization.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/rasterization.o ../../drivers/renderer/renderer/rasterization.c

${OBJECTDIR}/_ext/767442731/rasterizer.o: ../../drivers/renderer/renderer/rasterizer.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/rasterizer.o ../../drivers/renderer/renderer/rasterizer.c

${OBJECTDIR}/_ext/767442731/rasterpass.o: ../../drivers/renderer/renderer/rasterpass.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/rasterpass.o ../../drivers/renderer/renderer/rasterpass.c

${OBJECTDIR}/_ext/767442731/rasterpipeline.o: ../../drivers/renderer/renderer/rasterpipeline.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/rasterpipeline.o ../../drivers/renderer/renderer/rasterpipeline.c

${OBJECTDIR}/_ext/767442731/raytracepipeline.o: ../../drivers/renderer/renderer/raytracepipeline.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/raytracepipeline.o ../../drivers/renderer/renderer/raytracepipeline.c

${OBJECTDIR}/_ext/767442731/rendererinsmod.o: ../../drivers/renderer/renderer/rendererinsmod.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/rendererinsmod.o ../../drivers/renderer/renderer/rendererinsmod.c

${OBJECTDIR}/_ext/767442731/rendertarget.o: ../../drivers/renderer/renderer/rendertarget.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/rendertarget.o ../../drivers/renderer/renderer/rendertarget.c

${OBJECTDIR}/_ext/767442731/rtrenderer.o: ../../drivers/renderer/renderer/rtrenderer.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/rtrenderer.o ../../drivers/renderer/renderer/rtrenderer.c

${OBJECTDIR}/_ext/767442731/shaderformulate.o: ../../drivers/renderer/renderer/shaderformulate.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/shaderformulate.o ../../drivers/renderer/renderer/shaderformulate.c

${OBJECTDIR}/_ext/767442731/surface.o: ../../drivers/renderer/renderer/surface.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/surface.o ../../drivers/renderer/renderer/surface.c

${OBJECTDIR}/_ext/767442731/trianglebuffer.o: ../../drivers/renderer/renderer/trianglebuffer.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/trianglebuffer.o ../../drivers/renderer/renderer/trianglebuffer.c

${OBJECTDIR}/_ext/767442731/vbo.o: ../../drivers/renderer/renderer/vbo.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/vbo.o ../../drivers/renderer/renderer/vbo.c

${OBJECTDIR}/_ext/767442731/vertexprocessor.o: ../../drivers/renderer/renderer/vertexprocessor.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/767442731
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/767442731/vertexprocessor.o ../../drivers/renderer/renderer/vertexprocessor.c

${OBJECTDIR}/_ext/501084237/basic_rt_shader.o: ../../drivers/renderer/shader/basic_rt_shader.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/501084237
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/501084237/basic_rt_shader.o ../../drivers/renderer/shader/basic_rt_shader.c

${OBJECTDIR}/_ext/501084237/main.o: ../../drivers/renderer/shader/main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/501084237
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/501084237/main.o ../../drivers/renderer/shader/main.c

${OBJECTDIR}/_ext/501084237/shader.o: ../../drivers/renderer/shader/shader.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/501084237
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/501084237/shader.o ../../drivers/renderer/shader/shader.c

${OBJECTDIR}/_ext/1109276195/debug.o: ../../kernel/debug.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1109276195
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1109276195/debug.o ../../kernel/debug.c

${OBJECTDIR}/_ext/1109276195/dynamics.o: ../../kernel/dynamics.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1109276195
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1109276195/dynamics.o ../../kernel/dynamics.c

${OBJECTDIR}/_ext/1109276195/init.o: ../../kernel/init.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1109276195
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1109276195/init.o ../../kernel/init.c

${OBJECTDIR}/_ext/1109276195/modelentity.o: ../../kernel/modelentity.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1109276195
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1109276195/modelentity.o ../../kernel/modelentity.c

${OBJECTDIR}/_ext/1109276195/poi.o: ../../kernel/poi.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1109276195
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1109276195/poi.o ../../kernel/poi.c

${OBJECTDIR}/_ext/1109276195/projectionprobe.o: ../../kernel/projectionprobe.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1109276195
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1109276195/projectionprobe.o ../../kernel/projectionprobe.c

${OBJECTDIR}/_ext/1109276195/renderable.o: ../../kernel/renderable.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1109276195
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1109276195/renderable.o ../../kernel/renderable.c

${OBJECTDIR}/_ext/1109276195/renderableaggregate.o: ../../kernel/renderableaggregate.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1109276195
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1109276195/renderableaggregate.o ../../kernel/renderableaggregate.c

${OBJECTDIR}/_ext/1109276195/renderableaggregaterequest.o: ../../kernel/renderableaggregaterequest.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1109276195
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1109276195/renderableaggregaterequest.o ../../kernel/renderableaggregaterequest.c

${OBJECTDIR}/_ext/1109276195/renderablecontainerimpl.o: ../../kernel/renderablecontainerimpl.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1109276195
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1109276195/renderablecontainerimpl.o ../../kernel/renderablecontainerimpl.c

${OBJECTDIR}/_ext/1109276195/renderer.o: ../../kernel/renderer.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1109276195
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1109276195/renderer.o ../../kernel/renderer.c

${OBJECTDIR}/_ext/1109276195/rendernodehdr.o: ../../kernel/rendernodehdr.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1109276195
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1109276195/rendernodehdr.o ../../kernel/rendernodehdr.c

${OBJECTDIR}/_ext/1109276195/rendernodelayer.o: ../../kernel/rendernodelayer.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1109276195
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1109276195/rendernodelayer.o ../../kernel/rendernodelayer.c

${OBJECTDIR}/_ext/1109276195/rendernodeoutput.o: ../../kernel/rendernodeoutput.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1109276195
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1109276195/rendernodeoutput.o ../../kernel/rendernodeoutput.c

${OBJECTDIR}/_ext/1109276195/rendernoderadiance.o: ../../kernel/rendernoderadiance.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1109276195
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1109276195/rendernoderadiance.o ../../kernel/rendernoderadiance.c

${OBJECTDIR}/_ext/1109276195/rendernoderdaloader.o: ../../kernel/rendernoderdaloader.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1109276195
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1109276195/rendernoderdaloader.o ../../kernel/rendernoderdaloader.c

${OBJECTDIR}/_ext/1109276195/rendertree.o: ../../kernel/rendertree.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1109276195
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1109276195/rendertree.o ../../kernel/rendertree.c

${OBJECTDIR}/_ext/1109276195/resloader.o: ../../kernel/resloader.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1109276195
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1109276195/resloader.o ../../kernel/resloader.c

${OBJECTDIR}/_ext/1109276195/strategy.o: ../../kernel/strategy.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1109276195
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1109276195/strategy.o ../../kernel/strategy.c

${OBJECTDIR}/_ext/1047597177/serializer.o: ../../lib/codecs/serializer.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1047597177
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1047597177/serializer.o ../../lib/codecs/serializer.c

${OBJECTDIR}/_ext/1450543479/compiler.o: ../../lib/compiler/compiler.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1450543479
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1450543479/compiler.o ../../lib/compiler/compiler.c

${OBJECTDIR}/_ext/1450543479/emitter.o: ../../lib/compiler/emitter.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1450543479
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1450543479/emitter.o ../../lib/compiler/emitter.c

${OBJECTDIR}/_ext/1450543479/error.o: ../../lib/compiler/error.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1450543479
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1450543479/error.o ../../lib/compiler/error.c

${OBJECTDIR}/_ext/1450543479/lexer.o: ../../lib/compiler/lexer.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1450543479
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1450543479/lexer.o ../../lib/compiler/lexer.c

${OBJECTDIR}/_ext/1450543479/main.o: ../../lib/compiler/main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1450543479
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1450543479/main.o ../../lib/compiler/main.c

${OBJECTDIR}/_ext/1450543479/parser.o: ../../lib/compiler/parser.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1450543479
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1450543479/parser.o ../../lib/compiler/parser.c

${OBJECTDIR}/_ext/1450543479/symbol.o: ../../lib/compiler/symbol.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1450543479
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1450543479/symbol.o ../../lib/compiler/symbol.c

${OBJECTDIR}/_ext/1022623785/arraylist.o: ../../lib/container/arraylist.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1022623785
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1022623785/arraylist.o ../../lib/container/arraylist.c

${OBJECTDIR}/_ext/1022623785/balancedtree.o: ../../lib/container/balancedtree.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1022623785
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1022623785/balancedtree.o ../../lib/container/balancedtree.c

${OBJECTDIR}/_ext/1022623785/hashmap.o: ../../lib/container/hashmap.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1022623785
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1022623785/hashmap.o ../../lib/container/hashmap.c

${OBJECTDIR}/_ext/1022623785/linkedlist.o: ../../lib/container/linkedlist.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1022623785
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1022623785/linkedlist.o ../../lib/container/linkedlist.c

${OBJECTDIR}/_ext/1022623785/paramset.o: ../../lib/container/paramset.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1022623785
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1022623785/paramset.o ../../lib/container/paramset.c

${OBJECTDIR}/_ext/1022623785/quicksort.o: ../../lib/container/quicksort.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1022623785
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1022623785/quicksort.o ../../lib/container/quicksort.c

${OBJECTDIR}/_ext/1022623785/stack.o: ../../lib/container/stack.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1022623785
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1022623785/stack.o ../../lib/container/stack.c

${OBJECTDIR}/_ext/1022623785/staging.o: ../../lib/container/staging.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1022623785
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1022623785/staging.o ../../lib/container/staging.c

${OBJECTDIR}/_ext/1445267685/main.o: ../../lib/main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445267685
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445267685/main.o ../../lib/main.c

${OBJECTDIR}/_ext/380693006/decimal.o: ../../lib/math/decimal.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/380693006
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/380693006/decimal.o ../../lib/math/decimal.c

${OBJECTDIR}/_ext/380693006/fx.o: ../../lib/math/fx.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/380693006
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/380693006/fx.o ../../lib/math/fx.c

${OBJECTDIR}/_ext/380693006/geometry.o: ../../lib/math/geometry.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/380693006
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/380693006/geometry.o ../../lib/math/geometry.c

${OBJECTDIR}/_ext/380693006/main.o: ../../lib/math/main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/380693006
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/380693006/main.o ../../lib/math/main.c

${OBJECTDIR}/_ext/380693006/matrix.o: ../../lib/math/matrix.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/380693006
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/380693006/matrix.o ../../lib/math/matrix.c

${OBJECTDIR}/_ext/380693006/quat.o: ../../lib/math/quat.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/380693006
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/380693006/quat.o ../../lib/math/quat.c

${OBJECTDIR}/_ext/380693006/vector.o: ../../lib/math/vector.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/380693006
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/380693006/vector.o ../../lib/math/vector.c

${OBJECTDIR}/_ext/1445267685/misc.o: ../../lib/misc.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445267685
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445267685/misc.o ../../lib/misc.c

${OBJECTDIR}/_ext/579834215/allocator.o: ../../lib/system/allocator.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/579834215
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/579834215/allocator.o ../../lib/system/allocator.c

${OBJECTDIR}/_ext/579834215/log.o: ../../lib/system/log.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/579834215
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/579834215/log.o ../../lib/system/log.c

${OBJECTDIR}/_ext/579834215/panic.o: ../../lib/system/panic.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/579834215
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/579834215/panic.o ../../lib/system/panic.c

${OBJECTDIR}/_ext/579834215/symlib.o: ../../lib/system/symlib.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/579834215
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/579834215/symlib.o ../../lib/system/symlib.c

${OBJECTDIR}/_ext/579834215/thread.o: ../../lib/system/thread.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/579834215
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/579834215/thread.o ../../lib/system/thread.c

${OBJECTDIR}/_ext/579834215/timing.o: ../../lib/system/timing.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/579834215
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/579834215/timing.o ../../lib/system/timing.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} /home/davis/x3drenderlib-code/trunk/bin/libx3dcore.so

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
