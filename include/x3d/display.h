#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

#include <x3d/utility.h>

struct display;

typedef void (*f_Display_Host_Image) (struct display* self, struct util_image* image);
typedef void (*f_Display_Free) (struct display* self);
/*
 * <display> decl
 */
struct display {
        f_Display_Host_Image f_display_host;
        f_Display_Free       f_free;
};
/*
 * <display> public
 */
void display_init(struct display* self, f_Display_Host_Image f_display_host, f_Display_Free f_free);
void display_free(struct display* self);
void display_host_image(struct display* self, struct util_image* image);


typedef struct _GtkWidget GtkWidget;

/*
 * <display_gtk_host> decl
 */
struct display_gtk_host {
        struct display  _parent;
        GtkWidget*      widget;
};
/*
 * <display_gtk_host> public
 */
struct display_gtk_host* display_gtk_host_create(GtkWidget* widget);
void display_gtk_host_free(struct display_gtk_host* self);
void display_gtk_host_display_image(struct display_gtk_host* self, struct util_image* image);


#endif // DISPLAY_H_INCLUDED
