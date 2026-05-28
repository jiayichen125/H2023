#include "myfun.h"

void wavegenerate(SignalInfo *A, SignalInfo *B)
{
    ad9833_init();
    ad9833_sync_start(A->freq, A->type, B->freq, B->type);
}