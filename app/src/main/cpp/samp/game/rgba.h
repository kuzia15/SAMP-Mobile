#pragma once

#include <cstdint>
#include "game/RW/RenderWare.h"

class CRGBA
{
public:
    uint8_t r{}, g{}, b{}, a{};

public:
    CRGBA() = default;
    CRGBA(const CRGBA& rhs) = default;

    CRGBA(uint8_t red, uint8_t green, uint8_t blue) { Set(red, green, blue); }
    CRGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha){ Set(red, green, blue, alpha); }
    CRGBA(uint32_t intValue) { Set(intValue); }
    CRGBA(const RwRGBA& rhs) { Set(rhs); }
    CRGBA(const CRGBA& rhs, uint8_t alpha) { Set(rhs, alpha); };
    explicit CRGBA(RwRGBAReal rgba);

    void Set(uint8_t red, uint8_t green, uint8_t blue);
    void Set(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
    void Set(uint32_t intValue);
    void Set(const CRGBA& rhs);
    void Set(const CRGBA& rhs, uint8_t alpha);
    void Set(const RwRGBA& rwcolor);

    CRGBA  ToRGB() const;
    uint32_t ToInt() const;
    uint32_t ToIntARGB() const;
    RwRGBA* ToRwRGBA();

    void FromRwRGBA(const RwRGBA& rwcolor);
    void FromARGB(uint32_t intValue);
    CRGBA FromInt32(int32_t red, int32_t green, int32_t blue, int32_t alpha);

    void  Invert();
    CRGBA Inverted() const;
    void RainbowCycle(uint32_t time);

    bool   operator==(const CRGBA& rhs) const;
    CRGBA& operator=(const CRGBA& rhs);

    CRGBA operator*(float mult) const {
        return {
                (uint8_t)((float)r * mult),
                (uint8_t)((float)g * mult),
                (uint8_t)((float)b * mult),
                (uint8_t)((float)a * mult)
        };
    }

    CRGBA operator/(float divisor) {
        return *this * (1 / divisor);
    }

    CRGBA& operator*=(float mult) {
        *this = *this * mult;
        return *this;
    }

    operator RwRGBAReal() { return { (RwReal)r / 255.f, (RwReal)g / 255.f, (RwReal)b / 255.f, (RwReal)a / 255.f }; }
    operator RwRGBA()     { return { r, g, b, a }; }

    static CRGBA RandomColor();
};