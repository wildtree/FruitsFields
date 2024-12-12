
#include <screenshot.h>


ScreenShot::BMP_File_Header ScreenShot::_bf;
ScreenShot::BMP_Info_Header ScreenShot::_bi;

char ScreenShot::_filename[PATH_MAX] = "/screenshot_0000.bmp";


ScreenShot::ScreenShot()
{

}

ScreenShot::~ScreenShot()
{

}

const char *
ScreenShot::get_next_filename()
{
    char num[5];
    strncpy(num, strchr(_filename, '_') + 1, 4);
    int n = strtol(num, nullptr, 0);
    while (SD.exists(_filename))
    {
      if (++n > 9999) n = 0;
      snprintf(num, 5, "%04d", n);
      strncpy(strchr(_filename, '_') + 1, num, 4);
    }
    return _filename;
}

void
ScreenShot::take(const char *filename)
{
    if (filename == nullptr)
    {
        filename = get_next_filename();
    }
   
    _bf = {
        ('M'<<8)|'B',
        (uint32_t)(M5.Displays(0).width() * M5.Displays(0).height() * (M5.Displays(0).getColorDepth() / 8)),
        0,
        0,
        14 + 40,
    };
    _bi = {
        40,
        (uint32_t)M5.Displays(0).width(),
        (uint32_t)M5.Displays(0).height(),
        1,
        M5.Displays(0).getColorDepth(),
        0,
        (uint32_t)(M5.Displays(0).width() * M5.Displays(0).height() * (M5.Displays(0).getColorDepth() / 8)),
        7874,
        7874,
        0,
        0,
    };
    uint16_t lineBuf[M5.Displays(0).width()];
    uint8_t b = M5.Displays(0).getBrightness();
    M5.Displays(0).setBrightness(20);
    File fp = SD.open(filename, FILE_WRITE);
    if (fp)
    {
        fp.write((uint8_t*)&_bf, sizeof(_bf));
        fp.write((uint8_t*)&_bi, sizeof(_bi));
        for (int y = M5.Displays(0).height() - 1 ; y >= 0 ; y--)
        {
            for (int x = 0 ; x < M5.Displays(0).width() ; x++)
            {
                lineBuf[x] = M5.Displays(0).readPixel(x, y);
            }
            fp.write((uint8_t*)lineBuf, sizeof(lineBuf));
        }
        fp.close();
    }
    M5.Displays(0).setBrightness(b);
}