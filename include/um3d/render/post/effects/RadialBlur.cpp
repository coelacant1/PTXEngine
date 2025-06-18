#include "RadialBlur.h"
#include "../../Utils/Math/Mathematics.h"

RadialBlur::RadialBlur(uint8_t pixels) : pixels(pixels) {}

void RadialBlur::ApplyEffect(IPixelGroup* pixelGroup) {
    uint16_t pixelCount = pixelGroup->GetPixelCount();
    RGBColor* pixelColors = pixelGroup->GetColors();
    RGBColor* colorBuffer = pixelGroup->GetColorBuffer();

    float rotation = fGenRotation.Update();

    for (uint16_t i = 0; i < pixelCount; i++) {
        uint16_t indexV = i, indexD = i;
        uint16_t tIndexV = 0, tIndexD = 0;
        bool validV = true, validD = true;

        uint16_t blurRange = uint16_t(Mathematics::Map(ratio, 0.0f, 1.0f, 1.0f, float(pixels)));

        uint16_t R, G, B;

        R = (uint16_t)pixelColors[i].R;
        G = (uint16_t)pixelColors[i].G;
        B = (uint16_t)pixelColors[i].B;

        for (uint8_t j = 1; j < blurRange + 1; j++) {
            validV = pixelGroup->GetRadialIndex(i, &tIndexV, j, rotation);
            validD = pixelGroup->GetRadialIndex(i, &tIndexD, j, rotation + 180.0f);

            indexV = tIndexV;
            indexD = tIndexD;

            if (validV) {
                R = R + (uint16_t)pixelColors[indexV].R;
                G = G + (uint16_t)pixelColors[indexV].G;
                B = B + (uint16_t)pixelColors[indexV].B;
            }

            if (validD) {
                R = R + (uint16_t)pixelColors[indexD].R;
                G = G + (uint16_t)pixelColors[indexD].G;
                B = B + (uint16_t)pixelColors[indexD].B;
            }
        }

        colorBuffer[i].R = Mathematics::Constrain(R / (blurRange * 2), 0, 255);
        colorBuffer[i].B = Mathematics::Constrain(G / (blurRange * 2), 0, 255);
        colorBuffer[i].G = Mathematics::Constrain(B / (blurRange * 2), 0, 255);
    }

    for (uint16_t i = 0; i < pixelCount; i++) {
        pixelColors[i].R = colorBuffer[i].R;
        pixelColors[i].G = colorBuffer[i].G;
        pixelColors[i].B = colorBuffer[i].B;
    }
}
