#include <stdio.h>
#include <stdlib.h>

#define H  0.01
#define ORD 2

#define DEBUG 0

double PI;
int math_initialized = 0;

void ensure_math_init()
{
    if (!math_initialized)
    {
        math_init();
        math_initialized = 1;
    }
}

typedef void (*sys_f)(double t, double *y, double *dy, void *ctx);

double m_abs(double x) { return x < 0 ? -x : x; }

void solve(double t, double t_end, double *y, int dim, sys_f f, void *ctx) {
    double k1[ORD], k2[ORD], k3[ORD], k4[ORD], yt[ORD];
    double dt = t_end - t;
    int n = (int)(m_abs(dt) / H);
    double h = (dt < 0) ? -H : H;

    for (int step = 0; step <= n; step++) {
        // Remainder handl
        if (step == n) {
            h = t_end - t;
            if (m_abs(h) < 1e-9) break;
        }

        f(t, y, k1, ctx);
        for(int i=0; i<dim; i++) yt[i] = y[i] + k1[i] * h * 0.5;
        
        f(t + h*0.5, yt, k2, ctx);
        for(int i=0; i<dim; i++) yt[i] = y[i] + k2[i] * h * 0.5;
        
        f(t + h*0.5, yt, k3, ctx);
        for(int i=0; i<dim; i++) yt[i] = y[i] + k3[i] * h;
        
        f(t + h, yt, k4, ctx);
        
        for(int i=0; i<dim; i++) 
            y[i] += (h/6.0) * (k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);
        
        t += h;
    }
}

void d_sin(double t, double *y, double *dy, void *ctx) {
    dy[0] = y[1];  // y' = z
    dy[1] = -y[0]; // z' = -y
}

void d_ln(double t, double *y, double *dy, void *ctx) {
    dy[0] = 1.0 / t;
}

void d_pow(double t, double *y, double *dy, void *ctx) {
    double w = *(double*)ctx;
    dy[0] = (w / t) * y[0];
}

// my funcs

float Q_sqrt(float n) {
    uint32_t; float x2, y; const float th = 1.5F;
    x2 = n * 0.5F; y = n;
    i = * ( long * ) &y;
    i = 0x5f3759df - ( i >> 1 );
    y = * ( float * ) &i;
    return y * ( th - ( x2 * y * y ) );
}
double m_sqrt(double n) {
    if (n < 0) return 0.0/0.0; // what to return
    return 1.0 / Q_sqrt((float)n);
}

// had to put here cuz m_sqrt needed
//void d_asin(double t, double *y, double *dy, void *ctx) {
//    dy[0] = 1.0/(m_sqrt((1-(t*t))));
//}

void d_atan(double t, double *y, double *dy, void *ctx) {
    dy[0] = 1.0/(1+t*t);
}

void d_exp(double t, double *y, double *dy, void *ctx) {
    dy[0] = y[0];
}

double m_atan(double x) {
    double y[1] = {0.0};
    solve(0,x,y,1,d_atan,NULL);
    return y[0];
}
void math_init()
{
    PI = 4.0 * m_atan(1.0);

}
double m_asin(double x)
{
    ensure_math_init();

    if (m_abs(x) > 1.0)
        return 0.0 / 0.0;

    if (m_abs(x - 1.0) < 1e-12)
        return PI / 2.0;

    if (m_abs(x + 1.0) < 1e-12)
        return -PI / 2.0;

    return m_atan(x * Q_sqrt((float)(1.0 - x * x)));
}
double m_acos(double x)
{
    ensure_math_init();
    return PI / 2.0 - m_asin(x);
}
double m_exp(double x) {
    double y[1] = {1.0};
    solve (0,x,y,1,d_exp,NULL);
    return y[0];
}
double m_sin(double x)
{
    ensure_math_init();

    while (x > PI)
        x -= 2.0 * PI;

    while (x < -PI)
        x += 2.0 * PI;

    double y[2] = {0.0, 1.0};

    solve(0.0, x, y, 2, d_sin, NULL);

    return y[0];

}
double m_cos(double x)
{
    ensure_math_init();

    while (x > PI)
        x -= 2.0 * PI;

    while (x < -PI)
        x += 2.0 * PI;

    double y[2] = {1.0, 0.0};

    solve(0.0, x, y, 2, d_sin, NULL);

    return y[0];
}
double m_tan(double x)
{
    double c = m_cos(x);

    if (m_abs(c) < 1e-12)
    {
        if (m_sin(x) >= 0)
            return 1.0 / 0.0;
        else
            return -1.0 / 0.0;
    }

    return m_sin(x) / c;
} //y' = y^2+1, do this once. rn this is 4x more computations than normal!

double m_ln(double x) {
    if (x <= 0) return -1.0/0.0;
    double y[1] = {0};
    solve(1, x, y, 1, d_ln, NULL);
    return y[0];
}
double m_log10(double x) {
    return m_ln(x) / m_ln(10.0);
}

double m_pow(double x, double w) {
    double y[1] = {1};
    solve(1, x, y, 1, d_pow, &w);
    return y[0];
}

double m_fact(double n) {
    return (n <= 1) ? 1 : n * m_fact(n - 1);
}
