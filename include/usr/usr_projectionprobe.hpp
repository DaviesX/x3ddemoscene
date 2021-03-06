#ifndef USR_PROJECTIONPROBE_HPP_INCLUDED
#define USR_PROJECTIONPROBE_HPP_INCLUDED


#include <usr/usr_x3d.hpp>

namespace x3d
{
namespace usr
{

class Display;

/** \brief Projection Probe
 */
class ProjectionProbe : public CoreResource<struct projection_probe> {
        friend class PerspectiveProbe;
        friend class OrthogonalProbe;
private:
        ProjectionProbe(struct projection_probe* probe);
public:
        ~ProjectionProbe();
        void                            set_position(struct point3d* p);
        void                            set_base(struct vector3d* u, struct vector3d* v, struct vector3d* n);
        void                            rotate(float x, float y, float z);
        void                            rotate_relative(float x, float y, float z);
        void                            move(float x);
        void                            move_relative(float x);
        void                            transform_base(struct matrix4x4* transform);
        void                            transform_base_relative(struct matrix4x4* transform);

        struct projection_probe*        get_core_resource();
private:
        struct projection_probe*        m_probe;
};

/** \brief Perspective Probe
 */
class PerspectiveProbe : public ProjectionProbe {
public:
        PerspectiveProbe(Display* display, int xres, int yres);
        virtual ~PerspectiveProbe();

        void            set_optics(float focal_length, float v_dist, float aperture);
        void            set_direction(struct vector3d* up, struct point3d* target);
        void            set_range(float x, float y, float z);

private:
        struct perspective_probe*       m_probe;
};

class OrthogonalProbe : public ProjectionProbe {
public:
        OrthogonalProbe(Display* display, int xres, int yres);
        virtual ~OrthogonalProbe();
};

}// namespace usr

}// namespace x3d


#endif // USR_PROJECTIONPROBE_HPP_INCLUDED
