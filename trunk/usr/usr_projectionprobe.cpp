#include <usr/usr_projectionprobe.hpp>


namespace x3d
{
namespace usr
{

/* ProjectionProbe */
ProjectionProbe::ProjectionProbe(struct projection_probe* probe,
                                 enum OutputMethod method, void* target_screen,
                                 int width, int height, ColorMode colormode)
{
        m_probe = probe;
        projprobe_set_output_method(m_probe, method, target_screen);
        projprobe_set_output_format(m_probe, width, height, colormode);
}

ProjectionProbe::~ProjectionProbe()
{
        projprobe_free(m_probe);
}

void ProjectionProbe::set_output_method(enum OutputMethod method, void* target_screen)
{
        projprobe_set_output_method(m_probe, method, target_screen);
}

void ProjectionProbe::set_output_format(int width, int height, ColorMode colormode)
{
        projprobe_set_output_format(m_probe, width, height, colormode);
}

void ProjectionProbe::toggle_fullscreen(bool is_fullscreen)
{
        projprobe_toggle_fullscreen(m_probe, is_fullscreen);
}

void ProjectionProbe::set_position(struct point3d* p)
{
        projprobe_set_position(m_probe, p);
}

void ProjectionProbe::set_base(struct vector3d* u, struct vector3d* v, struct vector3d* n)
{
        projprobe_set_base(m_probe, u, v, n);
}

void ProjectionProbe::rotate(float x, float y, float z)
{
        projprobe_rotate(m_probe, x, y, z);
}

void ProjectionProbe::rotate_relative(float x, float y, float z)
{
        projprobe_rotate_relative(m_probe, x, y, z);
}

void ProjectionProbe::move(float x)
{
        projprobe_move(m_probe, x);
}

void ProjectionProbe::move_relative(float x)
{
        projprobe_move_relative(m_probe, x);
}

void ProjectionProbe::transform_base(struct matrix4x4* transform)
{
        projprobe_transform_base(m_probe, transform);
}

void ProjectionProbe::transform_base_relative(struct matrix4x4* transform)
{
        projprobe_transform_base_relative(m_probe, transform);
}

/* PerspectiveProbe */
PerspectiveProbe::PerspectiveProbe(enum OutputMethod method, void* target_screen, int width, int height, ColorMode colormode) :
        ProjectionProbe((struct projection_probe*) (persprobe_create(method, target_screen)),
                        method, target_screen, width, height, colormode)
{
        m_probe = (struct perspective_probe*) ((ProjectionProbe*) (this))->m_probe;
        persprobe_set_range(m_probe, width, height, 2000.0f);
        persprobe_set_optics(m_probe, 35.0f, 200.0f, 35.0f/8.0f);
}

PerspectiveProbe::~PerspectiveProbe()
{
        m_probe = nullptr;
}

void PerspectiveProbe::set_optics(float focal_length, float v_dist, float aperture)
{
        persprobe_set_optics(m_probe, focal_length, v_dist, aperture);
}

void PerspectiveProbe::set_direction(struct vector3d* up, struct point3d* target)
{
        persprobe_set_direction(m_probe, up, target);
}

void PerspectiveProbe::set_range(float x, float y, float z)
{
        persprobe_set_range(m_probe, x, y, z);
}

/* OrthogonalProbe*/
OrthogonalProbe::OrthogonalProbe(enum OutputMethod method, void* target_screen, int width, int height, ColorMode colormode) :
        ProjectionProbe((struct projection_probe*) orthoprobe_create(method, target_screen),
                        method, target_screen, width, height, colormode)
{
}

OrthogonalProbe::~OrthogonalProbe()
{
}

}// namespace usr

}// namespace x3d
