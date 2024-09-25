#pragma once
#include <stdint.h>

struct __attribute__((packed)) pl031_rtc  {
    uint32_t dr;
    uint32_t mr;
    uint32_t lr;
    uint32_t cr;
    uint32_t imsc;
    uint32_t ris;
    uint32_t mis;
    uint32_t icr;
};

#define RTCINTR 1