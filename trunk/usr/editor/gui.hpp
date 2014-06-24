#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include <gtk/gtk.h>

enum {
        X_EDITOR_RUN,
        X_EDITOR_FREEZE,
        X_EDITOR_DESTROY
};

enum X_EDITOR_MODE {
        X_EDITOR_DEMO_MODE,
        X_EDITOR_EDIT_MODE
};

struct common_data {
        GdkPixbuf *logo_pix_buf;
        GtkWidget *test_region;		/* for debuging */
        void *tmp_image;
        int tmp_image_w;
        int tmp_image_h;
        enum X_EDITOR_MODE ed_mode;
};

struct path_resource {
        char *glade_path;
        char *logo_file;
        char *raw_media_dir;
        char *config_dir;
};


static const struct path_resource g_path_res = {
        .glade_path = (char *) "./etc/editor/",
        .logo_file = (char *) "./etc/editor/x3d_logo.png",
        .raw_media_dir = (char *) "./etc/media/",
        .config_dir = (char *) "./etc/config/"
};
extern struct common_data g_comm_data;

/*
 * functions' declaration
 */
char *file_chooser_open ( char *default_dir );
char *file_chooser_save ( char *default_dir );
void message_box_info ( char *title, char *message );
void message_box_warning ( char *title, char *message );
void message_box_error ( char *title, char *message );
bool message_box_question ( char *title, char *message );

void widget_get_size ( GtkWidget *parent, GtkWidget *widget,
                       int *x, int *y, int *width, int *height );
GtkBuilder *builder_load ( char *filename );
void builder_all_set ( GtkBuilder *builder );


#endif // GUI_H_INCLUDED
