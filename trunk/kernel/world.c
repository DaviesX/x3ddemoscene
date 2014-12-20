/*! \file kernel/world.c
    \brief virtual world management goes here. */

#include <x3d/world.h>

struct world {
        enum WorldGenre genre;
        bool has_new_res;
};

struct world     g_world_slots[5] = {0};
struct world*    g_curr_world = nullptr;


void init_world ( void )
{
}

void free_world ( void )
{
}

void world_use_slot ( enum WorldNodeSlot slot )
{
        g_curr_world = &g_world_slots[slot];
}

void world_flush ( void )
{
}

bool world_load_from_file ( char* filename )
{
        return false;
}

bool world_save_to_file ( char* filename )
{
        return false;
}

void world_use_cell_size ( float cell_x, float cell_y, float cell_z )
{
}

void world_use_modelentity ( struct model_entity* entity, enum WorldGenre genre )
{
}

void world_ignore_modelentity ( struct model_entity* entity )
{
}

void world_use_pointofinterest ( struct pointofinterest* poi )
{
}

void world_use_occluder ( struct strategy_occluder* occluder )
{
}

void world_affect ( void )
{
}
