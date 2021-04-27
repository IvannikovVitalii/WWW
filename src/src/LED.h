/// LED SUPPORT ///////
#if defined(PLATFORM_ESP32) && defined(GPIO_PIN_LED)
#include <NeoPixelBus.h>
const uint16_t PixelCount = 2; // this example assumes 2 pixel
#define colorSaturation 50
NeoPixelBus<NeoRgbFeature, Neo800KbpsMethod> strip(PixelCount, GPIO_PIN_LED);

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

HslColor hslRed(red);
HslColor hslGreen(green);
HslColor hslBlue(blue);
HslColor hslWhite(white);
HslColor hslBlack(black);
#endif

