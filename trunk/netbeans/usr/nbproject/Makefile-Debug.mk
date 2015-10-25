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
	${OBJECTDIR}/_ext/1973935413/cornellbox.o \
	${OBJECTDIR}/_ext/1548469272/editassetlist.o \
	${OBJECTDIR}/_ext/1548469272/editbenchmark.o \
	${OBJECTDIR}/_ext/1548469272/editentityproperty.o \
	${OBJECTDIR}/_ext/1548469272/editor.o \
	${OBJECTDIR}/_ext/1548469272/editpowersaveswitch.o \
	${OBJECTDIR}/_ext/1548469272/editprojectmgr.o \
	${OBJECTDIR}/_ext/1548469272/editrenderconf.o \
	${OBJECTDIR}/_ext/1548469272/editrenderregion.o \
	${OBJECTDIR}/_ext/1548469272/editrunner.o \
	${OBJECTDIR}/_ext/1548469272/editworlddata.o \
	${OBJECTDIR}/_ext/1067377271/dynamics_editor.o \
	${OBJECTDIR}/_ext/1067377271/entity_editor.o \
	${OBJECTDIR}/_ext/1067377271/gtkgui.o \
	${OBJECTDIR}/_ext/1067377271/main_editor.o \
	${OBJECTDIR}/_ext/1067377271/main_editor_menu.o \
	${OBJECTDIR}/_ext/1067377271/project_manager.o \
	${OBJECTDIR}/_ext/1067377271/renderable_editor.o \
	${OBJECTDIR}/_ext/1067377271/renderer_configurator.o \
	${OBJECTDIR}/_ext/1067377271/splash_screen.o \
	${OBJECTDIR}/_ext/1445276660/usr_cinematicprogs.o \
	${OBJECTDIR}/_ext/1445276660/usr_entityprogs.o \
	${OBJECTDIR}/_ext/1445276660/usr_kernel.o \
	${OBJECTDIR}/_ext/1445276660/usr_missionprogs.o \
	${OBJECTDIR}/_ext/1445276660/usr_projectionprobe.o \
	${OBJECTDIR}/_ext/1445276660/usr_renderable.o \
	${OBJECTDIR}/_ext/1445276660/usr_renderaggregate.o \
	${OBJECTDIR}/_ext/1445276660/usr_renderer.o \
	${OBJECTDIR}/_ext/1445276660/usr_resourceloader.o \
	${OBJECTDIR}/_ext/1445276660/usr_world.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64 `pkg-config gtk+-3.0 --cflags` -Werror -fPIC  
CXXFLAGS=-m64 `pkg-config gtk+-3.0 --cflags` -Werror -fPIC  

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /home/davis/x3drenderlib-code/trunk/bin/libusr.so

/home/davis/x3drenderlib-code/trunk/bin/libusr.so: ${OBJECTFILES}
	${MKDIR} -p /home/davis/x3drenderlib-code/trunk/bin
	${LINK.cc} -o /home/davis/x3drenderlib-code/trunk/bin/libusr.so ${OBJECTFILES} ${LDLIBSOPTIONS} `pkg-config gtk+-3.0 --libs` -shared -fPIC

${OBJECTDIR}/_ext/1973935413/cornellbox.o: ../../usr/editor/cornellbox/cornellbox.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1973935413
	${RM} "$@.d"
	$(COMPILE.c) -g -DX3D_COMPILER_GCC -DX3D_DEBUG_MODE -DX3D_PLATFORM_POSIX -I../../lib -I../../include -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1973935413/cornellbox.o ../../usr/editor/cornellbox/cornellbox.c

${OBJECTDIR}/_ext/1548469272/editassetlist.o: ../../usr/editor/editassetlist.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1548469272
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1548469272/editassetlist.o ../../usr/editor/editassetlist.cpp

${OBJECTDIR}/_ext/1548469272/editbenchmark.o: ../../usr/editor/editbenchmark.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1548469272
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1548469272/editbenchmark.o ../../usr/editor/editbenchmark.cpp

${OBJECTDIR}/_ext/1548469272/editentityproperty.o: ../../usr/editor/editentityproperty.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1548469272
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1548469272/editentityproperty.o ../../usr/editor/editentityproperty.cpp

${OBJECTDIR}/_ext/1548469272/editor.o: ../../usr/editor/editor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1548469272
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1548469272/editor.o ../../usr/editor/editor.cpp

${OBJECTDIR}/_ext/1548469272/editpowersaveswitch.o: ../../usr/editor/editpowersaveswitch.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1548469272
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1548469272/editpowersaveswitch.o ../../usr/editor/editpowersaveswitch.cpp

${OBJECTDIR}/_ext/1548469272/editprojectmgr.o: ../../usr/editor/editprojectmgr.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1548469272
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1548469272/editprojectmgr.o ../../usr/editor/editprojectmgr.cpp

${OBJECTDIR}/_ext/1548469272/editrenderconf.o: ../../usr/editor/editrenderconf.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1548469272
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1548469272/editrenderconf.o ../../usr/editor/editrenderconf.cpp

${OBJECTDIR}/_ext/1548469272/editrenderregion.o: ../../usr/editor/editrenderregion.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1548469272
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1548469272/editrenderregion.o ../../usr/editor/editrenderregion.cpp

${OBJECTDIR}/_ext/1548469272/editrunner.o: ../../usr/editor/editrunner.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1548469272
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1548469272/editrunner.o ../../usr/editor/editrunner.cpp

${OBJECTDIR}/_ext/1548469272/editworlddata.o: ../../usr/editor/editworlddata.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1548469272
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1548469272/editworlddata.o ../../usr/editor/editworlddata.cpp

${OBJECTDIR}/_ext/1067377271/dynamics_editor.o: ../../usr/editor/gtkfrontend/dynamics_editor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1067377271
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1067377271/dynamics_editor.o ../../usr/editor/gtkfrontend/dynamics_editor.cpp

${OBJECTDIR}/_ext/1067377271/entity_editor.o: ../../usr/editor/gtkfrontend/entity_editor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1067377271
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1067377271/entity_editor.o ../../usr/editor/gtkfrontend/entity_editor.cpp

${OBJECTDIR}/_ext/1067377271/gtkgui.o: ../../usr/editor/gtkfrontend/gtkgui.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1067377271
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1067377271/gtkgui.o ../../usr/editor/gtkfrontend/gtkgui.cpp

${OBJECTDIR}/_ext/1067377271/main_editor.o: ../../usr/editor/gtkfrontend/main_editor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1067377271
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1067377271/main_editor.o ../../usr/editor/gtkfrontend/main_editor.cpp

${OBJECTDIR}/_ext/1067377271/main_editor_menu.o: ../../usr/editor/gtkfrontend/main_editor_menu.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1067377271
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1067377271/main_editor_menu.o ../../usr/editor/gtkfrontend/main_editor_menu.cpp

${OBJECTDIR}/_ext/1067377271/project_manager.o: ../../usr/editor/gtkfrontend/project_manager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1067377271
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1067377271/project_manager.o ../../usr/editor/gtkfrontend/project_manager.cpp

${OBJECTDIR}/_ext/1067377271/renderable_editor.o: ../../usr/editor/gtkfrontend/renderable_editor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1067377271
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1067377271/renderable_editor.o ../../usr/editor/gtkfrontend/renderable_editor.cpp

${OBJECTDIR}/_ext/1067377271/renderer_configurator.o: ../../usr/editor/gtkfrontend/renderer_configurator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1067377271
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1067377271/renderer_configurator.o ../../usr/editor/gtkfrontend/renderer_configurator.cpp

${OBJECTDIR}/_ext/1067377271/splash_screen.o: ../../usr/editor/gtkfrontend/splash_screen.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1067377271
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1067377271/splash_screen.o ../../usr/editor/gtkfrontend/splash_screen.cpp

${OBJECTDIR}/_ext/1445276660/usr_cinematicprogs.o: ../../usr/usr_cinematicprogs.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445276660
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445276660/usr_cinematicprogs.o ../../usr/usr_cinematicprogs.cpp

${OBJECTDIR}/_ext/1445276660/usr_entityprogs.o: ../../usr/usr_entityprogs.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445276660
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445276660/usr_entityprogs.o ../../usr/usr_entityprogs.cpp

${OBJECTDIR}/_ext/1445276660/usr_kernel.o: ../../usr/usr_kernel.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445276660
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445276660/usr_kernel.o ../../usr/usr_kernel.cpp

${OBJECTDIR}/_ext/1445276660/usr_missionprogs.o: ../../usr/usr_missionprogs.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445276660
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445276660/usr_missionprogs.o ../../usr/usr_missionprogs.cpp

${OBJECTDIR}/_ext/1445276660/usr_projectionprobe.o: ../../usr/usr_projectionprobe.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445276660
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445276660/usr_projectionprobe.o ../../usr/usr_projectionprobe.cpp

${OBJECTDIR}/_ext/1445276660/usr_renderable.o: ../../usr/usr_renderable.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445276660
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445276660/usr_renderable.o ../../usr/usr_renderable.cpp

${OBJECTDIR}/_ext/1445276660/usr_renderaggregate.o: ../../usr/usr_renderaggregate.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445276660
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445276660/usr_renderaggregate.o ../../usr/usr_renderaggregate.cpp

${OBJECTDIR}/_ext/1445276660/usr_renderer.o: ../../usr/usr_renderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445276660
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445276660/usr_renderer.o ../../usr/usr_renderer.cpp

${OBJECTDIR}/_ext/1445276660/usr_resourceloader.o: ../../usr/usr_resourceloader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445276660
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445276660/usr_resourceloader.o ../../usr/usr_resourceloader.cpp

${OBJECTDIR}/_ext/1445276660/usr_world.o: ../../usr/usr_world.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445276660
	${RM} "$@.d"
	$(COMPILE.cc) -g -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../lib -I../../include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445276660/usr_world.o ../../usr/usr_world.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} /home/davis/x3drenderlib-code/trunk/bin/libusr.so

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
