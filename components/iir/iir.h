#ifndef __IIR_H
#define __IIR_H

//Direct-Form II, Second-Order Sections
// s(t) = a0 * [x(t) - a1*s(t-1) - a2*s(t-2)];
// y(t) = gain * [b0*s(t) + b1*s(t-1) + b2*s(t-2)];

/*
    x(t)------+----a0---s(t)----b0---+---gain-->y(t)
           (-)|         |            |(+)
              |<---a1---s(t-1)--b1-->|
              |         |            |
              |<---a2---s(t-2)--b2-->|
*/


#include <stdint.h>


#define SECTION_NUM ((5-1)/2)

typedef struct _iir_dirii_secord_section_t
{
    float gain;
    float a[3];
    float b[3];
    float s[3];
}iir_dirii_secord_section_t;

typedef struct _iir_filter_handle_t
{
   iir_dirii_secord_section_t section[SECTION_NUM];
   float total_gain;
}iir_filter_handle_t;


#ifdef __cplusplus
 extern "C" {
#endif 

int32_t iir_init(iir_filter_handle_t* handle);
int32_t iir_filter(iir_filter_handle_t *handle, float input, float *output);



#ifdef __cplusplus
}
#endif



#endif /* __IIR_H */
