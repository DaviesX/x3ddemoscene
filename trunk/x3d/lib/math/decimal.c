/* decimal.c: Handling decimal fraction */
#include <math.h>
#include <stdlib.h>
#include "decimal.h"



fp16_16_t mul_fp16_16 ( fp16_16_t f0, fp16_16_t f1 )
{
	asm
	(
		"movl %0, %%eax 	\n"
		"imul %%eax, %1		\n"
		"shrd $16, %%edx, %%eax	\n"
		: /* no output */
		: "r"(f0), "r"(f1)
	);
}

fp16_16_t div_fp16_16 ( fp16_16_t f0, fp16_16_t f1 )
{
	asm (
		"movl %0, %%eax		\n"
		"cdq			\n"
		"shld $16, %%edx, %%eax \n"
		"sal $16, %%eax		\n"
		"idiv %1		\n"
		: /* no output */
		: "r"(f0), "r"(f1)
	);
}

float sci_to_float ( float b, int exp )
{
	int i_exp = abs ( exp );
	float r = 1.0f;
	int i;
	for ( i = 0; i < i_exp; i ++ ) {
		r *= 10.0f;
	}
	if ( exp < 0 ) {
		return b/r;
	} else {
		return b*r;
	}
}
