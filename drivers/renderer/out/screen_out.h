#ifndef SCREEN_OUT_H_INCLUDED
#define SCREEN_OUT_H_INCLUDED


struct render_out;

/* functions' declaration */
void ro_screen_retrieve ( void *data, int size, struct render_out *ro );
void ro_screen_free ( struct render_out *ro );
void ro_screen_run ( struct render_out *ro );


#endif // SCREEN_OUT_H_INCLUDED
