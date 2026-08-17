#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <helpers/ui/DisplayDriver.h>

class M5StackILI9341Display : public DisplayDriver
{
    Adafruit_ILI9341 display;
    GFXcanvas16 *frame = nullptr;
    U8G2_FOR_ADAFRUIT_GFX unicode;
    uint16_t scanline[320];
    bool _begun = false;
    bool _is_on = false;
    uint16_t _color = ILI9341_WHITE;
    int _cursor_x = 0;
    int _cursor_y = 0;

    int fontHeight();
    void applyFont(int sz);

  public:
    M5StackILI9341Display() : DisplayDriver(160, 120), display(&SPI, PIN_TFT_DC, PIN_TFT_CS, PIN_TFT_RST) {}

    bool begin();
    bool isOn() override { return _is_on; }
    void turnOn() override;
    void turnOff() override;
    void clear() override;
    void startFrame(Color bkg = DARK) override;
    void setTextSize(int sz) override;
    void setColor(Color c) override;
    void setCursor(int x, int y) override;
    void print(const char *str) override;
    void printWordWrap(const char *str, int max_width) override;
    void drawTextEllipsized(int x, int y, int max_width, const char *str) override;
    void translateUTF8ToBlocks(char *dest, const char *src, size_t dest_size) override;
    void fillRect(int x, int y, int w, int h) override;
    void drawRect(int x, int y, int w, int h) override;
    void drawXbm(int x, int y, const uint8_t *bits, int w, int h) override;
    uint16_t getTextWidth(const char *str) override;
    void endFrame() override;
};
