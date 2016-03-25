#ifndef MATERIAL_H_INCLUDED
#define MATERIAL_H_INCLUDED

#include <x3d/bsdf.h>
#include <x3d/colorspectrum.h>

struct material;

typedef struct bsdf_model* (*f_Material_BSDF) (struct material* self);

/*
 * <material> decl
 */
struct material {
        struct float_color3     refl;
        struct float_color3     roughness;
};


#endif // MATERIAL_H_INCLUDED
