#ifndef IRENDERER_H_INCLUDED
#define IRENDERER_H_INCLUDED


/*
 * Structures
 */
struct irenderer {
        enum RENDERER_IDR idr;
        enum RENDERER_THREAD_STATE_IDR thr_state;
        enum RENDER_SPEC_IDR spec;

};

/*
 * functions' declaration
 */
void init_irenderer (
        enum RENDERER_IDR idr, enum RENDERER_THREAD_STATE_IDR thr_state,
        enum RENDER_SPEC_IDR spec, void *_rend );
void free_irenderer ( void *_rend );


#endif // IRENDERER_H_INCLUDED
