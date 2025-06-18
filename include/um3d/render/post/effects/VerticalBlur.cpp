#include "VerticalBlur.h"

VerticalBlur::VerticalBlur(uint8_t pixels) : pixels(pixels) {}

void VerticalBlur::ApplyEffect(IPixelGroup* pixelGroup) {
    RGBColor* pixelColors = pixelGroup->GetColors();
    RGBColor* colorBuffer = pixelGroup->GetColorBuffer();

    for (uint16_t i = 0; i < pixelGroup->GetPixelCount(); i++) {
        uint16_t indexUp = i;
        uint16_t indexDown = i;
        uint16_t tIndexUp = 0;
        uint16_t tIndexDown = 0;
        bool validL = false;
        bool validR = false;

        uint16_t blurRange = uint16_t(Mathematics::Map(ratio, 0.0f, 1.0f, 1.0f, float(pixels / 2)));

        uint16_t R, G, B;

        R = (uint16_t)pixelColors[i].R;
        G = (uint16_t)pixelColors[i].G;
        B = (uint16_t)pixelColors[i].B;

        for (uint8_t j = 1; j < blurRange + 1; j++) {
            validL = pixelGroup->GetUpIndex(indexUp, &tIndexUp);
            validR = pixelGroup->GetDownIndex(indexDown, &tIndexDown);

            indexUp = tIndexUp;
            indexDown = tIndexDown;

            if (validL && validR) {
                R = R + (uint16_t)pixelColors[indexUp].R + (uint16_t)pixelColors[indexDown].R;
                G = G + (uint16_t)pixelColors[indexUp].G + (uint16_t)pixelColors[indexDown].G;
                B = B + (uint16_t)pixelColors[indexUp].B + (uint16_t)pixelColors[indexDown].B;
            }
        }

        colorBuffer[i].R = Mathematics::Constrain(R / (blurRange * 2), 0, 255);
        colorBuffer[i].B = Mathematics::Constrain(G / (blurRange * 2), 0, 255);
        colorBuffer[i].G = Mathematics::Constrain(B / (blurRange * 2), 0, 255);
    }

    for (uint16_t i = 0; i < pixelGroup->GetPixelCount(); i++) {
        pixelColors[i].R = colorBuffer[i].R;
        pixelColors[i].G = colorBuffer[i].G;
        pixelColors[i].B = colorBuffer[i].B;
    }
}
