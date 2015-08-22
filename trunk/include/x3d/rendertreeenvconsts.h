#ifndef RENDERTREEENVCONSTS_H_INCLUDED
#define RENDERTREEENVCONSTS_H_INCLUDED

/*
 * consts
 */
/* The following rendering method is expected to engage in the renderer */
/** \brief available renderer type
 */
enum PreferredRendererType {
        RendererUndeterminate,          /**< a memory block with undefined renderer */
        RendererRasterizer,             /**< rasterization renderer */
        RendererPathTracer,             /**< path tracing renderer */
        RendererPhotonTracer,           /**< photon tracing renderer */
        RendererPhotonMap,              /**< photon light map generation renderer */
        RendererRadiosity,              /**< radiosity light map generation renderer */
        RendererRadianceCache,          /**< radiance cache generation renderer */
        RendererPRT,                    /**< precomputed radiance transfer renderer */
        RendererSelection,              /**< object selection renderer */
        c_NumRendererType
};


const int LightModelDirect      = 0X1;
const int LightModelShadow      = 0X2;
const int LightModelLightMap    = 0X4;
const int LightModelSHProbe     = 0X8;
const int LightModelSVO         = 0X10;

enum GeometryModelType {
        GeometryModelWireframe,
        GeometryModelSolid
};

/* Renderer should not handle threading from these constants itself,
 * this are the states telling which situation
 * the renderer is being created with */
const int RenderThreadMutual            = 0X0;
const int RenderThreadSeparated         = 0X1;
/* These are what renderer expected to behave internally */
const int RenderThreadSingle            = 0X2;
const int RenderThreadMultiple          = 0X4;

enum RenderSpecType {
        RenderSpecSWBuiltin,
        RenderSpecHWOpenGL,
        RenderSpecHWDirectX
};

enum RenderEnvironment {
        RenderEnvVoid,
        RenderEnvSpec,
        RenderEnvProbe,
        RenderEnvRenderable,
        RenderEnvRenderOut,
        RenderEnvThread,
        RenderEnvAntialias,
        RenderEnvFiltering,
        RenderEnvGeometryModel,
        RenderEnvPreferredRenderer,
        c_NumRenderEnviornmentType
};


#endif // RENDERTREEENVCONSTS_H_INCLUDED
