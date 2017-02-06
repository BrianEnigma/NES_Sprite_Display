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

void drawTime(uint8_t &hours, uint8_t minutes, uint8_t seconds)
{
    matrix.setTextColor(matrix.Color444(4, 4, 0));
    matrix.setTextSize(1);
    matrix.setCursor(0, 0);
    matrix.print(hours);
    matrix.print(":");
    matrix.print(minutes);
    matrix.print(":");
    matrix.print(seconds);
}

void loop() 
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
