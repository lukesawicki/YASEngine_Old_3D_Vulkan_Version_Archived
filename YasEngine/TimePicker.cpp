#include"stdafx.hpp"
#include"TimePicker.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|
//                                                                            80                                     120

    TimePicker::TimePicker()
    {
        isFrequencyOfThePerformanceCounterPickedUp = (QueryPerformanceFrequency(&frequency) != 0);
    }

    float TimePicker::getSeconds()
    {
        if(!isFrequencyOfThePerformanceCounterPickedUp)
        {
            return GetTickCount64() / 1000.0F;
        }
        else
        {
            LARGE_INTEGER ticks;
            QueryPerformanceCounter(&ticks);
            return (float)(ticks.QuadPart / (double)frequency.QuadPart);
        }
    }

//                                                                            80                                     120
//-----------------------------------------------------------------------------|---------------------------------------|

