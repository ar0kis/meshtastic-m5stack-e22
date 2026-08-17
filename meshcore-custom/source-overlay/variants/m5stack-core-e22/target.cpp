#include "target.h"
#include <Arduino.h>

M5StackE22Board board;

RADIO_CLASS radio = new Module(P_LORA_NSS, P_LORA_DIO_1, P_LORA_RESET, P_LORA_BUSY, SPI);
WRAPPER_CLASS radio_driver(radio, board);

ESP32RTCClock fallback_clock;
AutoDiscoverRTCClock rtc_clock(fallback_clock);
SensorManager sensors;

#ifdef DISPLAY_CLASS
DISPLAY_CLASS display;
MomentaryButton user_btn(PIN_USER_BTN, 1000, true, true);
#endif

bool radio_init()
{
    fallback_clock.begin();
    rtc_clock.begin(Wire);
    return radio.std_init(&SPI);
}

mesh::LocalIdentity radio_new_identity()
{
    RadioNoiseListener rng(radio);
    return mesh::LocalIdentity(&rng);
}
