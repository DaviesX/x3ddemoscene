#ifndef RENDERNODEHDR_H_INCLUDED
#define RENDERNODEHDR_H_INCLUDED

/*
 * consts
 */
enum RenderNodeHdrType {
        RenderNodeHdrLinear,
        RenderNodeHdrFilmic
};

/*
 * structures
 */
struct render_filmic_hdr {
        struct render_node      _node;
        enum RenderNodeHdrType  hdrtype;
        float                   A;// = 0.15f;
        float                   B;// = 0.50f;
        float                   C;// = 0.10f;
        float                   D;// = 0.20f;
        float                   E;// = 0.02f;
        float                   F;// = 0.30f;
        float                   W;// = 11.2f;
        float                   key;
        float                   exp;
};


#endif // RENDERNODEHDR_H_INCLUDED
