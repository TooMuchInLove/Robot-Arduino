// ModuleStepper.h

#ifndef ModuleStepper_h
#define ModuleStepper_h

#include "Timer.h"

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

// Управление модулем;
class ModuleStepper {

  public: // Общедоступные (публичные) переменные, функции;
    
    // Конструктор класса ModuleStepper;
    ModuleStepper(uint8_t pinPul, uint8_t pinDir, uint8_t pin5vM) {
    
      _pinPul = pinPul; // Пин для вращения модуля;
      _pinDir = pinDir; // Пин для определения, в какую сторону делать вращение;
      _pin5vM = pin5vM; // Пин для постоянного питания 5v;
    
    }
    
    // Устанавливаем пины в программе;
    void setPinsMode();
    
    // Проверяем, в какую сторону вращать модуль;
    void setDirection(uint8_t _state);

    // Плавное торможение модуля при отпускании клавиши;
    void setSlowDown();
    
    // Осуществляем вращение модуля;
    void stepRun(uint8_t _state);
    
    // Осуществляем вращение модуля => доводчик;
    void stepRunPlus(uint8_t _state);

    // Полный шаг (задействуются все обмотки шагового мотора);
    void fullStep(uint16_t _pause);

    // Половина шага (задействуется половина обмоток шагового мотора);
    void halfStep(uint16_t _pause);
    
    // Устанавливаем скорость от 100 200 300 (при лимите равном 100);
    void setMaxOfSpeed(uint16_t _speed);
    
    // Устанавливаем количество вращений в секунду;
    void setCountRotate(uint8_t _count);

    // Программный таймер;
    void mainTimer(uint16_t _delay);

    // Останавливаем вращение модуля;
    void stepStop();
  
  private: // приватные (частные) переменные, функции;
  
    uint8_t _pinPul; // Пин для вращения модуля;
    
    uint8_t _pinDir; // Пин для определения, в какую сторону делать вращение;
    
    uint8_t _pin5vM; // Пин для постоянного питания 5v;
    
    uint8_t _stop = 0; // Определяем состояние модуля (остановлен или вращается);
    
    uint8_t _countRotate = 1; // Количество вращений в секунду;
    
    uint8_t _slowdown = 0; // Плавное торможение модуля [ВЫКЛ];
    
    uint8_t _stepSize = 10; // Размер шага для ускорения;
	
    uint8_t _limitForBoost = 100; // Предел ускорения;
	
    uint16_t _minOfSpeed = 0; // Минимальная скорость;
	
    uint16_t _maxOfSpeed = 100; // Максимальwная скорость [100, 200, 300];
	
    uint16_t _boostOfSpeed = _minOfSpeed; // Увеличение скорости (ускорение);
    
    int16_t _degree = 1; // Заданный угол поворота (360 градусов = 2000 шагов);
	
    uint32_t _stepCount = 2000; // Общее кол-во шагов за оборот;

    Timer timer0; // Создаём таймер 0;

    Timer timer1; // Создаём таймер 1;

    Timer timer2; // Создаём таймер 2;

    Timer timer3; // Создаём таймер 3;

};

#endif
