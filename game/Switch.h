#pragma once
#include "Button.h"


class Switch :
    public Button
{
public:
    int state = 0;
    const char* Function;
    int FunctionFontSize;
    Switch(const Vector2& _p, const Vector2& _d, const char* _text, int _fs) : Button(_p, _d, "OFF", 30, RED) {
        this->Function = _text;
        this->FunctionFontSize = _fs;
    };
    void DrawSwitch();
    void UpdateSwitch();
    int GetState();
};
