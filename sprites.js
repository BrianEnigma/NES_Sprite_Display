var MAX_ROW = 32;
var MAX_COL = 32;
var MAX_PAL = 15;

function generateGrid()
{
    s = "";
    for (var row = 0; row < MAX_ROW; row++)
    {
        s += '<div class="row">';
        for (var col = 0; col < MAX_COL; col++)
        {
            s += '<input id="p' + row + '-' + col + '" type="text" class="pixel" value="0" onchange="doPixelChange(this);" />';
        }
        s += '</div>';
    }
    $('#grid').html(s);
}

function generatePalette()
{
    var sampleColors = [
        [0x00, 0x00, 0x00],
        [0xFF, 0x00, 0x00],
        [0x00, 0xFF, 0x00],
        [0x00, 0x00, 0xFF],
        [0xFF, 0xFF, 0x00],
        [0x00, 0xFF, 0xFF],
        [0xFF, 0x00, 0xFF],
        [0xFF, 0xFF, 0xFF],
    ];
    s = '<div>0: transparent</div>';
    for (var i = 1; i <= MAX_PAL; i++)
    {
        s += '<div>' + i + ': ';
        s += 'R=<input type="text" id="col' + i + 'r" class="colpal" value="00" onchange="doPaletteChange();"/> ';
        s += 'G=<input type="text" id="col' + i + 'g" class="colpal" value="00" onchange="doPaletteChange();"/> ';
        s += 'B=<input type="text" id="col' + i + 'b" class="colpal" value="00" onchange="doPaletteChange();"/> ';
        s += '<div id="col' + i + '" class="colpalSample">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</div>';
        s += '</div>';
    }
    $('#colorPaletteForm').html(s);
    for (var i = 1; i <= MAX_PAL; i++)
    {
        if (i <= sampleColors.length)
        {
            $('#col' + i + 'r').val((sampleColors[i - 1][0]).toString(16));
            $('#col' + i + 'g').val((sampleColors[i - 1][1]).toString(16));
            $('#col' + i + 'b').val((sampleColors[i - 1][2]).toString(16));
        }
    }
}

function fixColor(colorValue)
{
    var result = parseInt(colorValue, 16);
    result = Math.min(255, result);
    result = Math.max(0, result);
    result = result.toString(16);
    if (1 == result.length)
        result = '0' + result;
    return result;
}

function parseColors()
{
    var colors=["#FFFFFF"];
    for (var color = 1; color <= MAX_PAL; color++)
    {
        colors[color] = "#" + fixColor($("#col" + color + "r").val()) + fixColor($("#col" + color + "g").val()) + fixColor($("#col" + color + "b").val());
    }
    return colors;
}

function updateAll()
{
    colorArray = parseColors();
    cCode = "";
    jCode = "";
    dText = "";
    // Display palette sample colors
    cCode += "// Palette values start at 1. (0 = transparent)\n";
    cCode += "unsigned int palette[] = {";
    jCode += '{"palette":[';
    for (var color = 1; color <= MAX_PAL; color++)
    {
        $("#col" + color).css("background-color", colorArray[color]);
        cCode += "0x" + colorArray[color].substring(1);
        jCode += '["' + colorArray[color].substring(1, 3) + '","' + colorArray[color].substring(3, 5) + '","' + colorArray[color].substring(5, 7) + '"]';
        if (color < MAX_PAL)
        {
            cCode += ', ';
            jCode += ',';
        }
    }
    cCode += "};\n";
    jCode += '],\n';
    
    // Display pixel bg as color
    cCode += "// Each byte represents two pixels, the first in the upper nibble\n";
    cCode += "unsigned char pixels[] = {";
    jCode += '"pixels":[';
    for (var row = 0; row < MAX_ROW; row++)
    {
        var cPixelValue = 0;
        for (var col = 0; col < MAX_COL; col++)
        {
            idString = '#p' + row + '-' + col;
            paletteNumber = parseInt($(idString).val());
            paletteNumber = Math.min(MAX_PAL, paletteNumber);
            paletteNumber = Math.max(0, paletteNumber);
            $(idString).css("background-color", colorArray[paletteNumber]);
            cPixelValue = cPixelValue * 16; // left shift 4 nits
            cPixelValue |= paletteNumber & 0x0F;
            if (col % 2 == 1)
            {
                cCode += "0x" + cPixelValue.toString(16);
                if ((MAX_ROW - 1) != row || (MAX_COL - 1) != col)
                    cCode += ","
                cPixelValue = 0;
            }
            jCode += paletteNumber;
            if ((MAX_ROW - 1) != row || (MAX_COL - 1) != col)
                jCode += ",";
            //dText += idString + " => " + paletteNumber + " " + colorArray[paletteNumber] + "\n";
        }
        cCode += "\n";
        jCode += "\n";
    }
    cCode += "};";
    jCode += "]}\n";
    $('#generatedCodeText').val(cCode);
    $('#generatedJsonText').val(jCode);
    //$('#debugText').val(dText);
}

function loadJson()
{
    var obj = JSON.parse($('#generatedJsonText').val());
    palette = obj['palette'];
    pixels = obj['pixels'];
    for (var i = 1; i <= MAX_PAL && i <= palette.length; i++)
    {
        color = palette[i - 1];
        $("#col" + i + "r").val(color[0]);
        $("#col" + i + "g").val(color[1]);
        $("#col" + i + "b").val(color[2]);
    }
    i = 0;
    for (var row = 0; row < MAX_ROW; row++)
    {
        for (var col = 0; col < MAX_COL; col++)
        {
            $('#p' + row + '-' + col).val(pixels[i]);
            i += 1;
        }
    }
    updateAll();
    return false;
}

function doPaletteChange()
{
    updateAll();
}

function doPixelChange()
{
    updateAll();
}

function doSizeChange()
{
    var size = parseInt($("input[name=gridSize]:checked").val());
    if (16 == size || 32 == size)
    {
        MAX_ROW = size;
        MAX_COL = size;
        generateGrid();
        generatePalette();
        updateAll();
    }
}

function doLoadFinished()
{
    generateGrid();
    generatePalette();
    updateAll();
}