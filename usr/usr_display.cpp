#include <usr/usr_display.hpp>


namespace x3d
{
namespace usr
{
Display::Display(struct display* display) : m_display(display)
{
}

struct display* Display::get_core_resource()
{
        return m_display;
}

DisplayGtkHost::DisplayGtkHost(GtkWidget* widget) : Display(&display_gtk_host_create(widget)->_parent)
{
}

}

}
