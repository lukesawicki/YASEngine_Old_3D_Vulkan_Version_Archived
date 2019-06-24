#ifndef TIMEPICKER_HPP
#define TIMEPICKER_HPP

//-----------------------------------------------------------------------------|---------------------------------------|
//                                                                            80                                     120

class TimePicker
{
    public:
        TimePicker();
        float getSeconds();

    private:
        LARGE_INTEGER frequency;
        bool isFrequencyOfThePerformanceCounterPickedUp;
};

#endif

//                                                                            80                                     120
//-----------------------------------------------------------------------------|---------------------------------------|
