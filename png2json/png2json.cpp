#include <stdio.h>
#include <set>
#include <map>
#include <gd.h>

//#define DEBUG_PALETTE

typedef unsigned int uint32_t;

void buildColorSet(gdImagePtr im, std::set<uint32_t> &list)
{
    size_t width = gdImageSX(im);
    size_t height = gdImageSY(im);
    list.clear();
    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            list.insert(gdImageGetPixel(im, x, y)); // TODO: handle full alpha with varying other colors?
        }
    }
#ifdef DEBUG_PALETTE
    printf("Color Set:\n");
    for (uint32_t color : list)
    {
        printf("  (R:%02X G:%02X B:%02X A:%02X)\n",
            gdImageRed(im, color),
            gdImageGreen(im, color),
            gdImageBlue(im, color),
            2 * gdImageAlpha(im, color)
        );
    }
#endif    
}

void buildColorMap(gdImagePtr im, const std::set<uint32_t> &list, std::map<uint32_t, uint32_t> &colorMap)
{
    unsigned int counter = 1;
    colorMap.clear();
#ifdef DEBUG_PALETTE
    printf("Inserting To Color Map:\n");
#endif    
    for (uint32_t color : list)
    {
        unsigned char alpha = gdImageAlpha(im, color);
        if (127 == alpha)
            colorMap[color] = 0;
        else if (0 == alpha)
        {
#ifdef DEBUG_PALETTE
        printf("  %d => (R:%02X G:%02X B:%02X A:%02X)\n",
            counter,
            gdImageRed(im, color),
            gdImageGreen(im, color),
            gdImageBlue(im, color),
            2 * gdImageAlpha(im, color)
        );
#endif
            colorMap[color] = counter++;
        }
        else
        {
            printf("WARNING: color %d has an alpha (R:%02X G:%02X B:%02X A:%02X)\n",
                counter,
                gdImageRed(im, color),
                gdImageGreen(im, color),
                gdImageBlue(im, color),
                2 * gdImageAlpha(im, color)
            );
            colorMap[color] = counter++;
        }
    }
#ifdef DEBUG_PALETTE
    printf("Displaying Color Map:\n");
    for (const auto mapPair : colorMap)
    {
        printf("  %d => (R:%02X G:%02X B:%02X A:%02X)\n",
            mapPair.second,
            gdImageRed(im, mapPair.first),
            gdImageGreen(im, mapPair.first),
            gdImageBlue(im, mapPair.first),
            2 * gdImageAlpha(im, mapPair.first)
        );
    }
#endif    
}

void generatePalette(gdImagePtr im, const std::map<uint32_t, uint32_t> &colorMap)
{
    bool firstPrint = true;
    std::map<uint32_t, uint32_t> reverseMap;
    for (const auto mapPair : colorMap)
    {
        reverseMap[mapPair.second] = mapPair.first;
    }
    printf("\"palette\":[");
    for (const auto mapPair : reverseMap)
    {
        if (0 != mapPair.first)
        {
            if (firstPrint)
                firstPrint = false;
            else
                printf(",");
            printf("[\"%02X\",\"%02X\",\"%02X\"]", 
                gdImageRed(im, mapPair.second), 
                gdImageGreen(im, mapPair.second), 
                gdImageBlue(im, mapPair.second));
        }
    }
    printf("],\n");
}

void generateImage(gdImagePtr im, const std::map<uint32_t, uint32_t> &colorMap)
{
    size_t width = gdImageSX(im);
    size_t height = gdImageSY(im);
    std::map<uint32_t, uint32_t>::const_iterator it;
    printf("\"pixels\":[");
    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            it = colorMap.find(gdImageGetPixel(im, x, y));
            if (it != colorMap.end())
                printf("%d", it->second);
            else
                printf("-1");
            if (y != height - 1 || x != width - 1)
                printf(",");
        }
        printf("\n");
    }
    printf("]");
}

int convert(const char *paletteFilename, const char *imageFilename)
{
    std::set<uint32_t> colors;
    std::map<uint32_t, uint32_t> colorMap;
    FILE *f = fopen(paletteFilename, "rb");
    gdImagePtr im = gdImageCreateFromPng(f);
    if (!f || !im)
    {
        printf("Error reading input file.\n");
        return 1;
    }
    buildColorSet(im, colors);
    if (colors.size() > 16)
    {
        printf("The color table had too many (%d) entries.", (unsigned int) colors.size());
        return 1;
    }
    buildColorMap(im, colors, colorMap);
    gdImageDestroy(im);
    fclose(f);

    f = fopen(imageFilename, "rb");
    im = gdImageCreateFromPng(f);
    if (!f || !im)
    {
        printf("Error reading input file.\n");
        return 1;
    }

    printf("{");
    generatePalette(im, colorMap);
    generateImage(im, colorMap);
    printf("}");
    gdImageDestroy(im);
    fclose(f);
    return 0;
}

int main(int argc, char **argv)
{
    if (2 != argc && 3 != argc)
    {
        printf("Error: put png file to read on command line.\n");
        return 1;
    }
    return convert(argv[1], 3 == argc ? argv[2] : argv[1]);
}