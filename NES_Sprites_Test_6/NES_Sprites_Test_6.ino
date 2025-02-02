#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <SoftWire.h>
#include <AsyncDelay.h>

#define CLK 8  // MUST be on PORTB! (Use pin 11 on Mega)
#define OE  9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);
SoftWire sw(A4, A5);

#define DIGIT_HEIGHT 5
#define DIGIT_WIDTH 3
#if 1
static const uint8_t DIGIT_PIXELS[DIGIT_HEIGHT * DIGIT_WIDTH * 10] PROGMEM = {
    1,1,1, 0,1,0, 1,1,1, 1,1,1, 1,0,1, 1,1,1, 1,1,1, 1,1,1, 1,1,1, 1,1,1,
    1,0,1, 0,1,0, 0,0,1, 0,0,1, 1,0,1, 1,0,0, 1,0,0, 0,0,1, 1,0,1, 1,0,1,
    1,0,1, 0,1,0, 1,1,1, 0,1,1, 1,1,1, 1,1,1, 1,1,1, 0,0,1, 1,1,1, 1,1,1,
    1,0,1, 0,1,0, 1,0,0, 0,0,1, 0,0,1, 0,0,1, 1,0,1, 0,0,1, 1,0,1, 0,0,1,
    1,1,1, 0,1,0, 1,1,1, 1,1,1, 0,0,1, 1,1,1, 1,1,1, 0,0,1, 1,1,1, 0,0,1
};
#else
static const uint8_t DIGIT_PIXELS[DIGIT_HEIGHT * DIGIT_WIDTH * 2] PROGMEM = {
    1,1,1, 0,1,0,
    1,0,1, 0,1,0,
    1,0,1, 0,1,0,
    1,0,1, 0,1,0,
    1,1,1, 0,1,0 
};
#endif

// We're making these globals to save stack space.
int row;                        ///< Row count, when looping.
int col;                        ///< Column count, when looping.
unsigned char paletteIndex;     ///< Index into the color palette.
unsigned long pixelPosition;    ///< Index into the pixel array.
unsigned char red;              ///< Red color value, pulled from the palette.
unsigned char green;            ///< Green color value, pulled from the palette.
unsigned char blue;             ///< Blue color value, pulled from the palette.

void setup() 
{
    matrix.begin();
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    sw.begin();
    sw.setTimeout_ms(500);
    sw.setDelay_us(100);
}

bool getTime(uint8_t &hours, uint8_t &minutes, uint8_t &seconds)
{
    uint8_t b1 = 0, b2 = 0, b3 = 0;
    sw.startWriteWait(0x68);
    sw.write(0);
    sw.stop();
    sw.startReadWait(0x68);
    sw.readThenAck(b1);
    sw.readThenAck(b2);
    sw.readThenNack(b3);
    sw.stop();
    seconds = (b1 >> 4) * 10 + (b1 & 0x0F);
    minutes = (b2 >> 4) * 10 + (b2 & 0x0F);
    hours = ((b3 >> 4) & 0x01) * 10 + (b3 & 0x0F);
    return true;
}

void drawDigit(uint8_t digit, uint16_t x, uint16_t y)
{
    for (int row = 0; row < DIGIT_HEIGHT; row++)
    {
        for (int column = 0; column < DIGIT_WIDTH; column++)
        {
            if (1 == pgm_read_byte(DIGIT_PIXELS + (row * DIGIT_WIDTH * 10) + (digit * DIGIT_WIDTH + column)))
                matrix.drawPixel(x + column, y + row, matrix.Color444(4, 4, 0));
        }
    }
}

void drawTime(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    uint16_t x = 0;
    if (hours / 10 >= 1)
    {
        drawDigit(hours   / 10, x, 0);
        x += DIGIT_WIDTH + 1;
    }
    drawDigit(hours   % 10, x, 0);
    x += DIGIT_WIDTH + 1;
    matrix.drawPixel(x, 1, matrix.Color444(2, 2, 0));
    matrix.drawPixel(x, 3, matrix.Color444(2, 2, 0));
    x += 2;
    drawDigit(minutes / 10, x, 0);
    x += DIGIT_WIDTH + 1;
    drawDigit(minutes % 10, x, 0);
    x += DIGIT_WIDTH + 1;
    matrix.drawPixel(x, 1, matrix.Color444(2, 2, 0));
    matrix.drawPixel(x, 3, matrix.Color444(2, 2, 0));
    x += 2;
    drawDigit(seconds / 10, x, 0);
    x += DIGIT_WIDTH + 1;
    drawDigit(seconds % 10, x, 0);
    x += DIGIT_WIDTH + 1;
}

void loop()
{
    loop1();
}

void loop2() 
{
    matrix.fillScreen(matrix.Color444(4, 0, 0));
    delay(1000);
    matrix.fillScreen(matrix.Color444(0, 0, 4));
    drawTime(1, 23, 45);
    delay(5000);
}

void loop1() 
{
    uint8_t hours, minutes, seconds;
    for (int counter = 0; counter < 3; counter++)
    {
        switch(counter)
        {
            case 0:     matrix.fillScreen(matrix.Color444(4, 0, 0)); break;
            case 1:     matrix.fillScreen(matrix.Color444(0, 4, 0)); break;
            case 2:     matrix.fillScreen(matrix.Color444(0, 0, 4)); break;
            default:    matrix.fillScreen(matrix.Color444(0, 0, 0)); break;
        }
        getTime(hours, minutes, seconds);
        drawTime(hours, minutes, seconds);
        delay(1000);
    }
}
