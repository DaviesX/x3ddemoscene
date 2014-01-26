#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED


/** Enable this to switch on debug mode **/
#define X3D_DEBUG_MODE
#ifndef X3D_DEBUG_MODE
#define NDEBUG
#endif

/** Debug if statement, Debug session block **/
#ifdef X3D_DEBUG_MODE
#define DEBUG_IF( _statement, _code )		if ( _statement ) {_code}
#define DEBUG_SESSION( _code )				_code
#else
#define DEBUG_IF( _statement, _code )		if ( 0 ) {}
#define DEBUG_SESSION( _code )
#endif

/** Enable this to not to compile with obsolete code **/
// #define X3D_STRIP_OBSOLETE_CODE
#ifndef X3D_STRIP_OBSOLETE_CODE
#define OBSOLETE_SESSION( _code )
#else
#define OBSOLETE_SESSION( _code )		_code
#endif


#define pseudo_def( defn )
#define SESSION_IN( _session )
#define nullptr				NULL
#define cast( _var )			(typeof (_var))

#ifdef X3D_DEBUG_MODE
#define RENDER_LIB_VERSION_STRING	"v 1.0 debug"
/*#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wformat-security"
#pragma GCC diagnostic ignored "-Wmissing-declarations" */
#pragma GCC diagnostic ignored "-Wformat-security"
#pragma GCC diagnostic ignored "-Wmissing-braces"
#else
#define RENDER_LIB_VERSION_STRING	"v 1.0"
#endif

#define __stdcall			__attribute__((stdcall))
#define __cdecl				__attribute__((cdecl))
#define __fastcall			__attribute__((fastcall))

/*
 * Include relative headers
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef uint8_t untyped;


#endif // COMMON_H_INCLUDED
