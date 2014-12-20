/*! \file include/x3d/world.h
    \brief virtual world management goes here. note that all api call should be done in the same thread as where kernel loop is running */

#ifndef WORLD_H_INCLUDED
#define WORLD_H_INCLUDED

#include <x3d/common.h>


enum WorldGenre {
        WorldGenreTest,
        WorldGenreOutdoor,
        WorldGenreIndoor
};

enum WorldNodeSlot {
        WorldNode0,
        WorldNode1,
        WorldNode2
};

struct model_entity;
struct pointofinterest;
struct strategy_occluder;

/*
 * functions' declaration
 */
void init_world ( void );
void free_world ( void );

void world_use_slot ( enum WorldNodeSlot slot );

void world_flush ( void );
bool world_load_from_file ( char* filename );
bool world_save_to_file ( char* filename );

// void world_use_genre ( enum WorldGenre genre );
void world_use_cell_size ( float cell_x, float cell_y, float cell_z );
void world_use_modelentity ( struct model_entity* entity, enum WorldGenre genre );
void world_ignore_modelentity ( struct model_entity* entity );
void world_use_pointofinterest ( struct pointofinterest* poi );
void world_use_occluder ( struct strategy_occluder* occluder );

void world_affect ( void );


#endif // WORLD_H_INCLUDED
