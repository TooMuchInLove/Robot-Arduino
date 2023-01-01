/* Импортимрование модулей */
//#include "PlatformStepper.h"
#include "ModuleStepper.h"
#include "Timer.h"

/* Порты для подключения модуля ULN2003 к Arduino */
#define PIN_VCC_MOTOR2  2 // Питание 5v;
#define PIN_DIR_MOTOR2  3 // Сторона вращения модуля;
#define PIN_PUL_MOTOR2  4 // Вращение модуля;

#define PIN_VCC_MOTOR1  5 // Питание 5v;
#define PIN_DIR_MOTOR1  6 // Сторона вращения модуля;
#define PIN_PUL_MOTOR1  7 // Вращение модуля;

#define pin5v_R 8 // Питание 5v;
#define pinRel1 9 // pin controll rele 1;

//#define pinRel2 10 // pin controll rele 2;
//#define pinRel3 11 // pin controll rele 3;
//#define pinRel4 12 // pin controll rele 4;

/* Некоторые переменные */
const char SEP = ','; // Разделитель подстрок сообщения;
const char EOL = ';'; // Конец строки сообщения;
const uint8_t LENGTH_MSG = 15; // Длина сообщения для Arduino;
const uint8_t LENGTH_ARR = 19; // Длина массива сообщений для Arduino;

const uint8_t FORWARD = 0; // Вращение ВПЕРЁД;
const uint8_t BACKWARD = 1; // Вращение НАЗАД;
const uint8_t countMin = 5; // ;
const uint8_t countMax = 100; // ;

uint8_t countR1 = 10; // Количество вращений;
uint8_t countR2 = countMin; // Количество вращений;
uint16_t degree = 1;

char key; // Команда;
char last_key; // Дополнительная команда;
char MSG[LENGTH_ARR][LENGTH_MSG] = {
  "STOP",
  "FORWARD", "forward",
  "BACKWARD", "backward",
  "LEFT1", "LEFT2", "LEFT3", "left",
  "RIGHT1", "RIGHT2", "RIGHT3", "right",
  "Q-FORWARDLEFT", "E-FORWARDRIGHT",
  "Z-BACKWARDLEFT", "X-BACKWARDRIGHT",
  "INITIALPOS",
  "P",
};

uint32_t mainTime = 0; // Время начала работы модуля;
uint32_t timeEnd0 = 0; // Время окончания работы модуля;
uint32_t timeEnd1 = 0; // Время окончания работы модуля;
//uint32_t timeEnd2 = 0; // Время окончания работы модуля;
//uint32_t timeEnd3 = 0; // Время окончания работы модуля;
//uint32_t timeEnd4 = 0; // Время окончания работы модуля;


/* Экземпляры классов */
ModuleStepper module1(PIN_PUL_MOTOR1, PIN_DIR_MOTOR1, PIN_VCC_MOTOR1);
ModuleStepper module2(PIN_PUL_MOTOR2, PIN_DIR_MOTOR2, PIN_VCC_MOTOR2);
Timer timer(100); // Создаём таймер на 100 микросекунд;;

void setup() {

  module1.setPinsMode(); // Устанавливаем пины для модуля 1;
  module2.setPinsMode(); // Устанавливаем пины для модуля 2;
  Serial.begin(38400); // Скорость передачи данных; 115200 38400 9600
  Serial.setTimeout(10); // Скорость обмена данными между интерфейсами Python и Arduino;

}

void loop() {
  
  mainTime = millis();
  ///*
  if (Serial.available() > 0) { // Если в буфере есть данные;
    ///*
    char message[LENGTH_MSG]; // Сюда записываем принятое сообщение;
    char speed_step;
    uint8_t num = Serial.readBytesUntil(EOL, message, LENGTH_MSG); // Читаем сообщение;
    message[num] = NULL; // Заменяем конец сообщения;
    uint8_t len_msg = strlen(message); // Фактическая длина сообщения;
    key = message[0]; // Запись первого символа сообщения;    
    Serial.flush(); // Очистка буфера (чтобы избежать поступления непонятных символов и данных);
    //Serial.print(String(message));
    //*/

    /*
    if (key == 'P') {
      degree = Serial.parseInt();
      Serial.println(String(degree)+"--------");
    }
    /*/

    //*
    if (key == 'L' || key == 'R') {
      speed_step = message[len_msg-1];
      //Serial.println(speed_step);
      if (speed_step == '1')
        module2.setMaxOfSpeed(1);
      if (speed_step == '2')
        module2.setMaxOfSpeed(3);
      if (speed_step == '3')
        module2.setMaxOfSpeed(5);
    }
    //*/

    //* Разбиваем сообщение и проверяем нежелательные символы;
    if (key != last_key) { // Если ключ не совпадает с последним ключём;
      for (uint8_t i=0; i<=LENGTH_ARR-1; i++) { // Проходим по массиву сообщений;
        if (key != MSG[i][0]) { // Если ключ НЕравен первому символу сообщения, то;
          if (i == LENGTH_ARR-1 && key != MSG[i][0]) { // Если не нашли нужный ключ, то;
            key = last_key; // Присваиваем ключу последний ключ;
            break; // Прекращаем поиск;
          }
          continue; // Продолжаем поиск;
        }
        if (key == MSG[i][0]) { // Если ключ равен первому символу сообщения, то;
          if (speed_step != MSG[i][len_msg-1]) // Скорость модуля;
            break;
          if (String(message) != String(MSG[i])) // ;
            key = last_key; // Присваиваем ключу последний ключ;
          break; // Прекращаем поиск;
        }
      }
    }
    last_key = key; // Запоминаем текущий ключ;
    //*/
    
    if (timer.Now()) // Таймер на 100 микросекунд;
      countR2 += 1;
    if (countR2 >= countMax)
      countR2 = countMax;
    
    ///*
    switch (key) { // Определяем команду, которая поступила;
      case 'P':
        module1.setDegreeRotate(degree);
        module2.setDegreeRotate(degree);
        break;
      case 'S': // Остановка модуля;
        module1.stepStop(); // Остановка модуля 1;
        module2.stepStop(); // Остановка модуля 2;
        //countR1 = 10; // ;
        countR2 = countMin; // ;
        break;
      case 'I':
        module1.stepRunInitPos();
        break;
      case 'Q':
        module1.setCountRotate(1); // Устанавливаем количество вращений;
        module1.stepRun(FORWARD); // Вращение модуля 1 ВПЕРЁД;
        module2.setCountRotate(countR2); // Устанавливаем количество вращений;
        module2.stepRun(BACKWARD); // Вращение модуля 2 ВЛЕВО;
        break;
      case 'E':
        module1.setCountRotate(1); // Устанавливаем количество вращений;
        module1.stepRun(FORWARD); // Вращение модуля 1 ВПЕРЁД;
        module2.setCountRotate(countR2); // Устанавливаем количество вращений;
        module2.stepRun(FORWARD); // Вращение модуля 2 ВЛЕВО;
        break;
      case 'Z':
        module1.setCountRotate(1); // Устанавливаем количество вращений;
        module1.stepRun(BACKWARD); // Вращение модуля 1 ВПЕРЁД;
        module2.setCountRotate(countR2); // Устанавливаем количество вращений;
        module2.stepRun(BACKWARD); // Вращение модуля 2 ВЛЕВО;
        break;
      case 'X':
        module1.setCountRotate(1); // Устанавливаем количество вращений;
        module1.stepRun(BACKWARD); // Вращение модуля 1 ВПЕРЁД;
        module2.setCountRotate(countR2); // Устанавливаем количество вращений;
        module2.stepRun(FORWARD); // Вращение модуля 2 ВЛЕВО;
        break;
      case 'F': // Вращение модуля ВПЕРЁД;
        module1.setCountRotate(1); // Устанавливаем количество вращений;
        module1.stepRun(FORWARD); // Вращение модуля 1 ВПЕРЁД;
        break;
      case 'f': // Вращение модуля ВПЕРЁД => ДОВОДЧИК;
        module1.stepRunPlus(FORWARD); // Вращение модуля 1 ВПЕРЁД;
        break;
      case 'B': // Вращение модуля НАЗАД;
        module1.setCountRotate(1); // Устанавливаем количество вращений;
        module1.stepRun(BACKWARD); // Вращение модуля 1 НАЗАД;
        break;
      case 'b': // Вращение модуля НАЗАД => ДОВОДЧИК;
        module1.stepRunPlus(BACKWARD); // Вращение модуля 1 НАЗАД;
        break;
      case 'L': // Вращение модуля ВЛЕВО;
        module2.setCountRotate(countR2); // Устанавливаем количество вращений;
        module2.stepRun(BACKWARD); // Вращение модуля 2 ВЛЕВО;
        break;
      case 'l': // Вращение модуля ВЛЕВО  => ДОВОДЧИК;
        module2.stepRunPlus(BACKWARD); // Вращение модуля 2 ВЛЕВО;
        break;
      case 'R': // Вращение модуля ВПРАВО;
        module2.setCountRotate(countR2); // Устанавливаем количество вращений;
        module2.stepRun(FORWARD); // Вращение модуля 2 ВПРАВО;
        break;
      case 'r': // Вращение модуля ВПРАВО  => ДОВОДЧИК;
        module2.stepRunPlus(FORWARD); // Вращение модуля 2 ВЛЕВО;
        break;
    }
    //*/
  } else {
    //
  }
  //*/
}

void getBoostOfSpeed() {

  if (millis() - timeEnd0 >= 100) {
    timeEnd0 = mainTime;
    countR2 += 1;
  } else
  if (millis() - timeEnd0 >= 200) {
    timeEnd0 = mainTime;
    countR2 += 1;
  } else
  if (millis() - timeEnd0 >= 300) {
    timeEnd0 = mainTime;
    countR2 += 1;
  } else
  if (millis() - timeEnd0 >= 400) {
    timeEnd0 = mainTime;
    countR2 += 1;
  } else
  if (millis() - timeEnd0 >= 500) {
    timeEnd0 = mainTime;
    countR2 += 1;
  } else
  if (millis() - timeEnd0 >= 800) {
    timeEnd0 = mainTime;
    countR2 += 2;
  } else
  if (millis() - timeEnd0 >= 1100) {
    timeEnd0 = mainTime;
    countR2 += 2;
  } else
  if (millis() - timeEnd0 >= 1400) {
    timeEnd0 = mainTime;
    countR2 += 2;
  } else
  if (millis() - timeEnd0 >= 1900) {
    timeEnd0 = mainTime;
    countR2 += 3;
  } else
  if (millis() - timeEnd0 >= 2400) {
    timeEnd0 = mainTime;
    countR2 += 3;
  } else
  if (millis() - timeEnd0 >= 3100) {
    timeEnd0 = mainTime;
    countR2 += 4;
  } else
  if (millis() - timeEnd0 >= 3800) {
    timeEnd0 = mainTime;
    countR2 += 4;
  } else
  if (millis() - timeEnd0 >= 4700) { // if countMax == 30
    timeEnd0 = mainTime;
    countR2 += 5;
  } else
  if (millis() - timeEnd0 >= 5600) {
    timeEnd0 = mainTime;
    countR2 += 5;
  } else
  if (millis() - timeEnd0 >= 7000) { // if countMax == 50
    timeEnd0 = mainTime;
    countR2 += 15;
  } else
  if (millis() - timeEnd0 >= 9000) {
    timeEnd0 = mainTime;
    countR2 += 20;
  } else
  if (millis() - timeEnd0 >= 12000) { // if countMax == 100
    timeEnd0 = mainTime;
    countR2 += 30;
  }
  if (countR2 >= countMax) {
    countR2 = countMax;
  }

}
