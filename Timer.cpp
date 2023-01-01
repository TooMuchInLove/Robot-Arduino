#include "Timer.h"

Timer::Timer(uint32_t _time) {
  timeMain = _time;
  state = TIME_START;
}

uint8_t Timer::Now() {
  uint32_t timeNow = micros();
  // Важно помнить про переполнение целого!
  if (timeNow - timeThen < timeMain)
    return false;
  timeThen = timeNow;
  return (state == TIME_START);
}

uint32_t Timer::Get() const {
  return timeMain;
}

uint32_t Timer::Set(uint32_t _time) {
  uint32_t old = timeMain;
  timeMain = _time;
  return old;
}

uint32_t Timer::Wait() {
  uint32_t old = timeMain;
  timeThen = micros();
  return old;
}

uint32_t Timer::Reset() {
  uint32_t old = timeMain;
  timeThen = 0; // про переполнение целого помнить!
  return old;
}

uint32_t Timer::Start() {
  uint32_t old = timeMain;
  state = TIME_START;
  return old;
}

uint32_t Timer::Stop() {
  uint32_t old = timeMain;
  state = TIME_STOP;
  return old;
}
