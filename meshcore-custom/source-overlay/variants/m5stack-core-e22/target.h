#pragma once

#define RADIOLIB_STATIC_ONLY 1
#include "M5StackE22Board.h"
#include <RadioLib.h>
#include <helpers/AutoDiscoverRTCClock.h>
#include <helpers/SensorManager.h>
#include <helpers/radiolib/CustomSX1262Wrapper.h>
#include <helpers/radiolib/RadioLibWrappers.h>

#ifdef DISPLAY_CLASS
#include "M5StackILI9341Display.h"
#include <helpers/ui/MomentaryButton.h>
#endif

extern M5StackE22Board board;
extern WRAPPER_CLASS radio_driver;
extern AutoDiscoverRTCClock rtc_clock;
extern SensorManager sensors;

#ifdef DISPLAY_CLASS
extern DISPLAY_CLASS display;
extern MomentaryButton user_btn;
#endif

bool radio_init();
mesh::LocalIdentity radio_new_identity();
