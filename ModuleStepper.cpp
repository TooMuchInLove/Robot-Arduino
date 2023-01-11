// ModuleStepper.cpp

#include "ModuleStepper.h"

// Устанавливаем пины в программе;
void ModuleStepper::setPinsMode() {

  pinMode(_pinPul, OUTPUT); // Настройка пина для вращения модуля;
  pinMode(_pinDir, OUTPUT); // Настройка пина для определения, в какую сторону делать вращение;
  pinMode(_pin5vM, OUTPUT); // Настройка пина для питания;
  
  digitalWrite(_pinPul, HIGH); // Настройка пина для вращения модуля;
  digitalWrite(_pin5vM, HIGH); // Постоянное питание пина;

}

// Проверяем, в какую сторону вращать модуль;
void ModuleStepper::setDirection(uint8_t _state) {

  // state=0 [LOW ] -> Вращение против часовой стрелки;
  // state=1 [HIGH] -> Вращение по часовой стрелке;
  digitalWrite(_pinDir, _state);
  if (_state != _stop) // Проверяем смену вращения, если сменили, то ШИМ заного;
    stepStop(); // Останавливаем вращение модуля;
  _stop = _state;

}

// Осуществляем вращение (ВПЕРЁД/НАЗАД или останавливаем модуль);
void ModuleStepper::stepRun(uint8_t _state) {
  
  setDirection(_state); // Направление вращения модуль ВПЕРЁД/НАЗАД;

  _boostOfSpeed += _stepSize; // Увеличение скорости (ускорение);
  if (_maxOfSpeed-(_minOfSpeed+_boostOfSpeed) >= _limitForBoost) // Определяем предел ускорения;
    _boostOfSpeed = _maxOfSpeed; // Ускорение всегда от 0 до limit + step;

  uint8_t state = 0;
  uint32_t pause = _delayAdjustment - (_maxOfSpeed + _boostOfSpeed); // Задержка;
  for (uint16_t turn=0; turn<=_boostOfSpeed/2; turn++) {
    for (uint8_t count=1; count<=_countRotate; count++) {
      digitalWrite(_pinPul, HIGH);
      if (count == _countRotate) mainTimer(pause);
      digitalWrite(_pinPul, LOW);
    }
  }

}

// Осуществляем вращение (влево, вправо или останавливаем модуль) => доводчик;
void ModuleStepper::stepRunPlus(uint8_t _state) {

  setDirection(_state); // Направление вращения модуль ВПЕРЁД/НАЗАД;
  
  uint32_t steps = (_stepCount * _degree) / 360; // Кол-во шагов на заданный градус;
  uint16_t pause = 5000; // Задержка между прокруткой;
  uint16_t boostup = (steps * 20) / 100; // Повышение скорости (плавное ускорение);
  uint16_t slowdown = (steps * 70) / 100; // Понижение скорости (плавное торможение);
  timer0.Set(pause); // Устанавливаем таймер;
  
  if (timer0.Now()) { // Если заданное время (пауза) прошло, то;
    for (uint16_t count=0; count<=steps; count++) {
      if (steps >= 0 && steps <= 10) {
        fullStep(pause); // Полный шаг;
      } else {
        if (count >= 0 && count <= boostup) { // Плавное ускорение;
          pause -= 50;
          fullStep(pause); // Полный шаг;
        } else
        if (count >= boostup+1 && count <= slowdown) { // Нормализация скорости;
          fullStep(pause); // Полный шаг;
        } else
        if (count >= slowdown+1 && count <= steps) { // Плавное торможение;
          pause += 50;
          fullStep(pause); // Полный шаг;
        }
      }
    }
  }
  stepStop(); // Останавливаем вращение;

}

// Возвращаемся в изначальную позицию;
void ModuleStepper::stepRunInitPos() {

  //Serial.println(":"+String(_degreeMain));
  
  /*
  if (_degreeMain == 0)
    return;
  //else
  if (_degreeMain < 0)
    setDirection(0); // Направление вращения модуль ВПЕРЁД;
  //else
  if (_degreeMain > 0)
    setDirection(1); // Направление вращения модуль НАЗАД;

  int16_t h = abs(ceil(_degreeMain / 360));
  int16_t n = 360 * h;
  _degreeMain = n - abs(_degreeMain);

  
  //_degreeMain = abs(_degreeMain); // Кол-во шагов на заданный градус;
  uint16_t pause = 3000; // Задержка между прокруткой;
  fullStep(_degreeMain, pause); // Полный шаг;

  _degreeMain = 0;
  stepStop();
  */

}

// Полный шаг (задействуются все обмотки шагового мотора);
void ModuleStepper::fullStep(uint16_t _pause) {

  digitalWrite(_pinPul, HIGH);
  mainTimer(_pause);
  digitalWrite(_pinPul, LOW);

}

// Половина шага (задействуется половина обмоток шагового мотора);
void ModuleStepper::halfStep(uint16_t _pause) {

  // state = !state;
  // digitalWrite(_pinPul, state);

}

// Программный таймер;
void ModuleStepper::mainTimer(uint16_t _delay) {

  uint32_t timeNow = micros(); // Начальное время;
  while (true) { // Открываем таймер;
    if (micros() - timeNow >= _delay) // Если текущее время - начально = заданной задержке;
      return; // Завершаем подсчёт времени;
  }

}

// Устанавливаем скорость от 100 200 300 (при лимите равном 100);
void ModuleStepper::setMaxOfSpeed(uint16_t _speed) {

  // Устанавливается в зависимости от скорости;
  _speed = _speed * _limitForBoost;
  // Максимальная скорость модуля;
  _minOfSpeed = _speed - _limitForBoost;
  // Максимальная скорость модуля;
  _maxOfSpeed = _speed;

}

// Устанавливаем количество вращений в секунду;
void ModuleStepper::setCountRotate(uint8_t _count) {

  // Количество вращений в секунду;
  _countRotate = _count;

}

// Устанавливаем угол вращения;
void ModuleStepper::setDegreeRotate(uint16_t _degreeRotate) {

  _degree = _degreeRotate;

}

// Останавливаем вращение модуля;
void ModuleStepper::stepStop() {

  // Сбрасываем скорость;
  _boostOfSpeed = _minOfSpeed;
  // Максимальная скорость модуля;
  _minOfSpeed = _maxOfSpeed - _limitForBoost;
  // Останавливаем модуль;
  digitalWrite(_pinPul, HIGH);

}

/*
 * 1    оборот модуля равен count=2000 (360o)
 * 1/2  оборота count=1000 (180o)
 * 1/4  оборота count=500  (90o)
 * 1/8  оборота count=250  (45o)
 * 1/16 оборота count=125  (22.5o)
 * и т.д.
 * 
 * Формула (1 шаг):
 *    - задействуется пара обмоток (2000 шагов = 1 оброт);
 *    - 2000 шагов = 360 градусов => 5.5 шагов = 1 градус)
 *    digitalWrite(_pinPul, HIGH);
 *    digitalWrite(_pinPul, LOW);
 * Формула (1/2 шага):
 *    - задействуется одна обмоток (4000 шагов = 1 оброт);
 *    - задействуется одна обмоток (2000 шагов = пол оброта);
 *    state = !state;
 *    digitalWrite(_pinPul, state);
 */
