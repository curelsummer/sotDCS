#include <stddef.h>
#include "waveform.h"
#include "tdcs_gseq.h"
#include "tacs_gseq.h"
#include "ces_gseq.h"

sequence_t* generate_secquence(waveform_t *wave)
{
    if(wave == NULL)
    {
        return NULL;
    }

    
    switch(wave->id)
    {
        case TDCS_WAVE_ID:
            return tdcs_generate_secquence(wave);

        case TACS_WAVE_ID:
            return tacs_generate_secquence(wave);
            
        case CES_WAVE_ID:
            return ces_generate_secquence(wave);

        // case WAVE_ID_RTACS:
        //     return gen_seq_rtacs(wave);

        default:
            return NULL;
    }
}