// Timer.h

#ifndef Timer_h
#define Timer_h

#include <stdlib.h>
#if ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
  #include <wiring.h>
#endif

#if (defined(ARDUINO) && ARDUINO >= 155) || defined(ESP8266)
  #define YIELD yield();
#else
  #define YIELD
#endif

class Timer {
  private:
    uint8_t TIME_START = 0;
    uint8_t TIME_STOP = 1;

    uint32_t timeMain;
    uint32_t timeThen;
    unsigned char state;

  public:
    Timer() {
      timeMain = 0;
      state = TIME_START;
    };
    
    Timer(uint32_t _time);

    uint8_t  Now();
    uint32_t Get() const;
    uint32_t Set(uint32_t _time);
    uint32_t Wait();
    uint32_t Reset();
    uint32_t Stop();
    uint32_t Start();

    // ms is better as mks
    //uint32_t setMs(uint32_t _time) { return Set(_time*1000UL); } 
 
    // OOP methods
    //void run(); // to pun into loop()
    //virtual void Process(); // to overload
};

#endif
