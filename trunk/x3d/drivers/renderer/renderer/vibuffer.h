#ifndef VIBUFFER_H_INCLUDED
#define VIBUFFER_H_INCLUDED


struct vertattri;
struct vertex_buffer;
struct index_buffer;


enum VERTEX_ELM_FORMAT {
	VERTEX_ELM_INT8,
	VERTEX_ELM_INT16,
	VERTEX_ELM_INT32,
	VERTEX_ELM_FLOAT,
	VERTEX_ELM_DOUBLE
};

/*
 * structures
 */
#define MAX_VERT_ATTRI		10
struct vertattri {
	enum VERTEX_ELM_FORMAT format[MAX_VERT_ATTRI];
	int n_elm[MAX_VERT_ATTRI];
	int offset[MAX_VERT_ATTRI];
	int n_attri;
	int vertsize;
};

struct vertex_buffer {
	void *buffer;
	untyped *curr;
	struct vertattri attri;
	bool is_static;
	int elm_cnt;
};

enum INDEX_ELEMENT_FORMAT {
	INDEX_ELEMENT_INT8,
	INDEX_ELEMENT_INT16,
	INDEX_ELEMENT_INT32,
};

struct index_buffer {
	void *buffer;
	untyped *curr;
	enum INDEX_ELEMENT_FORMAT format;
	bool is_static;
	int elm_cnt;
};

/*
 * functions' declaration
 */
struct vertex_buffer *create_vbuffer ( bool is_static );
struct index_buffer *create_ibuffer ( bool is_static );
void free_vbuffer ( struct vertex_buffer *vbuffer );
void free_ibuffer ( struct index_buffer *ibuffer );

void vertattri_init ( struct vertattri *vattri );
void vertattri_set ( int i, enum VERTEX_ELM_FORMAT format, int n_elm, struct vertattri *vattri );

void vbuffer_bind_vertattri ( struct vertattri *vattri, struct vertex_buffer *vbuffer );
void ibuffer_set_format ( enum INDEX_ELEMENT_FORMAT format, struct index_buffer *ibuffer );
void vbuffer_add_vertex ( struct vertex_buffer *vbuffer );
void ibuffer_add_index ( struct index_buffer *ibuffer );

#define vibuffer_add_element( _elm, _vibuffer ) \
{ \
	*(typeof(_elm) (_vibuffer)->curr) = *(_elm); \
	(_vibuffer)->curr += sizeof _elm; \
}


#endif // VIBUFFER_H_INCLUDED
