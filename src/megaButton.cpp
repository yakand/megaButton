#include <megaButton.h>

megaButton::megaButton(int pin, bool enable_pullup_pulldown, bool active_low, DigitalFilterType filter)
{
    _pin = pin;
    _active_low = active_low;
    _filter = filter;
    if (enable_pullup_pulldown)
    {
#ifdef ESP8266
        if (pin == 16)
            pinMode(_pin, INPUT_PULLDOWN_16);
        else
            pinMode(_pin, INPUT_PULLUP);
#else
        pinMode(_pin, INPUT_PULLUP);
#endif
    }
    else
    {
        pinMode(_pin, INPUT);
    }
}

int megaButton::_readSafe()
{
    int v = digitalRead(_pin);
    int l = v;
    for (uint8_t i = 0; i < _read_safe_count; i++)
    {
        v = digitalRead(_pin);
        if (v != l)
        {
            l = -1;
            break;
        }
        l = v;
    }
    return l;
}

int megaButton::_readInput()
{
    int ret = -1;
    if (_filter == kDigitalFilterTypeNone)
    {
        ret = digitalRead(_pin);
    }
    else if (_filter == kDigitalFilterTypeSimple)
    {
        ret = _readSafe();
    }
    else if (_filter == kDigitalFilterTypeHard)
    {
        unsigned long time = millis();
        if ((time - _time_filter) >= _try_read_interval_ms)
        {
            int v;
            _time_filter = time;
            if (_try_read_count == 0)
                _last_value = _readSafe();
            v = _readSafe();
            if (v != _last_value || v == -1)
            {
                _try_read_count = 0;
            }
            else if (_try_read_count++ == _try_read_number)
            {
                _try_read_count = 0;
                ret = _last_value;
            }
        }
    }
    return ret;
}

void megaButton::handle()
{
    int v = _readInput();
    if (v == -1)
        return;
    if (_active_low)
        v = !v;
    _time_current = millis();

    if (v)
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