#include "calculate.h"
#include "log.h"

#define ADC_LEN 1024
#define FFT_LEN 1024
#define PEAK_GUARD 5
extern uint16_t ADC_UA[ADC_LEN];
extern uint16_t ADC_UB[ADC_LEN];
extern uint16_t ADC_UC[ADC_LEN];
extern float fs;
extern float FFT_mag[FFT_LEN];
extern float FFT_Input[FFT_LEN * 2];

SignalInfo A;
SignalInfo B;

void wavetypedetect(float *FFT_mag, float fs, SignalInfo *A, SignalInfo *B)
{
    float uc_amp;
    FFT_Process(ADC_UC, &uc_amp);

    // 找两个峰 最大和次大
    float max1 = 0.0f, max2 = 0.0f;
    uint16_t index1 = 0, index2 = 0;
    for (uint16_t i = 2; i < FFT_LEN / 2 - 1; i++)
    {
        if (FFT_mag[i] > FFT_mag[i - 1] && FFT_mag[i] > FFT_mag[i + 1])
        {
            if (FFT_mag[i] > max1)
            {
                max1 = FFT_mag[i];
                index1 = i;
            }
        }
    }
    for (uint16_t i = 2; i < FFT_LEN / 2 - 1; i++)
    {
        if (abs((int)i - (int)index1) <= PEAK_GUARD)
        {
            continue;
        }

        if (FFT_mag[i] > FFT_mag[i - 1] && FFT_mag[i] > FFT_mag[i + 1])
        {
            if (FFT_mag[i] > max2)
            {
                max2 = FFT_mag[i];
                index2 = i;
            }
        }
    }
    if (index1 < index2)
    {
        A->bin = index1;
        B->bin = index2;
    }
    else
    {
        A->bin = index2;
        B->bin = index1;
    }

    ADC_FFT_Get_Wave_Mes(A->bin, fs, &A->amp, &A->freq, 2);
    ADC_FFT_Get_Wave_Mes(B->bin, fs, &B->amp, &B->freq, 2);

    // 波形判断
    uint16_t a_3_index = A->bin * 3;
    uint16_t b_3_index = B->bin * 3;
    uint16_t a_5_index = A->bin * 5;
    uint16_t b_5_index = B->bin * 5;

    if (FFT_mag[a_3_index] / FFT_mag[A->bin] > 0.08f)
    {
        if (a_3_index == B->bin)
        {
            if (FFT_mag[a_5_index] / FFT_mag[A->bin] > 0.02f)
            {
                A->type = WAVE_TRIANGLE;
                B->type = WAVE_SINE;
                return;
            }
            else
            {
                A->type = WAVE_SINE;
                B->type = (FFT_mag[b_3_index] / FFT_mag[B->bin] > 0.08f) ? WAVE_TRIANGLE : WAVE_SINE;
                return;
            }
        }
        else
        {
            A->type = WAVE_TRIANGLE;
            B->type = WAVE_SINE;
            return;
        }
    }
    else if (FFT_mag[a_5_index] / FFT_mag[A->bin] > 0.02f)
    {    
        if (a_5_index == B->bin)
        {
            if (FFT_mag[a_3_index] / FFT_mag[A->bin] > 0.08f)
            {
                A->type = WAVE_TRIANGLE;
                B->type = WAVE_SINE;
                return;
            }
            else
            {
                A->type = WAVE_SINE;
                B->type = (FFT_mag[b_3_index] / FFT_mag[B->bin] > 0.08f) ? WAVE_TRIANGLE : WAVE_SINE;
                return;
            }
        }
        else if (a_5_index == B->bin * 3)
        {
            if (FFT_mag[a_3_index] / FFT_mag[A->bin] > 0.08f)
            {
                A->type = WAVE_TRIANGLE;
                B->type = WAVE_SINE;
                return;
            }
            else
            {
                A->type = WAVE_SINE;
                B->type = (FFT_mag[b_3_index] / FFT_mag[B->bin] > 0.08f) ? WAVE_TRIANGLE : WAVE_SINE;
                return;
            }
        }
        else
        {
            A->type = WAVE_SINE;
            B->type = (FFT_mag[b_3_index] / FFT_mag[B->bin] > 0.08f) ? WAVE_TRIANGLE : WAVE_SINE;
            return;
        }
    }
    else
    {
        A->type = WAVE_SINE;
        B->type = (FFT_mag[b_3_index] / FFT_mag[B->bin] > 0.08f) ? WAVE_TRIANGLE : WAVE_SINE;
        return;
    }
}

void test()
{
    wavetypedetect(FFT_mag, fs, &A, &B);
    LOG_INFO("WAVE", "result: A=%s %.1fHz %.4fV | B=%s %.1fHz %.4fV",
             A.type == WAVE_TRIANGLE ? "TRI" : "SIN", A.freq, A.amp,
             B.type == WAVE_TRIANGLE ? "TRI" : "SIN", B.freq, B.amp);
}
