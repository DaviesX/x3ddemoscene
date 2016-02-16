#ifndef X3DDECIMAL_H_INCLUDED
#define X3DDECIMAL_H_INCLUDED


/*
 * Defintions
 */
/* Utility constants for operation of fix point number */
#define FIXP16_SHIFT			16
#define FIXP16_MAGICAL			(float) 65536
#define FIXP16_DECIMAL_MASK		0X0000FFFF
#define FIXP16_WHOLE_MASK		0XFFFF0000
#define FIXP16_ROUND_UP			0X00008000

#define FIXP20_SHIFT			(float) 20
#define FIXP20_MAGICAL			1048575
#define FIXP20_DECIMAL_MASK		0X00FFFFFF
#define FIXP20_WHOLE_MASK		0X00000FFF
#define FIXP20_ROUND_UP			0X00080000

#define FIXP28_SHIFT			(float) 28
#define FIXP28_MAGICAL			268435456
#define FIXP28_DECIMAL_MASK		0X7FFFFFFF
#define FIXP28_WHOLE_MASK		0X00000003
#define FIXP28_ROUND_UP			0x08000000


/* convert fixed point to float */
#define fp16_16_tf(fp)			(((float) fp)/FIXP16_MAGICAL)
#define fp4_28_tf(fp)			(((float) fp)/FIXP28_MAGICAL)
#define fp12_20_tf(fp)			(((float) fp)/FIXP20_MAGICAL)

/* extract the int part and float part of a fixed point */
#define fp16_16i(fp)			(fp >> FIXP16_SHIFT)
#define fp16_16f(fp)			(fp & FIXP16_DECIMAL_MASK)
#define fp4_28i(fp)			(fp >> FIXP28_SHIFT)
#define fp4_28f(fp)			(fp & FIXP28_DECIMAL_MASK)
#define fp12_20i(fp)			(fp >> FIXP20_SHIFT)
#define fp12_20f(fp)			(fp & FIXP20_DECIMAL_MASK)

/* Convert integer and float to fixed-point number */
#define i_fp16_16(_int)				(fp16_16_t) ((_int) << FIXP16_SHIFT)
#define f_fp16_16(_dec)				(fp16_16_t) ((float) (_dec)*FIXP16_MAGICAL)
#define i_fp4_28(_int)				(fp4_28_t) (_int) << FIXP28_SHIFT)
#define f_fp4_28(_dec)				(fp4_28_t) ((float) (_dec)*FIXP28_MAGICAL)
#define i_fp12_20(_int)				(fp12_20_t) (_int << FIXP20_SHIFT)
#define f_fp12_20(_dec)				(fp12_20_t) ((float) (_dec)*FIXP20_MAGICAL)



/*
 * fixed point types
 */
typedef int fp16_16_t;
typedef int fp4_28_t;
typedef int fp12_20_t;


/*
 * functions' definition
 */
fp16_16_t mul_fp16_16 ( fp16_16_t f0, fp16_16_t f1 );
fp16_16_t div_fp16_16 ( fp16_16_t f0, fp16_16_t f1 );
float sci_to_float ( float b, int exp );


#endif // X3DFIXEDPOINT_H_INCLUDED
