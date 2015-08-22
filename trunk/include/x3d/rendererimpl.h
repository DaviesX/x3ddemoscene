#ifndef RENDERERIMPL_H_INCLUDED
#define RENDERERIMPL_H_INCLUDED


struct renderer;
struct render_node;

/* function interfaces */
typedef struct lcrenderer*              (*f_LCRenderer_Create) ();
typedef void (*f_LCRenderer_Free)       (struct lcrenderer* renderer);
typedef void (*f_LCRenderer_Output)     (struct lcrenderer* renderer);

struct renderer {
        f_LCRenderer_Create     creator;
        f_LCRenderer_Free       free;
        f_LCRenderer_Output     output;
};

enum RendererRegistration {
        RendererCreate,
        RendererFree,
        RendererOutput,
        c_NumRendererRegistration
};


void renderer_register_interface(struct renderer* renderer, enum RendererRegistration interface,


#endif // RENDERERIMPL_H_INCLUDED
