#include <megaButton.h>

megaButton::megaButton(int pin, bool enable_pullup, bool active_low)
{
    _pin = pin;
    _active_low = active_low;
    if (enable_pullup)
        pinMode(_pin, INPUT_PULLUP);
    else
        pinMode(_pin, INPUT);
    _last_value = active_low ? 1 : 0;
}

void megaButton::handle()
{
    _read_value = digitalRead(_pin);
    if (_last_value == _read_value)
        return;
    _time_current = millis();
    _last_value = _read_value;
    if (_active_low)
        _read_value = !_read_value;
    if ((_read_value & 0x01) && (_state == kIDLE))
    {
        _state = kPRESSED;
        _time_start = _time_current;
        if (_func_press)
            _func_press();
    }
    else if (_time_start > 0)
    {
        if (_time_current - _time_start < _tick_debounce)
        {
            _state = kRELEASED;
            if (_func_release)
                _func_release();
        }
        else if (_time_current - _time_start >= _tick_timeout)
        {
            _state = kTIMEOUT;
            if (_func_timeout)
                _func_timeout();
        }
        else if (_time_current - _time_start >= _tick_long_press)
        {
            _state = kLONGPRESS;
            if (_func_long_press)
                _func_long_press();
        }
        else if (_time_current - _time_start >= _tick_debounce)
        {
            _state = kCLICK;
            if (_func_click)
                _func_click();
        }
        _time_start = 0;
        _state = kIDLE;
    }
}
void megaButton::attachPress(callbackFunction func)
{
    _func_press = func;
}
void megaButton::attachLongPress(callbackFunction func)
{
    _func_long_press = func;
}
void megaButton::attachClick(callbackFunction func)
{
    _func_click = func;
}
void megaButton::attachRelease(callbackFunction func)
{
    _func_release = func;
}
void megaButton::attachTimeout(callbackFunction func)
{
    _func_timeout = func;
}
void megaButton::setDebounceTicks(const int ticks)
{
    _tick_debounce = ticks;
}
void megaButton::setLongPressTicks(const int ticks)
{
    _tick_long_press = ticks;
}
void megaButton::setTimeoutTicks(const int ticks)
{
    _tick_timeout = ticks;
}
ButtonState megaButton::getState()
{
    return _state;
}