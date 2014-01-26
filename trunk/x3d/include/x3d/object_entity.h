#ifndef OBJECT_ENTITY_H_INCLUDED
#define OBJECT_ENTITY_H_INCLUDED


enum OBJECT_ENTITY_IDR {
	OBJECT_ENTITY_STATIC = 1 << 1,
	OBJECT_ENTITY_DYNAMIC = 1 << 2,
	OBJECT_ENTITY_GLOBAL = 1 << 3,
	OBJECT_ENTITY_RENDERABLE = 1 << 4,
	OBJECT_ENTITY_KINEMATIC	= 1 << 5,
	OBJECT_ENTITY_LOGICAL = 1 << 6
};


struct renderable;
struct dynamics;
struct logical;

struct stdmesh {
	int *index;
	int n_index;
	struct point3d *vertex;
	int n_vertex;
};

struct object_entity {
	enum OBJECT_ENTITY_IDR idr;
	struct box3d bound;
	struct renderable *renderable;
	struct dynamics *dynamics;
	struct logical *logical;
};


#endif // OBJECT_ENTITY_H_INCLUDED
