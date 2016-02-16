#ifndef RENDERENVCONSTS_H_INCLUDED
#define RENDERENVCONSTS_H_INCLUDED

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


static const int LightModelDirect      = 0X1;
static const int LightModelShadow      = 0X2;
static const int LightModelLightMap    = 0X4;
static const int LightModelSHProbe     = 0X8;
static const int LightModelSVO         = 0X10;

enum GeometryModelType {
        GeometryModelWireframe,
        GeometryModelSolid
};

/* Renderer should not handle threading from these constants itself,
 * this are the states telling which situation
 * the renderer is being created with */
static const int RenderThreadMutual            = 0X0;
static const int RenderThreadSeparated         = 0X1;
/* These are what renderer expected to behave internally */
static const int RenderThreadSingle            = 0X2;
static const int RenderThreadMultiple          = 0X4;

enum RenderSpecType {
        RenderSpecSWBuiltin,
        RenderSpecSWAdvBuiltin,
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


#endif // RENDERENVCONSTS_H_INCLUDED
