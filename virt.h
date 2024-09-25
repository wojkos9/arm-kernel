#pragma once

#include "pl031.h"
#include <stdint.h>

volatile struct pl031_rtc *rtc = (void *)0x9010000;
#define RTCIRQ (34u)