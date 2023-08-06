#ifndef MEGABUTTON_H
#define MEGABUTTON_H
#include <Arduino.h>

extern "C"
{
    typedef void (*callbackFunction)(void);
}
enum ButtonState
{
    kIDLE,
    kPRESSED,
    kRELEASED,
    kLONGPRESS,
    kCLICK,
    kTIMEOUT
};
enum DigitalFilterType
{
    kDigitalFilterTypeNone,
    kDigitalFilterTypeSimple,
    kDigitalFilterTypeHard,
};
class megaButton
{
private:
    int _readInput();
    int _readSafe();
    int _pin;
    int _last_value;
    uint8_t _read_safe_count = 20;
    uint8_t _try_read_number = 5;
    uint8_t _try_read_count = 0;
    uint8_t _try_read_interval_ms = 10;
    bool _active_low;
    DigitalFilterType _filter = kDigitalFilterTypeNone;
    unsigned long _time_filter = 0;
    unsigned long _time_current = 0;
    unsigned long _time_start = 0;
    unsigned long _tick_debounce = 50;
    unsigned long _tick_long_press = 1000;
    unsigned long _tick_timeout = 5000;
    callbackFunction _func_press = NULL;
    callbackFunction _func_long_press = NULL;
    callbackFunction _func_click = NULL;
    callbackFunction _func_release = NULL;
    callbackFunction _func_timeout = NULL;
    ButtonState _state = kIDLE;

public:
    megaButton(int pin, bool enable_pullup_pulldown, bool active_low, DigitalFilterType filter = kDigitalFilterTypeNone);
    void handle();
    void attachPress(callbackFunction func);
    void attachLongPress(callbackFunction func);
    void attachClick(callbackFunction func);
    void attachRelease(callbackFunction func);
    void attachTimeout(callbackFunction func);
    void setDebounceTicks(const int ticks);
    void setLongPressTicks(const int ticks);
    void setTimeoutTicks(const int ticks);
    ButtonState getState();
};

#endif