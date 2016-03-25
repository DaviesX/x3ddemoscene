#ifndef FX_H_INCLUDED
#define FX_H_INCLUDED



/*
 * Functions defintion
 */
void init_sincos_table();
void init_pow_table();
void init_log2_table();
float fast_sin_m(float theta);
float fast_sin(float theta);
float fast_cos_m(float theta);
float fast_cos(float theta);
float fast_pow(float x, float y);
float fast_sqrt(float a);
float lerp_y_x(struct point2d *p0, struct point2d *p1, float x);
float lerp_y_t(struct point2d *p0, struct point2d *p1, float t);
int solve_quadratic(float a, float b, float c, float *x0, float *x1);
int log2_int_c(int x);
int log2_int_t(int x);
int fast_log2(int x);
float arctan2f(float y, float x);

#define __shuffle_index(a, b)      (a + rand()%(b - a))

#define shuffle(_arr, _n)                               \
{                                                       \
        int _i;                                         \
        for (_i = 0; _i <= (_n) - 2; _i ++) {           \
                int _j = __shuffle_index(0, (_n) - _i); \
                swap((_arr)[_i], (_arr)[_i + _j]);      \
        }                                               \
}

#define uniform0_1()                    ((double) rand()/RAND_MAX)


#endif // FX_H_INCLUDED
