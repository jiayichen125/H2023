#include "calculate.h"

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

    //float a_3_ratio = FFT_mag[a_3_index] / FFT_mag[A->bin];
    //if (a_3_ratio > 0.08f)
    //{
    //    A->type = WAVE_TRIANGLE;
    //    B->type = WAVE_SINE;
    //}
    //else
    //{
    //    if (B->bin * 3 < FFT_LEN / 2 && FFT_mag[b_3_index] / FFT_mag[B->bin] > 0.08f)
    //    {
    //        A->type = WAVE_SINE;
    //        B->type = WAVE_TRIANGLE;
    //    }
    //    else
    //    {
    //        A->type = WAVE_SINE;
    //        B->type = WAVE_SINE;
    //    }
    //}

    
    // 注：以下是我写的部分，根据题意，只有一个三角波因此判断A为三角波后B必定为正弦。而判断A为正弦后B的类型判断没写
    if (FFT_mag[a_3_index] > 0)
    {
        if (a_3_index == B->bin)
        {
            if (FFT_mag[a_5_index] > 0.0f)
            {
                if (FFT_mag[a_5_index] / FFT_mag[A->bin] > 0.02f)
                {
                    A->type = WAVE_TRIANGLE;
                    B->type = WAVE_SINE;
                }
                else
                {
                    A->type = WAVE_SINE;
                    // 缺少B的判断
                }
            }
            else
            {
                A->type = WAVE_SINE;
                // 缺少B的判断
            }
        }
        else
        {
            A->type = WAVE_TRIANGLE;
            B->type = WAVE_SINE;
        }
    }
    //此分支应当为A三次谐波幅值为零、五次谐波幅值不为零的情况，但是前面没加return所以有bug
    else if (FFT_mag[a_5_index] > 0)
    {
        if (a_5_index == B->bin)
        {
            if (FFT_mag[a_3_index] > 0.0f)
            {
                //理论上在三次谐波为零情况不会进入这个分支。为了可读性高增加了这个判断。
                if (FFT_mag[a_3_index] / FFT_mag[A->bin] > 0.08f)
                {
                    A->type = WAVE_TRIANGLE;
                    B->type = WAVE_SINE;
                }
                else
                {
                    A->type = WAVE_SINE;
                    // 缺少B的判断
                }
            }
            else
            {
                A->type = WAVE_SINE;
                // 缺少B的判断
            }
        }
        else if (a_5_index == B->bin * 3)
        {
            //理论上在三次谐波为零情况不会进入这个分支。为了可读性高增加了这个判断。
            if (FFT_mag[a_3_index] > 0.0f)
            {
                if (FFT_mag[a_3_index] / FFT_mag[A->bin] > 0.08f)
                {
                    A->type = WAVE_TRIANGLE;
                    B->type = WAVE_SINE;
                }
                else
                {
                    A->type = WAVE_SINE;
                    // 缺少B的判断
                }
            }
            else
            {
                A->type = WAVE_SINE;
                // 缺少B的判断
            }
        }
    }
    // A的三次谐波无幅值且五次谐波无幅值才会进入else但是前面没加return所以有bug
    else
    {
        //等你补充
    }
}


void test()
{
    wavetypedetect(FFT_mag, fs, &A, &B);
}
