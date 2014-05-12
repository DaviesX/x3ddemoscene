/* main.c: All maths unit tests go here */
#include <math/math.h>
#include "main.h"


// Test FCompareRelOrC function
void ErrorCompareTest ( void )
{
        float a = 0.333333f,
              b = 1.0f/3.0f;

        if ( fcmp_rel_lc ( a, b, EPSILON_E5 ) ) {
                printf ( "equal\n" );
        }// End If

}// End Function ErrorCompareTest

// Test the performance and precision of using precomputed table
void PowTableFunctionTest ( void )
{
        float err = 0.0f;

        init_math_lib ();

        int i, j;
        for ( j = 0; j < 10; j ++ )
                for ( i = 0; i < 10000000; i ++ ) {
                        float base = (rand () % 99999);
//		float power = (rand () % 99999)/100000.0f;
//		float res0 = powf ( 0.23564f, 0.76543f );
//		float res1 = FastPow ( 0.23564f, 0.76543f );
                        float res0 = 0.0f;
                        float res1 = sqrtf ( base );
                        err += fabs ( res1 - res0 );

                }// End For

        printf ( "%f\n", err );

}// End Function PowTableFunctionTest
