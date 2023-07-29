#include <megaButton.h>

megaButton::megaButton(int pin, bool enable_pullup_pulldown, bool active_low)
{
    _pin = pin;
    _active_low = active_low;
    if (enable_pullup_pulldown)
    {
        if (ESP8266 && pin == 16)
            pinMode(_pin, INPUT_PULLDOWN_16);
        else
            pinMode(_pin, INPUT_PULLUP);
        
    }
    else
    {
        pinMode(_pin, INPUT);
    }
}

void megaButton::handle()
{
    _read_value = digitalRead(_pin);
    if (_active_low)
        _read_value = !_read_value;
    _time_current = millis();

    if (_read_value)
    {
        if ((_state == kIDLE) && (_time_start == 0))
        {
            _time_start = _time_current;
        }
        else if ((_time_current - _time_start) > _tick_timeout)
        {
            if (_state == kTIMEOUT)
                return;
            _state = kTIMEOUT;
            if (_func_timeout)
                _func_timeout();
        }
        else if ((_time_current - _time_start) > _tick_long_press)
        {
            if (_state == kLONGPRESS)
                return;
            _state = kLONGPRESS;
            if (_func_long_press)
                _func_long_press();
        }
        else if ((_time_current - _time_start) > _tick_debounce)
        {
            if (_state == kPRESSED)
                return;
            _state = kPRESSED;
            if (_func_press)
                _func_press();
        }
    }
    else
    {
        if (_time_start > 0)
        {
            if (((_time_current - _time_start) > _tick_debounce) && ((_time_current - _time_start) < _tick_long_press))
            {
                _state = kCLICK;
                if (_func_click)
                    _func_click();
            }
            _state = kIDLE;
            _time_start = 0;
        }
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