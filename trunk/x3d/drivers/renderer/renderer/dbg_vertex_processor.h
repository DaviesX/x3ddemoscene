#ifndef DBG_VERTPROCESSOR_H_INCLUDED
#define DBG_VERTPROCESSOR_H_INCLUDED


/* dbg interface */
typedef int (*f_Process_Point) ( rt_vertex *vi,
				 struct matrix4x4 *t_all, int vert_size,
				 rt_vertex *vo );
typedef int (*f_Process_Line) ( rt_vertex *vi,
				struct matrix4x4 *t_all,
				int *comp_offset, int *comp_format, int n_comp,
				int vert_size, rt_vertex *vo );
typedef int (*f_Process_Triangle) ( rt_vertex *vi,
				    struct matrix4x4 *t_view, struct matrix4x4 *t_proj,
				    float cull_factor,
				    int *comp_offset, int *comp_format, int n_comp,
				    int vert_size, rt_vertex *vo );

f_Process_Point dbg_process_point;
f_Process_Line dbg_process_line;
f_Process_Triangle dbg_process_triangle;


#endif // DBG_VERTPROCESSOR_H_INCLUDED
