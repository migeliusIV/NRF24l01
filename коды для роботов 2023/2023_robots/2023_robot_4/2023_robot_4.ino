#include <SPI.h>  // Подключаем библиотеку для работы с SPI-интерфейсом
#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01
#include <Servo.h>

#define PIN_ENA 3 // Вывод управления скоростью вращения мотора №1
#define PIN_ENB 6 // Вывод управления скоростью вращения мотора №2
#define PIN_IN1 2 // Вывод управления направлением вращения мотора №1
#define PIN_IN2 4 // Вывод управления направлением вращения мотора №1
#define PIN_IN3 5 // Вывод управления направлением вращения мотора №2
#define PIN_IN4 7 // Вывод управления направлением вращения мотора №2
#define pin_servo 12

// ПОЛОЖЕНИЯ СЕРВОПРИВОДА (Устанавливаются эксперементально для каждого робота. По причине того, что деталь к сервоприводу крепится под разным углом)
#define servo_start 0
#define servo_second 120
#define servo_ready 40


#define PIN_CE  9  // Номер пина Arduino, к которому подключен вывод CE радиомодуля
#define PIN_CSN 10 // Номер пина Arduino, к которому подключен вывод CSN радиомодуля

RF24 radio(PIN_CE, PIN_CSN); // Создаём объект radio с указанием выводов CE и CSN
Servo shooter;
int potValue[7]; // Создаём массив для приёма значений 
bool motor_direction = true;
bool shoot = false; // Нажата ли кнопка выстрела?
bool servo_is_ready = false; // Готова ли стрелялка к выстрелу?
uint8_t power = 0; // Значение ШИМ (или скорости вращения)




void setup() {
  Serial.begin(9600);

  // Установка всех управляющих пинов в режим выхода
  pinMode(PIN_ENA, OUTPUT); //левое колесо на аналог
  pinMode(PIN_ENB, OUTPUT); //правое колесо на аналог
  pinMode(PIN_IN1, OUTPUT); 
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);

  // Команда остановки двум моторам
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, LOW);

  // Настройка радиоприёма
  radio.begin();  // Инициализация модуля NRF24L01
  radio.setChannel(69); // Обмен данными будет вестись на 16 канале (2,416 ГГц)
  radio.setDataRate (RF24_1MBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openReadingPipe (1, 0x6868686868LL); // Открываем трубу ID передатчика
  radio.startListening(); // Начинаем прослушивать открываемую трубу
  
  shooter.attach(pin_servo);
  // Инициализация стрелялки: нулевое положение(servo_start 0) -> положение взвода курка(servo_second 120) -> положение на изготове(servo_ready 30)
  for (int i = servo_start; i <= servo_second; i++) {
    shooter.write(i);
    delay(15);
  }
  for (int i = servo_second; i >= servo_ready; i--) {
    shooter.write(i);
    delay(15);
  }
  servo_is_ready = true; // Стрелялка готова к работе
}



void loop() {
  if (radio.available()) {  // Если в буфер приёмника поступили данные
    radio.read(&potValue, sizeof(potValue));    // Читаем показания с пульта
    // Изначально было два варианта. Первый - прописать действия в зависимости от джойстика для каждого положения, но так моторы работали ассинхронно.
    // Поэтому было решено прописать действия ситуационно. Оба джойстика вверх, два случая на разные напрваления джойстиков и случай, когда оба вниз.
    // Программы для управления моторами описаны в функциях: command_slowmode, command_motor, command_forsagemode. Подробнее о них прочитаете в void.
    
    if (potValue[2] == 0) {
      motor_direction = true;
    } else if (potValue[3] == 0) {
      motor_direction = false;
    } else {
      motor_direction = motor_direction;
    }
    
    if (potValue[4] == 0) {
      shoot = true;
    }

    if (potValue[5] == 0){
      command_slowmode();
    } else if (potValue[6] == 0){
      command_forsagemode();
    } else {
      command_motor();
    }
    
    command_shooter();
    
  }
}

void read_data_motors() {
  if (radio.available()) {  // Если в буфер приёмника поступили данные
    radio.read(&potValue, sizeof(potValue));    // Читаем показания с пульта
    //изначально было два варианта. первый - прописать действия в зависимости от джойстика для каждого положения, но так моторы работали ассинхронно.
    // поэтому было решено прописать действия ситуационно:  4 случая, приведённые ниже
    if (potValue[2] == 0) {
      motor_direction = true;
    } else if (potValue[3] == 0) {
      motor_direction = false;
    } else {
      motor_direction = motor_direction;
    }
  }
}

void command_slowmode() {
  // Конвертирование значений с потенциометров на моторы в режиме slow mode (для манёвров, требующих бОльшей осторожности, потом поймёте)
  
  if (motor_direction) {
    if (potValue[0] > 125 && potValue[1] > 125) {
      analogWrite(PIN_ENA, map(potValue[1], 126, 255, 0, 70));
      analogWrite(PIN_ENB, map(potValue[0], 126, 255, 0, 70));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, HIGH);
      digitalWrite(PIN_IN2, LOW);
      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, HIGH);
      digitalWrite(PIN_IN4, LOW);

    } else if (potValue[1] < 125 && potValue[0] > 125) {
      analogWrite(PIN_ENA, map(potValue[1], 0, 125, 70, 0));
      analogWrite(PIN_ENB, map(potValue[0], 126, 255, 0, 70));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, HIGH);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, HIGH);
      digitalWrite(PIN_IN4, LOW);
    } else if (potValue[1] > 125 && potValue[0] < 125) {
      analogWrite(PIN_ENA, map(potValue[1], 126, 255, 0, 70));
      analogWrite(PIN_ENB, map(potValue[0], 0, 125, 70, 0));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, HIGH);
      digitalWrite(PIN_IN2, LOW);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, LOW);
      digitalWrite(PIN_IN4, HIGH);
    } else if (potValue[0] < 125 && potValue[1] < 125) {
      analogWrite(PIN_ENA, map(potValue[1], 0, 125, 70, 0));
      analogWrite(PIN_ENB, map(potValue[0], 0, 125, 70, 0));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, HIGH);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, LOW);
      digitalWrite(PIN_IN4, HIGH);
    }
  } else {
    if (potValue[0] > 125 && potValue[1] > 125) {
      analogWrite(PIN_ENA, map(potValue[0], 126, 255, 0, 70));
      analogWrite(PIN_ENB, map(potValue[1], 126, 255, 0, 70));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, HIGH);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, LOW);
      digitalWrite(PIN_IN4, HIGH);
    } else if (potValue[0] < 125 && potValue[1] > 125) {
      analogWrite(PIN_ENA, map(potValue[0], 0, 125, 70, 0));
      analogWrite(PIN_ENB, map(potValue[1], 126, 255, 0, 70));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, HIGH);
      digitalWrite(PIN_IN2, LOW);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, LOW);
      digitalWrite(PIN_IN4, HIGH);
    } else if (potValue[0] > 125 && potValue[1] < 125) {
      analogWrite(PIN_ENA, map(potValue[0], 126, 255, 0, 70));
      analogWrite(PIN_ENB, map(potValue[1], 0, 125, 70, 0));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, HIGH);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, HIGH);
      digitalWrite(PIN_IN4, LOW);
    } else if (potValue[1] < 125 && potValue[0] < 125) {
      analogWrite(PIN_ENA, map(potValue[0], 0, 125, 70, 0));
      analogWrite(PIN_ENB, map(potValue[1], 0, 125, 70, 0));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, HIGH);
      digitalWrite(PIN_IN2, LOW);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, HIGH);
      digitalWrite(PIN_IN4, LOW);
    }
  }
}

void command_motor() { 
  // Конвертирование значений с потенциометров на моторы в режиме обычного движения
  // (совсем не всегда удобно передвигаться на максимальной скорости, даже с учётом того, что она и так небольшая)
  if (motor_direction) {
    if (potValue[0] > 125 && potValue[1] > 125) {
      analogWrite(PIN_ENA, map(potValue[1], 126, 255, 0, 125));
      analogWrite(PIN_ENB, map(potValue[0], 126, 255, 0, 125));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, HIGH);
      digitalWrite(PIN_IN2, LOW);
      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, HIGH);
      digitalWrite(PIN_IN4, LOW);

    } else if (potValue[1] < 125 && potValue[0] > 125) {
      analogWrite(PIN_ENA, map(potValue[1], 0, 125, 125, 0));
      analogWrite(PIN_ENB, map(potValue[0], 126, 255, 0, 125));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, HIGH);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, HIGH);
      digitalWrite(PIN_IN4, LOW);
    } else if (potValue[1] > 125 && potValue[0] < 125) {
      analogWrite(PIN_ENA, map(potValue[1], 126, 255, 0, 125));
      analogWrite(PIN_ENB, map(potValue[0], 0, 125, 125, 0));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, HIGH);
      digitalWrite(PIN_IN2, LOW);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, LOW);
      digitalWrite(PIN_IN4, HIGH);
    } else if (potValue[0] < 125 && potValue[1] < 125) {
      analogWrite(PIN_ENA, map(potValue[1], 0, 125, 125, 0));
      analogWrite(PIN_ENB, map(potValue[0], 0, 125, 125, 0));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, HIGH);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, LOW);
      digitalWrite(PIN_IN4, HIGH);
    }
  } else {
    if (potValue[0] > 125 && potValue[1] > 125) {
      analogWrite(PIN_ENA, map(potValue[0], 126, 255, 0, 125));
      analogWrite(PIN_ENB, map(potValue[1], 126, 255, 0, 125));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, HIGH);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, LOW);
      digitalWrite(PIN_IN4, HIGH);
    } else if (potValue[0] < 125 && potValue[1] > 125) {
      analogWrite(PIN_ENA, map(potValue[0], 0, 125, 125, 0));
      analogWrite(PIN_ENB, map(potValue[1], 126, 255, 0, 125));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, HIGH);
      digitalWrite(PIN_IN2, LOW);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, LOW);
      digitalWrite(PIN_IN4, HIGH);
    } else if (potValue[0] > 125 && potValue[1] < 125) {
      analogWrite(PIN_ENA, map(potValue[0], 126, 255, 0, 125));
      analogWrite(PIN_ENB, map(potValue[1], 0, 125, 125, 0));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, HIGH);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, HIGH);
      digitalWrite(PIN_IN4, LOW);
    } else if (potValue[1] < 125 && potValue[0] < 125) {
      analogWrite(PIN_ENA, map(potValue[0], 0, 125, 125, 0));
      analogWrite(PIN_ENB, map(potValue[1], 0, 125, 125, 0));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, HIGH);
      digitalWrite(PIN_IN2, LOW);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, HIGH);
      digitalWrite(PIN_IN4, LOW);
    }
  }
}

void command_forsagemode() { 
  // Конвертирование значений с потенциометров на моторы в режиме forsage mode (для передвижения во время матча по полю)
  if (motor_direction) {
    if (potValue[0] > 125 && potValue[1] > 125) {
      analogWrite(PIN_ENA, map(potValue[1], 126, 255, 0, 255));
      analogWrite(PIN_ENB, map(potValue[0], 126, 255, 0, 255));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, HIGH);
      digitalWrite(PIN_IN2, LOW);
      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, HIGH);
      digitalWrite(PIN_IN4, LOW);

    } else if (potValue[1] < 125 && potValue[0] > 125) {
      analogWrite(PIN_ENA, map(potValue[1], 0, 125, 255, 0));
      analogWrite(PIN_ENB, map(potValue[0], 126, 255, 0, 255));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, HIGH);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, HIGH);
      digitalWrite(PIN_IN4, LOW);
    } else if (potValue[1] > 125 && potValue[0] < 125) {
      analogWrite(PIN_ENA, map(potValue[1], 126, 255, 0, 255));
      analogWrite(PIN_ENB, map(potValue[0], 0, 125, 255, 0));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, HIGH);
      digitalWrite(PIN_IN2, LOW);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, LOW);
      digitalWrite(PIN_IN4, HIGH);
    } else if (potValue[0] < 125 && potValue[1] < 125) {
      analogWrite(PIN_ENA, map(potValue[1], 0, 125, 255, 0));
      analogWrite(PIN_ENB, map(potValue[0], 0, 125, 255, 0));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, HIGH);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, LOW);
      digitalWrite(PIN_IN4, HIGH);
    }
  } else {
    if (potValue[0] > 125 && potValue[1] > 125) {
      analogWrite(PIN_ENA, map(potValue[0], 126, 255, 0, 255));
      analogWrite(PIN_ENB, map(potValue[1], 126, 255, 0, 255));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, HIGH);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, LOW);
      digitalWrite(PIN_IN4, HIGH);
    } else if (potValue[0] < 125 && potValue[1] > 125) {
      analogWrite(PIN_ENA, map(potValue[0], 0, 125, 255, 0));
      analogWrite(PIN_ENB, map(potValue[1], 126, 255, 0, 255));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, HIGH);
      digitalWrite(PIN_IN2, LOW);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, LOW);
      digitalWrite(PIN_IN4, HIGH);
    } else if (potValue[0] > 125 && potValue[1] < 125) {
      analogWrite(PIN_ENA, map(potValue[0], 126, 255, 0, 255));
      analogWrite(PIN_ENB, map(potValue[1], 0, 125, 255, 0));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, HIGH);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, HIGH);
      digitalWrite(PIN_IN4, LOW);
    } else if (potValue[1] < 125 && potValue[0] < 125) {
      analogWrite(PIN_ENA, map(potValue[0], 0, 125, 255, 0));
      analogWrite(PIN_ENB, map(potValue[1], 0, 125, 255, 0));

      // Задаём направление для 1-го мотора
      digitalWrite(PIN_IN1, HIGH);
      digitalWrite(PIN_IN2, LOW);

      // Задаём направление для 2-го мотора
      digitalWrite(PIN_IN3, HIGH);
      digitalWrite(PIN_IN4, LOW);
    }
  }
}

void command_shooter() {
  // Обработка команд для стрелялки
  // Чтобы полностью не парализовать робота в момент активации катапульты (delay(...) останаваливает работу программы на время, указанное в скобках в мс)
  // мы прямо в цикле каждый раз выполняем хотя бы какое-то движение.
  
  if (shoot && servo_is_ready) {
    shoot = false;
    servo_is_ready = false;
    for (int i = servo_ready; i >= servo_start; i--) {
      shooter.write(i);
      read_data_motors();

      // Вот эти вставки позволяют нам сохранить работоспособность
      if (potValue[5] == 0){
        command_slowmode();
      } else if (potValue[6] == 0){
        command_forsagemode();
      } else {
        command_motor();
      }
      delay(13);
    }
    
  } else if (!servo_is_ready) {
    for (int i = servo_start; i <= servo_second; i++) {
      shooter.write(i);
      read_data_motors();

      // Вот эти
      if (potValue[5] == 0){
        command_slowmode();
      } else if (potValue[6] == 0){
        command_forsagemode();
      } else {
        command_motor();
      }
      delay(13);
    }
    
    for (int i = servo_second; i >= servo_ready; i--) {
      shooter.write(i);
      read_data_motors();

      // Вот эти, если вы ещё не поняли
      if (potValue[5] == 0){
        command_slowmode();
      } else if (potValue[6] == 0){
        command_forsagemode();
      } else {
        command_motor();
      }
      delay(13);
    }
    servo_is_ready = true;
  }
}
