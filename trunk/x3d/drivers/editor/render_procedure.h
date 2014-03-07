#ifndef RENDERPROCEDURE_H_INCLUDED
#define RENDERPROCEDURE_H_INCLUDED

#if 0

/*
 * Include system libraries
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
 * Include relatives headers
 */

enum RP_SCENE_ERR {
        RP_SCENE_NORMAL,
        RP_SCENE_LOADED,
        RP_SCENE_LOAD_FAILED,
        RP_SCENE_NONEXIST
};


/*
 * Functions' declarations
 */
void rp_init ( void );
void rp_free ( void );
void rp_new_scene ();
enum RP_SCENE_ERR rp_load_scene ( char *filename );
enum RP_SCENE_ERR rp_unload_scene ( void );
int rp_load_model ( char *filename );
int rp_unload_model ( char *filename );
void rp_update_renderer ( void );
void rp_render ( void );
void rp_stop_renderer ( void );
void rp_test ( void );

#endif // 0

#endif // RENDERPROCEDURE_H_INCLUDED
