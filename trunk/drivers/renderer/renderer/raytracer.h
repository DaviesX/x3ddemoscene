#ifndef RAYTRACER_H_INCLUDED
#define RAYTRACER_H_INCLUDED


struct raytracer {
        int                     nthreads;
        int                     nsamples;
        int                     nprobes;
        void*                   probe_buf;
        struct shader_attribute probe_attr;
        struct triangle_buffer  tribuf;
        struct shader_attribute vbo_attr;
        struct shader_form      emitter;
        struct shader_form      bounce;
        struct shader_form      transfer;
};

/*
 * functions' declaration
 */
void raytracer_run ( struct raytracer* r );


#endif // RAYTRACER_H_INCLUDED
