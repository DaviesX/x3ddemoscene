#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED


/**
  * Switch between different platforms
  * #define X3D_COMPILER_GCC
  * #define X3D_COMPILER_MSVC
  * #define X3D_PLATFORM_POSIX
  * #define X3D_PLATFORM_WIN32
 **/

/**
 * Enable this to switch on debug mode
 * #define X3D_DEBUG_MODE
 **/

#ifndef X3D_DEBUG_MODE
#  define NDEBUG
#endif

/* Debug if statement, Debug session block */
#ifdef X3D_DEBUG_MODE
#  define DebugIf(_statement, _code) \
           if ( _statement ) { \
                _code \
           }
#  define DebugSession(_code)                  _code
#else
#  define DebugIf(_statement, _code)	\
           if (0) ;
#  define DebugSession(_code)
#endif

#if defined (X3D_DEBUG_MODE)
#  define X3D_VERSION_STRING	"v 1.0 debug"
#  pragma GCC diagnostic ignored "-Wunused-function"
#  pragma GCC diagnostic ignored "-Wmissing-braces"
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#else
#  define X3D_VERSION_STRING	"v 1.0"
#  pragma GCC diagnostic ignored "-Wunused-function"
#  pragma GCC diagnostic ignored "-Wmissing-braces"
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#define SessionIn(_session)
#define PseudoDef(defn)

#ifdef X3D_COMPILER_GCC
#  define __stdcall                     __attribute__((stdcall))
#  define __cdecl                       __attribute__((cdecl))
#  define __fastcall                    __attribute__((fastcall))
#  ifdef __cplusplus
#    define __dlexport                  __attribute__((__visibility__("default")))
#  else
#    define __dlexport                  __attribute__((__visibility__("default")))
#  endif
#  ifdef __cplusplus
#    define cpp_compiler
#  else
#    define c_compiler
#  endif
#  define asm_return( _fake_val ) \
        { \
                asm ("ret \n"); \
                return (_fake_val); \
        }
#endif


/*
 * Include relative headers
 */
#ifdef X3D_COMPILER_GCC
#  ifndef _GNU_SOURCE
#    define _GNU_SOURCE
#  endif
#  include <stdio.h>
#  include <stdlib.h>
#  include <stdarg.h>
#  include <stddef.h>
#  include <stdint.h>
#  include <stdbool.h>
#  include <unistd.h>
#  include <memory.h>
#  include <string.h>
#  include <ctype.h>
#  include <assert.h>
#  include <math.h>
#  include <pthread.h>
#  include <semaphore.h>
#  include <limits.h>
#  include <time.h>
#  include <errno.h>
#  include <sys/types.h>
#  include <sys/wait.h>
#elif defined (X3D_COMPILER_MSVC)
#endif


#endif // COMMON_H_INCLUDED
