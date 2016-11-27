#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

#define CLK 8  // MUST be on PORTB! (Use pin 11 on Mega)
#define OE  9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);

static const uint32_t TEST_PATTERN_PALETTE[] PROGMEM = {0xff0000, 0x00ff00, 0x0000ff, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000};

static const int8_t TEST_PATTERN_FRAME[] PROGMEM = {
    0x12,0x31,0x23,0x12,0x31,0x23,0x12,0x31,
    0x12,0x31,0x23,0x12,0x31,0x23,0x12,0x31,
    0x12,0x31,0x23,0x12,0x31,0x23,0x12,0x31,
    0x12,0x31,0x23,0x12,0x31,0x23,0x12,0x31,
    0x12,0x31,0x23,0x12,0x31,0x23,0x12,0x31,
    0x12,0x31,0x23,0x12,0x31,0x23,0x12,0x31,
    0x12,0x31,0x23,0x12,0x31,0x23,0x12,0x31,
    0x12,0x31,0x23,0x12,0x31,0x23,0x12,0x31,
    0x12,0x31,0x23,0x12,0x31,0x23,0x12,0x31,
    0x12,0x31,0x23,0x12,0x31,0x23,0x12,0x31,
    0x12,0x31,0x23,0x12,0x31,0x23,0x12,0x31,
    0x12,0x31,0x23,0x12,0x31,0x23,0x12,0x31,
    0x12,0x31,0x23,0x12,0x31,0x23,0x12,0x31,
    0x12,0x31,0x23,0x12,0x31,0x23,0x12,0x31,
    0x12,0x31,0x23,0x12,0x31,0x23,0x12,0x31,
    0x12,0x31,0x23,0x12,0x31,0x23,0x12,0x31};

static const int8_t TEST_PATTERN_FRAME_BYTE[] PROGMEM = {
    0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,
    0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,
    0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,
    0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,
    0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,
    0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,
    0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,
    0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,
    0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,
    0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,
    0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,
    0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,
    0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,
    0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,
    0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,
    0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01,0x02,0x03,0x01};

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
}

void draw16x16ByNibble(const unsigned char *pixels, const void *palette)
{
    pixelPosition = 0;
    for (row = 0; row < 16; row++)
    {
        for (col = 0; col < 16; col++)
        {
            red = green = blue = 0; // Assume transparent is black.
            paletteIndex = pgm_read_byte(pixels + pixelPosition); // Find the byte containing the two pixels.
            if (0 == col % 2)
                paletteIndex = paletteIndex >> 4;
            else
                paletteIndex = paletteIndex & 0x0F;
            if (paletteIndex > 0 && paletteIndex < 16)
            {
                red = pgm_read_byte(palette + 4 * (paletteIndex - 1) + 2);
                green = pgm_read_byte(palette + 4 * (paletteIndex - 1) + 1);
                blue = pgm_read_byte(palette + 4 * (paletteIndex - 1) + 0);
            }
            matrix.drawPixel(row, col, matrix.Color888(red, green, blue)); 
            if (col % 2 == 1)
                pixelPosition++;
        }
    }
}

void draw16x16ByByte(const int8_t *pixels, const void *palette)
{
    pixelPosition = 0;
    for (row = 0; row < 16; row++)
    {
        for (col = 0; col < 16; col++)
        {
            red = green = blue = 0; // Assume transparent is black.
            paletteIndex = pgm_read_byte(pixels + pixelPosition); // Find the byte containing the pixel.
            if (paletteIndex > 0 && paletteIndex < 16)
            {
                red = pgm_read_byte(palette + 4 * (paletteIndex - 1) + 2);
                green = pgm_read_byte(palette + 4 * (paletteIndex - 1) + 1);
                blue = pgm_read_byte(palette + 4 * (paletteIndex - 1) + 0);
            }
            matrix.drawPixel(row, col, matrix.Color888(red, green, blue)); 
            pixelPosition += 1;
        }
    }
}

void drawTestPattern(const unsigned char *pixels, const unsigned long *palette)
{
    int counter = 0;
    for (row = 0; row < 16; row++)
    {
        for (col = 0; col < 16; col++)
        {
            red = green = blue = 0; // Assume transparent is black.
            switch(counter)
            {
                case 0: red = 0xFF; break;
                case 1: green = 0xFF; break;
                case 2: blue = 0xFF; break;
            }
            counter = (counter + 1) % 3;
            matrix.drawPixel(col, row, matrix.Color888(red, green, blue)); 
        }
    }
}

void loop() 
{
    static const uint8_t TEST_CASE = 3;
    switch(TEST_CASE)
    {
        case 1: drawTestPattern(TEST_PATTERN_FRAME_BYTE, TEST_PATTERN_PALETTE); break;
        case 2: draw16x16ByByte(TEST_PATTERN_FRAME_BYTE, TEST_PATTERN_PALETTE); break;
        case 3: draw16x16ByNibble(TEST_PATTERN_FRAME, TEST_PATTERN_PALETTE); break;
    }
    delay(1000);
}
