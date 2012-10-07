// ******************************************************************
// * 
// * proj : openLIBC
// *
// * desc : Totally Free LIC replacement
// *
// * file : math.h
// *
// * note : This LIBC is TOTALLY free - do what you like with it!!
// *
// ******************************************************************

#ifndef		__OPENXDK_MATH__
#define		__OPENXDK_MATH__

//here for now
#define OPENXDK_UNIMPLEMENTEDC(x) // x are not implemented in C!

#ifdef	__cplusplus
extern "C" {
#endif

//Double-precision functions

//trig
double sin(double);
double cos(double);
double tan(double);
double asin(double);
double acos(double);
double atan(double);
double atan2(double, double);

//power
double sqrt(double);
double pow(double, double); //approx.
double exp(double); //approx.
double log(double); //approx.
double log10(double); //approx
double hypot(double, double);

//rounding
double ceil(double);
double floor(double);
double fmod(double, double);
double fabs(double);

//Single-precision functions

//trig
float sinf(float);
float cosf(float);
float tanf(float);
float asinf(float);
float acosf(float);
float atanf(float);
float atan2f(float, float);

//power
float sqrtf(float);
float powf(float, float);//approx.
float expf(float);//approx.
float logf(float);//approx.
float log10f(float);//approx
float hypotf(float, float);

//rounding
float ceilf(float);
float floorf(float);
float fmodf(float, float);
float fabsf(float);

#ifdef	__cplusplus
};
#endif

#endif   //__OPENXDK_MATH__
