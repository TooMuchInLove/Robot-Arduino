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
    
    // Осуществляем вращение модуля;
    void stepRun(uint8_t _state);
    
    // Осуществляем вращение модуля => доводчик;
    void stepRunPlus(uint8_t _state);

    // Возвращаемся в изначальную позицию;
    void stepRunInitPos();

    // Полный шаг (задействуются все обмотки шагового мотора);
    void fullStep(uint16_t _pause);

    // Половина шага (задействуется половина обмоток шагового мотора);
    void halfStep(uint16_t _pause);
    
    // Останавливаем вращение модуля;
    void stepStop();
    
    // Программный таймер;
    void mainTimer(uint16_t _delay);
    
    // Устанавливаем скорость от 100 200 300 (при лимите равном 100);
    void setMaxOfSpeed(uint16_t _speed);
    
    // Устанавливаем количество вращений в секунду;
    void setCountRotate(uint8_t _count);

    // Устанавливаем угол вращения;
    void setDegreeRotate(uint16_t _degreeRotate);
  
  private: // приватные (частные) переменные, функции;
  
    uint8_t _pinPul; // Пин для вращения модуля;
    
    uint8_t _pinDir; // Пин для определения, в какую сторону делать вращение;
    
    uint8_t _pin5vM; // Пин для постоянного питания 5v;
    
    uint8_t _stop = 0; // Определяем состояние модуля (остановлен или вращается);
    
    uint8_t _countRotate = 1; // Количество вращений в секунду;
    
    //* ----------------------------------------------------------------------------- *//
    uint8_t _stepSize = 10; // Размер шага для ускорения;
    uint8_t _limitForBoost = 100; // Предел ускорения;
    uint16_t _minOfSpeed = 0; // Минимальная скорость;
    uint16_t _maxOfSpeed = 100; // Максимальная скорость [100, 200, 300];
    uint16_t _boostOfSpeed = _minOfSpeed; // Увеличение скорости (ускорение);
    uint16_t _delayAdjustment = 2000; // Корректировка задержки [1миллисек];

    int16_t _degreeMain = 0; // Начальное положение модуля;
    
    int16_t _degree = 45; // Заданный угол поворота (2000 = 360 градусов);
    uint32_t _stepCount = 2000; // Общее кол-во шагов за оборот;
    //* ----------------------------------------------------------------------------- *//

    Timer timer0; // Создаём таймер 0;

    Timer timer1; // Создаём таймер 1;

    Timer timer2; // Создаём таймер 2;

    Timer timer3; // Создаём таймер 3;

};

#endif
