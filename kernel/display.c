#include <gtk/gtk.h>
#include <x3d/display.h>


/*
 * <display> public
 */
void display_init(struct display* self, f_Display_Host_Image f_display_host, f_Display_Free f_free)
{
        self->f_display_host = f_display_host;
        self->f_free = f_free;
}

void display_free(struct display* self)
{
        self->f_free(self);
}

void display_host_image(struct display* self, struct util_image* image)
{
        self->f_display_host(self, image);
}


/*
 * <display_gtk_host> public
 */
struct display_gtk_host* display_gtk_host_create(GtkWidget* widget)
{
        struct display_gtk_host* self = alloc_obj(self);
        return self;
}

void display_gtk_host_free(struct display_gtk_host* self)
{
}

void display_gtk_host_display_image(struct display_gtk_host* self, struct util_image* image)
{
}
