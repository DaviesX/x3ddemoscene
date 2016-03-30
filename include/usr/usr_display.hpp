#ifndef USR_DISPLAY_HPP_INCLUDED
#define USR_DISPLAY_HPP_INCLUDED

#include <usr/usr_x3d.hpp>

namespace x3d
{
namespace usr
{

class Display : public CoreResource<struct display> {
public:
        Display(struct display* display);
        struct display* get_core_resource();
private:
        struct display* m_display;
};

class DisplayGtkHost : public Display {
public:
        DisplayGtkHost(GtkWidget* widget);
private:

};

}// namespace usr

}// namespace x3d

#endif // USR_DISPLAY_HPP_INCLUDED
