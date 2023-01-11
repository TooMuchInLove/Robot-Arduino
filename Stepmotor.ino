/* Импортимрование модулей */
#include "ModuleStepper.h"
#include "Timer.h"

/* Порты для подключения модуля ULN2003 к Arduino */
#define PIN_VCC_MOTOR2 2 // Питание 5v;
#define PIN_DIR_MOTOR2 3 // Сторона вращения модуля;
#define PIN_PUL_MOTOR2 4 // Вращение модуля;

#define PIN_VCC_MOTOR1 5 // Питание 5v;
#define PIN_DIR_MOTOR1 6 // Сторона вращения модуля;
#define PIN_PUL_MOTOR1 7 // Вращение модуля;

#define PIN_PUL_PLATFORM 50 // Сторона вращения платформы;

/* Некоторые переменные */
const char EOL = ';'; // Конец строки сообщения;
const uint8_t SIZE_MSG = 15; // Размер сообщения для Arduino;
const uint8_t SIZE_ARR = 18; // Размер массива сообщений для Arduino;
const uint8_t FORWARD = 0; // Вращение ВПЕРЁД;
const uint8_t BACKWARD = 1; // Вращение НАЗАД;

// Counter for count a rotate [Счётчик для подсчёта оборотов];
const uint8_t CFCaR_Min = 10; // Минимальное кол-во вращений для модуля 2;
const uint8_t CFCaR_Max = 50; // Максимальное кол-во вращений для модуля 2;
uint8_t CFCaR_1 = 1;  // Количество вращений для модуля 1;
uint8_t CFCaR_2 = 10; // Количество вращений для модуля 2;

char key; // Команда;
char last_key; // Дополнительная команда;
char MSG[SIZE_ARR][SIZE_MSG] = {
  "STOP",
  // Модуль;
  "FORWARD", "F1", "forward",
  "BACKWARD", "B1", "backward",
  "LEFT1", "LEFT2", "LEFT3", "left",
  "RIGHT1", "RIGHT2", "RIGHT3", "right",
  // Платформа;
  "UP",
  "DOWN",
};

/* Экземпляры классов */
ModuleStepper module1(PIN_PUL_MOTOR1, PIN_DIR_MOTOR1, PIN_VCC_MOTOR1);
ModuleStepper module2(PIN_PUL_MOTOR2, PIN_DIR_MOTOR2, PIN_VCC_MOTOR2);
Timer timer(100); // Создаём таймер на 100 микросекунд;

void setup() {

  module1.setPinsMode(); // Устанавливаем пины для модуля 1;
  module2.setPinsMode(); // Устанавливаем пины для модуля 2;
  Serial.begin(38400); // Скорость передачи данных;
  Serial.setTimeout(50); // Скорость обмена данными между интерфейсами Python и Arduino;

}

void loop() {
  if (Serial.available() > 0) { // Если в буфере есть данные;
    char message[SIZE_MSG]; // Сюда записываем принятое сообщение;
    char speed_step, key_for_slowdown;
    uint8_t num, msg_len;
    num = Serial.readBytesUntil(EOL, message, SIZE_MSG); // Читаем сообщение;
    message[num] = NULL; // Заменяем конец сообщения;
    msg_len = strlen(message); // Фактическая длина сообщения;
    key = message[0]; // Запись первого символа сообщения;
    Serial.flush(); // Очистка буфера (чтобы избежать поступления непонятных символов и данных);
	
    if (key == 'F' || key == 'B') {
      key_for_slowdown = message[msg_len-1];
    }
    Serial.println(String(message)+":"+String(key_for_slowdown));
    if (key == 'L' || key == 'R') {
      speed_step = message[msg_len-1];
      if (speed_step == '1') module2.setMaxOfSpeed(5);
      if (speed_step == '2') module2.setMaxOfSpeed(10);
      if (speed_step == '3') module2.setMaxOfSpeed(15);
    }
    if (timer.Now()) { // Таймер на 100 микросекунд;
      ++CFCaR_2;
    }
    if (CFCaR_2 >= CFCaR_Max) { // Если кол-во вращение превышают допустимое, то;
      CFCaR_2 = CFCaR_Max;
    }

    // Разбиваем сообщение и проверяем нежелательные символы;
    if (key != last_key) { // Если ключ не совпадает с последним ключём;
      for (uint8_t i=0; i<=SIZE_ARR-1; i++) { // Проходим по массиву сообщений;
        if (key != MSG[i][0]) { // Если ключ НЕравен первому символу сообщения, то;
          if (i == SIZE_ARR-1 && key != MSG[i][0]) { // Если не нашли нужный ключ, то;
            key = last_key; // Присваиваем ключу последний ключ;
            break; // Прекращаем поиск;
          }
          continue; // Продолжаем поиск;
        }
        if (key == MSG[i][0]) { // Если ключ равен первому символу сообщения, то;
          if (speed_step != MSG[i][msg_len-1]) // Скорость модуля;
            break;
          if (String(message) != String(MSG[i])) // ;
            key = last_key; // Присваиваем ключу последний ключ;
          break; // Прекращаем поиск;
        }
      }
    }
    last_key = key; // Запоминаем текущий ключ;
	
    switch (key) { // Определяем команду, которая поступила;
      case 'S': // Остановка модуля;
        CFCaR_1 = 1; // Количество вращений для модуля 1;
        CFCaR_2 = CFCaR_Min; // Количество вращений для модуля 2;
        module1.stepStop(); // Остановка модуля 1;
        module2.stepStop(); // Остановка модуля 2;
        break;
      case 'F': // Вращение модуля ВПЕРЁД;
        module1.setCountRotate(CFCaR_1); // Устанавливаем количество вращений;
        if (key_for_slowdown == '1') module1.setSlowDown(); // Плавное торможение модуля 1;
        module1.stepRun(FORWARD); // Вращение модуля 1 ВПЕРЁД;
        break;
      case 'f': // Вращение модуля ВПЕРЁД => ДОВОДЧИК;
        module1.stepRunPlus(FORWARD); // Вращение модуля 1 ВПЕРЁД;
        break;
      case 'B': // Вращение модуля НАЗАД;
        module1.setCountRotate(CFCaR_1); // Устанавливаем количество вращений;
        if (key_for_slowdown == '1') module1.setSlowDown(); // Плавное торможение модуля 1;
        module1.stepRun(BACKWARD); // Вращение модуля 1 НАЗАД;
        break;
      case 'b': // Вращение модуля НАЗАД => ДОВОДЧИК;
        module1.stepRunPlus(BACKWARD); // Вращение модуля 1 НАЗАД;
        break;
      case 'L': // Вращение модуля ВЛЕВО;
        module2.setCountRotate(CFCaR_2); // Устанавливаем количество вращений;
        module2.stepRun(BACKWARD); // Вращение модуля 2 ВЛЕВО;
        break;
      case 'l': // Вращение модуля ВЛЕВО  => ДОВОДЧИК;
        module2.stepRunPlus(BACKWARD); // Вращение модуля 2 ВЛЕВО;
        break;
      case 'R': // Вращение модуля ВПРАВО;
        module2.setCountRotate(CFCaR_2); // Устанавливаем количество вращений;
        module2.stepRun(FORWARD); // Вращение модуля 2 ВПРАВО;
        break;
      case 'r': // Вращение модуля ВПРАВО  => ДОВОДЧИК;
        module2.stepRunPlus(FORWARD); // Вращение модуля 2 ВЛЕВО;
        break;
    }
  }
}
