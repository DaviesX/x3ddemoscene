#ifndef VERTATTRI_LERP_H_INCLUDED
#define VERTATTRI_LERP_H_INCLUDED


/* interpolators */
/* without perspective correction */
static void byte_interpolator ( int8_t *s, int8_t *e, float t, int8_t *r )
{
        r[0] = (int8_t) ((float) (*s) + (float) ((*e) - (*s))*t);
}

static void short_interpolator ( int16_t *s, uint16_t *e, float t, int16_t *r )
{
        r[0] = (int16_t) ((float) s[0] + (float) (e[0] - s[0])*t);
}

static void int_interpolator ( int32_t *s, int32_t *e, float t, int32_t *r )
{
        r[0] = (int32_t) ((float) s[0] + (float) (e[0] - s[0])*t);
}

static void int2_interpolator ( int32_t *s, int32_t *e, float t, int32_t *r )
{
        int i;
        for ( i = 0; i < 2; i ++ ) {
                r[i] = (int32_t) ((float) s[i] + (float) (e[i] - e[i])*t);
        }
}

static void int3_interpolator ( int32_t *s, int32_t *e, float t, int32_t *r )
{
        int i;
        for ( i = 0; i < 3; i ++ ) {
                r[i] = (int32_t) ((float) s[i] + (float) (e[i] - s[i])*t);
        }
}

static void int4_interpolator ( int32_t *s, int32_t *e, float t, int32_t *r )
{
        int i;
        for ( i = 0; i < 4; i ++ ) {
                r[i] = (int32_t) ((float) s[i] + (float) (e[i] - s[i])*t);
        }
}

static void float_interpolator ( float *s, float *e, float t, float *r )
{
        r[0] = s[0] + (e[0] - s[0])*t;
}

static void float2_interpolator ( float *s, float *e, float t, float *r )
{
        int i;
        for ( i = 0; i < 2; i ++ ) {
                r[i] = s[i] + (e[i] - s[i])*t;
        }
}

static void float3_interpolator ( float *s, float *e, float t, float *r )
{
        int i;
        for ( i = 0; i < 3; i ++ ) {
                r[i] = s[i] + (e[i] - s[i])*t;
        }
}

static void float4_interpolator ( float *s, float *e, float t, float *r )
{
        int i;
        for ( i = 0; i < 4; i ++ ) {
                r[i] = s[i] + (e[i] - s[i])*t;
        }
}

/* with perspective correction */
static void byte_interpolator_c ( int8_t *s, float s_invw, int8_t *e, float e_invw,
                                  float t, int8_t *r, float r_invw )
{
        float ts = (float) s[0]*s_invw;
        float te = (float) e[0]*e_invw;
        r[0] = (int8_t) ((ts + (te - ts)*t)/r_invw);
}

static void short_interpolator_c ( int16_t *s, float s_invw, uint16_t *e, float e_invw,
                                   float t, int16_t *r, float r_invw )
{
        float ts = (float) s[0]*s_invw;
        float te = (float) e[0]*e_invw;
        r[0] = (int16_t) ((ts + (te - ts)*t)/r_invw);
}

static void int_interpolator_c ( int32_t *s, float s_invw, int32_t *e, float e_invw,
                                 float t, int32_t *r, float r_invw )
{
        float ts = (float) s[0]*s_invw;
        float te = (float) e[0]*e_invw;
        r[0] = (int32_t) ((ts + (te - ts)*t)/r_invw);
}

static void int2_interpolator_c ( int32_t *s, float s_invw, int32_t *e, float e_invw,
                                  float t, int32_t *r, float r_invw )
{
        int i;
        for ( i = 0; i < 2; i ++ ) {
                float ts = (float) s[0]*s_invw;
                float te = (float) e[0]*e_invw;
                r[0] = (int32_t) ((ts + (te - ts)*t)/r_invw);
        }
}

static void int3_interpolator_c ( int32_t *s, float s_invw, int32_t *e, float e_invw,
                                  float t, int32_t *r, float r_invw )
{
        int i;
        for ( i = 0; i < 3; i ++ ) {
                float ts = (float) s[0]*s_invw;
                float te = (float) e[0]*e_invw;
                r[0] = (int32_t) ((ts + (te - ts)*t)/r_invw);
        }
}

static void int4_interpolator_c ( int32_t *s, float s_invw, int32_t *e, float e_invw,
                                  float t, int32_t *r, float r_invw )
{
        int i;
        for ( i = 0; i < 4; i ++ ) {
                float ts = (float) s[0]*s_invw;
                float te = (float) e[0]*e_invw;
                r[0] = (int32_t) ((ts + (te - ts)*t)/r_invw);
        }
}

static void float_interpolator_c ( float *s, float s_invw, float *e, float e_invw,
                                   float t, float *r, float r_invw )
{
        float ts = s[0]*s_invw;
        float te = e[0]*e_invw;
        r[0] = (ts + (te - ts)*t)/r_invw;
}

static void float2_interpolator_c ( float *s, float s_invw, float *e, float e_invw,
                                    float t, float *r, float r_invw )
{
        int i;
        for ( i = 0; i < 2; i ++ ) {
                float ts = s[i]*s_invw;
                float te = e[i]*e_invw;
                r[i] = (ts + (te - ts)*t)/r_invw;
        }
}

static void float3_interpolator_c ( float *s, float s_invw, float *e, float e_invw,
                                    float t, float *r, float r_invw )
{
        int i;
        for ( i = 0; i < 3; i ++ ) {
                float ts = s[i]*s_invw;
                float te = e[i]*e_invw;
                r[i] = (ts + (te - ts)*t)/r_invw;
        }
}

static void float4_interpolator_c ( float *s, float s_invw, float *e, float e_invw,
                                    float t, float *r, float r_invw )
{
        int i;
        for ( i = 0; i < 4; i ++ ) {
                float ts = s[i]*s_invw;
                float te = e[i]*e_invw;
                r[i] = (ts + (te - ts)*t)/r_invw;
        }
}

typedef void (*f_Interpolator) ( void *s, void *e, float t, void *r );
static const f_Interpolator VertIntOps[] = {
        [VERTEX_DEFN_BYTE] = (f_Interpolator)	byte_interpolator,
        [VERTEX_DEFN_SHORT] = (f_Interpolator)	short_interpolator,
        [VERTEX_DEFN_INT] = (f_Interpolator)	int_interpolator,
        [VERTEX_DEFN_INT2] = (f_Interpolator)	int2_interpolator,
        [VERTEX_DEFN_INT3] = (f_Interpolator)	int3_interpolator,
        [VERTEX_DEFN_INT4] = (f_Interpolator)	int4_interpolator,
        [VERTEX_DEFN_FLOAT] = (f_Interpolator)	float_interpolator,
        [VERTEX_DEFN_FLOAT2] = (f_Interpolator)	float2_interpolator,
        [VERTEX_DEFN_FLOAT3] = (f_Interpolator)	float3_interpolator,
        [VERTEX_DEFN_FLOAT4] = (f_Interpolator)	float4_interpolator
};

typedef void (*f_interpolator_c) ( void *s, float s_invw, void *e, float e_invw,
                                   float t, void *r, float r_invw );
static const f_interpolator_c VertIntCOps[] = {
        [VERTEX_DEFN_BYTE] = (f_interpolator_c)		byte_interpolator_c,
        [VERTEX_DEFN_SHORT] = (f_interpolator_c)	short_interpolator_c,
        [VERTEX_DEFN_INT] = (f_interpolator_c)		int_interpolator_c,
        [VERTEX_DEFN_INT2] = (f_interpolator_c)		int2_interpolator_c,
        [VERTEX_DEFN_INT3] = (f_interpolator_c)		int3_interpolator_c,
        [VERTEX_DEFN_INT4] = (f_interpolator_c)		int4_interpolator_c,
        [VERTEX_DEFN_FLOAT] = (f_interpolator_c)	float_interpolator_c,
        [VERTEX_DEFN_FLOAT2] = (f_interpolator_c)	float2_interpolator_c,
        [VERTEX_DEFN_FLOAT3] = (f_interpolator_c)	float3_interpolator_c,
        [VERTEX_DEFN_FLOAT4] = (f_interpolator_c)	float4_interpolator_c
};

static const int SizeOfComp[] = {
        [VERTEX_DEFN_BYTE] = 	1,
        [VERTEX_DEFN_SHORT] = 	2,
        [VERTEX_DEFN_INT] = 	4,
        [VERTEX_DEFN_INT2] = 	8,
        [VERTEX_DEFN_INT3] = 	12,
        [VERTEX_DEFN_INT4] = 	16,
        [VERTEX_DEFN_FLOAT] = 	4,
        [VERTEX_DEFN_FLOAT2] = 	8,
        [VERTEX_DEFN_FLOAT3] = 	12,
        [VERTEX_DEFN_FLOAT4] = 	16
};

/* without perspective correction */
static inline void lerp_attri ( int *type, int *offset, int n_comp,
                                void *s, void *e, float t, void *r )
{
        int i;
        for ( i = 0; i < n_comp; i ++ ) {
                int iop = vertdefn_mask ( type[i] );
                int mod = vertdefn_mod ( type[i] );
                if ( mod ) {
                        VertIntOps[iop] ( (untyped *) s + offset[i],
                                          (untyped *) e + offset[i],
                                          t,
                                          (untyped *) r + offset[i] );
                } else {
                        memcpy ( r, s + i, SizeOfComp[iop] );
                }
        }
}

/* with perspective correction */
static inline void lerp_attri_c ( int *type, int n_comp, int *offset,
                                  void *s, float s_invw, void *e, float e_invw,
                                  float t, void *r, float r_invw )
{
        int i;
        for ( i = 0; i < n_comp; i ++ ) {
                int iop = vertdefn_mask ( type[i] );
                int mod = vertdefn_mod ( type[i] );
                if ( mod ) {
                        if ( mod >> 1 ) {
                                /* using perspecive correction */
                                VertIntCOps[iop] ( (untyped *) s + offset[i], s_invw,
                                                   (untyped *) e + offset[i], e_invw,
                                                   t,
                                                   (untyped *) r + offset[i], r_invw );
                        } else {
                                /* do not use perspective correction */
                                /* using perspecive correction */
                                VertIntOps[iop] ( (untyped *) s + offset[i],
                                                  (untyped *) e + offset[i],
                                                  t,
                                                  (untyped *) r + offset[i] );
                        }
                } else {
                        memcpy ( (untyped *) r + offset[i], (untyped *) s + offset[i],
                                 SizeOfComp[iop] );
                }
        }
}


#endif // VERTATTRI_LERP_H_INCLUDED
