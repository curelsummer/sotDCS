#include <stdint.h>
#include <stdio.h>
#include "fdacoefs.h"
#include "iir.h"
static float second_order_filter(iir_dirii_secord_section_t *section, float input);

int32_t iir_init(iir_filter_handle_t* handle)
{
    if(handle == NULL)
    {
        return -1;
    }

    uint32_t i;
    for(i=0; i<SECTION_NUM; i++)
    {
        //printf("section %u:\n", i);

        handle->section[i].gain = NUM[2*i][0];

        handle->section[i].a[0] = DEN[2*i+1][0];
        handle->section[i].a[1] = DEN[2*i+1][1];
        handle->section[i].a[2] = DEN[2*i+1][2];

        handle->section[i].b[0] = NUM[2*i+1][0];
        handle->section[i].b[1] = NUM[2*i+1][1];
        handle->section[i].b[2] = NUM[2*i+1][2];

        handle->section[i].s[0] = 0;
        handle->section[i].s[1] = 0;
        handle->section[i].s[2] = 0;

        // printf("\tgain: %.4f\n", handle->section[i].gain);
        // printf("\ta: %.4f\t%.4f\t%.4f\n", handle->section[i].a[0], handle->section[i].a[1], handle->section[i].a[2]);
        // printf("\tb: %.4f\t%.4f\t%.4f\n", handle->section[i].b[0], handle->section[i].b[1], handle->section[i].b[2]);
    }

    handle->total_gain = NUM[2*i][0];
    // printf("total gain: %.4f\n\n", handle->total_gain);

    return 0;
}

int32_t iir_filter(iir_filter_handle_t *handle, float input, float *output)
{
    if(handle == NULL)
    {
        return -1;
    }

    float result = second_order_filter(&handle->section[0], input);
    for(uint32_t i=1; i<SECTION_NUM; i++)
    {
        result = second_order_filter(&handle->section[i], result);
    }

    result = result * handle->total_gain;

    *output = result;

    return 0;

}

static float second_order_filter(iir_dirii_secord_section_t *section, float input)
{
    float result;

    section->s[0] = section->a[0] * (input - (section->a[1] * section->s[1]) - (section->a[2] * section->s[2]));
    
    result = section->b[0] * section->s[0] + section->b[1] * section->s[1] + section->b[2] * section->s[2];

    result = result * section->gain;

    section->s[2] = section->s[1];

    section->s[1] = section->s[0];

    return result;
}