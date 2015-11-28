#ifndef RENDERTARGET_H_INCLUDED
#define RENDERTARGET_H_INCLUDED

#include <x3d/renderenvconsts.h>

enum RenderTargetType {
        RenderTargetColorRGBA32,
        c_NumRenderTargetType
};

/*
 * <render_target> decl
 */
struct render_target {
};
/*
 * <render_target> public
 */
struct render_target* rendt_factory_new(enum RenderSpecType spec);
void rendt_init(struct render_target* self);
void rendt_free(struct render_target* self);
void rendt_set_format(struct render_target* self, enum RenderTargetType type);
void rendt_set_size(struct render_target* self, int width, int height);


#endif // RENDERTARGET_H_INCLUDED
