#ifndef THREADS
#define THREADS

#include "thread_blink.hpp"
#include "thread_bmp180.hpp"
#include "thread_bme280.hpp"

extern BlinkThread  blinkThread;
extern Bmp180Thread bmp180Thread;
extern Bme280Thread bme280Thread;

#endif // THREADS
