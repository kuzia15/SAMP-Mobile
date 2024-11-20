/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "RGBA.h"
#include "common.h"

CRGBA::CRGBA(RwRGBAReal rgba) :
        CRGBA{
                (uint8_t)(rgba.red * 255.f),
                (uint8_t)(rgba.green * 255.f),
                (uint8_t)(rgba.blue * 255.f),
                (uint8_t)(rgba.alpha * 255.f)
        }
{
}

void CRGBA::RainbowCycle(uint32 t) {
    const uint32_t period = 4000;

    // ��������� ��������� ����� � ������ �������
    uint32_t timeElapsed = t % period;

    // ��������� �������� � ������� �� 0 �� 1
    double progress = static_cast<double>(timeElapsed) / static_cast<double>(period);

    // ���������� sin ��� �������� �������� ����� �������
    double angle = progress * 2 * M_PI; // ����������� �������� � ���� (0 - 2?)
    r = sin(angle + 0) * 127 + 128; // ���������� ���������� �����
    g = sin(angle + 2 * M_PI / 3) * 127 + 128;
    b = sin(angle + 4 * M_PI / 3) * 127 + 128;

}

void CRGBA::Set(uint8_t red, uint8_t green, uint8_t blue) {
    r = red;
    g = green;
    b = blue;
}

void CRGBA::Set(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    Set(red, green, blue);
    a = alpha;
}

void CRGBA::Set(uint32_t intValue) {
    r = (uint8_t)(intValue >> 24);
    g = (uint8_t)(intValue >> 16);
    b = (uint8_t)(intValue >> 8 );
    a = (uint8_t)(intValue >> 0 );
}

void CRGBA::Set(const CRGBA& rhs) {
    Set(rhs.r, rhs.g, rhs.b, rhs.a);
}

void CRGBA::Set(const CRGBA& rhs, uint8_t alpha) {
    Set(rhs.r, rhs.g, rhs.b, alpha);
}

void CRGBA::Set(const RwRGBA& rwcolor) {
    Set(rwcolor.red, rwcolor.green, rwcolor.blue, rwcolor.alpha);
}

uint32_t CRGBA::ToInt() const { // RGBA (msb(r) -> lsb(a))
    return a | (b << 8) | (g << 16) | (r << 24);
}

uint32_t CRGBA::ToIntARGB() const {
    return b | (g << 8) | (r << 16) | (a << 24);
}
#include <random>
CRGBA CRGBA::RandomColor() {
    CRGBA randomColor;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> distribution(0, 255);

    randomColor.r = distribution(gen);
    randomColor.g = distribution(gen);
    randomColor.b = distribution(gen);
    randomColor.a = 255;

    return randomColor;
}

RwRGBA* CRGBA::ToRwRGBA() {
    RwRGBA tmp {r, g, b, a};
    return &tmp;
}

void CRGBA::FromRwRGBA(const RwRGBA& rwcolor) {
    Set(rwcolor);
}

void CRGBA::FromARGB(uint32_t intValue) {
    a = (intValue >> 24) & 0xFF;
    r = (intValue >> 16) & 0xFF;
    g = (intValue >> 8) & 0xFF;
    b = intValue & 0xFF;
}

CRGBA CRGBA::FromInt32(int32_t red, int32_t green, int32_t blue, int32_t alpha) {
    return {
            static_cast<uint8_t>(red),
            static_cast<uint8_t>(green),
            static_cast<uint8_t>(blue),
            static_cast<uint8_t>(alpha),
    };
}

void CRGBA::Invert() {
    Set(255 - r, 255 - g, 255 - b);
}

CRGBA CRGBA::Inverted() const {
    CRGBA invertedColor = *this;
    invertedColor.Invert();
    return invertedColor;
}

bool CRGBA::operator==(const CRGBA& rhs) const {
    return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
}

CRGBA& CRGBA::operator=(const CRGBA& rhs) {
    Set(rhs);
    return *this;
}

CRGBA CRGBA::ToRGB() const {
    return CRGBA(r, g, b, 255);
}
