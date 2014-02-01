#ifndef DBG_VERTPROCESSOR_H_INCLUDED
#define DBG_VERTPROCESSOR_H_INCLUDED


/* dbg interface */
extern int process_point ( rt_vertex *vi,
			   struct matrix4x4 *t_all, int vert_size,
			   rt_vertex *vo );
extern int process_line ( rt_vertex *vi,
			  struct matrix4x4 *t_all,
			  int *comp_offset, int *comp_format, int n_comp,
			  int vert_size, rt_vertex *vo );
extern int process_triangle ( rt_vertex *vi,
			      struct matrix4x4 *t_view, struct matrix4x4 *t_proj,
			      float cull_factor,
			      int *comp_offset, int *comp_format, int n_comp,
			      int vert_size, rt_vertex *cache, rt_vertex *vo );


#endif // DBG_VERTPROCESSOR_H_INCLUDED
