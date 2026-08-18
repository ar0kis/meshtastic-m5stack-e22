#pragma once

#include <helpers/CommonCLI.h>
#include <helpers/ui/DisplayDriver.h>

class UITask
{
    DisplayDriver *_display;
    unsigned long _next_read, _next_refresh, _auto_off;
    int _prevBtnState;
    uint8_t _page;
    NodePrefs *_node_prefs;
    char _version_info[32];

    void renderCurrScreen();
#ifdef M5STACK_CORE_E22
    void renderEnvironmentScreen();
    void renderRadioScreen();
    void renderHeader(const char *title);
    void drawValue(int x, int y, const char *value);
    void drawValueCentered(int middle_x, int y, const char *value);
#endif
  public:
    UITask(DisplayDriver &display) : _display(&display), _page(0) { _next_read = _next_refresh = 0; }
    void begin(NodePrefs *node_prefs, const char *build_date, const char *firmware_version);

    void loop();
};
