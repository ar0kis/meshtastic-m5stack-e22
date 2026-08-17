#include "M5StackILI9341Display.h"

#include <cstring>

namespace
{
constexpr uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
{
    return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
}

// Minimal high-contrast palette: neutral UI with colour used only for state.
constexpr uint16_t kBackground = rgb565(0, 0, 0);   // black
constexpr uint16_t kText = rgb565(242, 242, 242);   // soft white
constexpr uint16_t kPanel = rgb565(28, 28, 28);     // graphite
constexpr uint16_t kAccent = rgb565(190, 190, 190); // light grey
constexpr uint16_t kSuccess = rgb565(90, 170, 112); // status green
constexpr uint16_t kWarning = rgb565(210, 176, 92); // warning amber
constexpr uint16_t kError = rgb565(196, 82, 82);    // error red

void initM5StackPanel(Adafruit_ILI9341 &display)
{
    // M5Stack Core Basic uses an ILI9342C-compatible panel wired in landscape.
    // Its power/interface setup and MADCTL mapping differ from a generic
    // portrait ILI9341 breakout.  This sequence follows the official M5Stack
    // display driver.
    const uint8_t unlock[] = {0xFF, 0x93, 0x42};
    const uint8_t power1[] = {0x12, 0x12};
    const uint8_t power2[] = {0x03};
    const uint8_t rgb_interface[] = {0xE0};
    const uint8_t interface_control[] = {0x00, 0x01, 0x01};
    const uint8_t pixel_format[] = {0x55};
    const uint8_t display_function[] = {0x08, 0x82, 0x27};
    const uint8_t gamma_positive[] = {0x00, 0x0C, 0x11, 0x04, 0x11, 0x08, 0x37, 0x89, 0x4C, 0x06, 0x0C, 0x0A, 0x2E, 0x34, 0x0F};
    const uint8_t gamma_negative[] = {0x00, 0x0B, 0x11, 0x05, 0x13, 0x09, 0x33, 0x67, 0x48, 0x07, 0x0E, 0x0B, 0x2E, 0x33, 0x0F};

    display.sendCommand(0xC8, unlock, sizeof(unlock));
    display.sendCommand(ILI9341_PWCTR1, power1, sizeof(power1));
    display.sendCommand(ILI9341_PWCTR2, power2, sizeof(power2));
    display.sendCommand(0xB0, rgb_interface, sizeof(rgb_interface));
    display.sendCommand(0xF6, interface_control, sizeof(interface_control));
    display.sendCommand(ILI9341_PIXFMT, pixel_format, sizeof(pixel_format));
    display.sendCommand(ILI9341_DFUNCTR, display_function, sizeof(display_function));
    display.sendCommand(ILI9341_GMCTRP1, gamma_positive, sizeof(gamma_positive));
    display.sendCommand(ILI9341_GMCTRN1, gamma_negative, sizeof(gamma_negative));
    display.sendCommand(ILI9341_SLPOUT);
    delay(120);
    display.sendCommand(ILI9341_DISPON);

    // Keep Adafruit_GFX's logical dimensions at 320x240, then replace the
    // generic rotation-1 MADCTL with M5Stack's native landscape mapping.
    display.setRotation(1);
    const uint8_t m5stack_landscape_madctl = 0x08; // BGR, no MX/MY/MV
    display.sendCommand(ILI9341_MADCTL, &m5stack_landscape_madctl, 1);
}
} // namespace

bool M5StackILI9341Display::begin()
{
    if (!_begun) {
        // Keep the E22 deselected while the shared SPI bus initializes the LCD.
        pinMode(P_LORA_NSS, OUTPUT);
        digitalWrite(P_LORA_NSS, HIGH);

        pinMode(PIN_TFT_BACKLIGHT, OUTPUT);
        digitalWrite(PIN_TFT_BACKLIGHT, HIGH);
#ifdef PIN_USER_BTN
        // M5Stack middle front button (BtnB / GPIO38), active LOW.
        pinMode(PIN_USER_BTN, INPUT_PULLUP);
#endif
        display.begin(40000000);
        initM5StackPanel(display);
        display.setTextWrap(false);
        display.cp437(true);
        display.fillScreen(ILI9341_BLACK);

        frame = new GFXcanvas16(width(), height());
        if (frame == nullptr || frame->getBuffer() == nullptr) {
            Serial.println("ERROR: unable to allocate LCD frame buffer");
            return false;
        }
        frame->setTextWrap(false);
        frame->cp437(true);
        frame->fillScreen(kBackground);
        unicode.begin(*frame);
        unicode.setFontMode(1);
        unicode.setFontDirection(0);
        unicode.setForegroundColor(kText);
        applyFont(1);
        _begun = true;
    }
    turnOn();
    return true;
}

void M5StackILI9341Display::turnOn()
{
    if (!_begun) {
        begin();
        return;
    }
    digitalWrite(PIN_TFT_BACKLIGHT, HIGH);
    _is_on = true;
}

void M5StackILI9341Display::turnOff()
{
    digitalWrite(PIN_TFT_BACKLIGHT, LOW);
    _is_on = false;
}

void M5StackILI9341Display::clear()
{
    frame->fillScreen(kBackground);
}

void M5StackILI9341Display::startFrame(Color bkg)
{
    frame->fillScreen(bkg == DARK ? kBackground : kText);
    _color = kText;
    unicode.setForegroundColor(_color);
    applyFont(1);
    _cursor_x = 0;
    _cursor_y = 0;
}

void M5StackILI9341Display::setTextSize(int sz)
{
    applyFont(sz);
}

int M5StackILI9341Display::fontHeight()
{
    return unicode.getFontAscent() - unicode.getFontDescent();
}

void M5StackILI9341Display::applyFont(int sz)
{
    if (sz >= 2) {
        unicode.setFont(u8g2_font_unifont_t_cyrillic);
    } else {
        unicode.setFont(u8g2_font_5x7_t_cyrillic);
    }
}

void M5StackILI9341Display::setColor(Color c)
{
    switch (c) {
    case DARK:
        _color = kBackground;
        break;
    case LIGHT:
        _color = kText;
        break;
    case RED:
        _color = kError;
        break;
    case GREEN:
        _color = kSuccess;
        break;
    case BLUE:
        _color = kPanel;
        break;
    case YELLOW:
        _color = kWarning;
        break;
    case ORANGE:
        _color = kAccent;
        break;
    default:
        _color = kText;
        break;
    }
    unicode.setForegroundColor(_color);
}

void M5StackILI9341Display::setCursor(int x, int y)
{
    _cursor_x = x;
    _cursor_y = y;
    unicode.setCursor(x, y + unicode.getFontAscent());
}

void M5StackILI9341Display::print(const char *str)
{
    unicode.setCursor(_cursor_x, _cursor_y + unicode.getFontAscent());
    unicode.print(str);
    _cursor_x += unicode.getUTF8Width(str);
}

void M5StackILI9341Display::printWordWrap(const char *str, int max_width)
{
    const char *p = str;
    int x = _cursor_x;
    int y = _cursor_y;
    const int line_height = fontHeight();

    while (*p && y + line_height <= height()) {
        const uint8_t lead = static_cast<uint8_t>(*p);
        size_t char_len = 1;
        if ((lead & 0xE0) == 0xC0)
            char_len = 2;
        else if ((lead & 0xF0) == 0xE0)
            char_len = 3;
        else if ((lead & 0xF8) == 0xF0)
            char_len = 4;

        if (*p == '\n') {
            x = 0;
            y += line_height;
            ++p;
            continue;
        }

        char glyph[5] = {0, 0, 0, 0, 0};
        size_t copied = 0;
        while (copied < char_len && p[copied]) {
            glyph[copied] = p[copied];
            ++copied;
        }
        const int glyph_width = unicode.getUTF8Width(glyph);
        if (x > 0 && x + glyph_width > max_width) {
            x = 0;
            y += line_height;
            if (y + line_height > height())
                break;
        }

        unicode.setCursor(x, y + unicode.getFontAscent());
        unicode.print(glyph);
        x += glyph_width;
        p += copied;
    }

    _cursor_x = x;
    _cursor_y = y;
}

void M5StackILI9341Display::drawTextEllipsized(int x, int y, int max_width, const char *str)
{
    char text[256];
    size_t len = strlen(str);
    if (len >= sizeof(text))
        len = sizeof(text) - 1;
    memcpy(text, str, len);
    text[len] = 0;

    if (getTextWidth(text) <= max_width) {
        setCursor(x, y);
        print(text);
        return;
    }

    const char *ellipsis = "...";
    const int ellipsis_width = getTextWidth(ellipsis);
    while (len > 0 && getTextWidth(text) > max_width - ellipsis_width) {
        do {
            --len;
        } while (len > 0 && (static_cast<uint8_t>(text[len]) & 0xC0) == 0x80);
        text[len] = 0;
    }
    strncat(text, ellipsis, sizeof(text) - strlen(text) - 1);
    setCursor(x, y);
    print(text);
}

void M5StackILI9341Display::translateUTF8ToBlocks(char *dest, const char *src, size_t dest_size)
{
    if (dest_size == 0)
        return;
    size_t len = strlen(src);
    const bool truncated = len >= dest_size;
    if (truncated)
        len = dest_size - 1;
    if (truncated) {
        while (len > 0 && (static_cast<uint8_t>(src[len]) & 0xC0) == 0x80) {
            --len;
        }
    }
    memcpy(dest, src, len);
    dest[len] = 0;
}

void M5StackILI9341Display::fillRect(int x, int y, int w, int h)
{
    frame->fillRect(x, y, w, h, _color);
}

void M5StackILI9341Display::drawRect(int x, int y, int w, int h)
{
    frame->drawRect(x, y, w, h, _color);
}

void M5StackILI9341Display::drawXbm(int x, int y, const uint8_t *bits, int w, int h)
{
    frame->drawXBitmap(x, y, bits, w, h, _color);
}

uint16_t M5StackILI9341Display::getTextWidth(const char *str)
{
    return unicode.getUTF8Width(str);
}

void M5StackILI9341Display::endFrame()
{
    // The 160x120 logical canvas scales exactly 2x to the 320x240 panel. The
    // physical display only sees complete frames, which also prevents flicker.
    const uint16_t *pixels = frame->getBuffer();
    display.startWrite();
    display.setAddrWindow(0, 0, 320, 240);
    for (int out_y = 0; out_y < 240; ++out_y) {
        const int src_y = out_y * height() / 240;
        const uint16_t *src_row = pixels + src_y * width();
        for (int out_x = 0; out_x < 320; ++out_x) {
            scanline[out_x] = src_row[out_x * width() / 320];
        }
        display.writePixels(scanline, 320);
    }
    display.endWrite();
}
