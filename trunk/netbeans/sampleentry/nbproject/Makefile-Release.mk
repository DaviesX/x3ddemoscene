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
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1817864255/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-Wl,-rpath,../x3dcore/dist/Release/GNU-Linux-x86 -L../x3dcore/dist/Release/GNU-Linux-x86 -lx3dcore -Wl,-rpath,../usr/dist/Release/GNU-Linux-x86 -L../usr/dist/Release/GNU-Linux-x86 -lusr

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /home/davis/x3drenderlib-code/trunk/bin/sampleentry

/home/davis/x3drenderlib-code/trunk/bin/sampleentry: ../x3dcore/dist/Release/GNU-Linux-x86/libx3dcore.so

/home/davis/x3drenderlib-code/trunk/bin/sampleentry: ../usr/dist/Release/GNU-Linux-x86/libusr.so

/home/davis/x3drenderlib-code/trunk/bin/sampleentry: ${OBJECTFILES}
	${MKDIR} -p /home/davis/x3drenderlib-code/trunk/bin
	${LINK.cc} -o /home/davis/x3drenderlib-code/trunk/bin/sampleentry ${OBJECTFILES} ${LDLIBSOPTIONS} `pkg-config gtk+-3.0 --libs` 

${OBJECTDIR}/_ext/1817864255/main.o: ../../external/sample_entry/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1817864255
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DX3D_COMPILER_GCC -DX3D_PLATFORM_POSIX -I../../include -I../../lib -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1817864255/main.o ../../external/sample_entry/main.cpp

# Subprojects
.build-subprojects:
	cd ../x3dcore && ${MAKE}  -f Makefile CONF=Release
	cd ../usr && ${MAKE}  -f Makefile CONF=Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} /home/davis/x3drenderlib-code/trunk/bin/sampleentry

# Subprojects
.clean-subprojects:
	cd ../x3dcore && ${MAKE}  -f Makefile CONF=Release clean
	cd ../usr && ${MAKE}  -f Makefile CONF=Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
