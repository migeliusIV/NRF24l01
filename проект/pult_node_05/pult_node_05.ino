#include <SPI.h>  // Подключаем библиотеку для работы с SPI-интерфейсом
//#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01
//---
#include <RF24Network.h> // БИБЛИОТЕКА, БЛАГОДАРЯ КОТОРОЙ ОТРГАНИЗОВАНА СЕТЬ 
//---

#define pin_pot_1 A1  // Джойстик 1
#define pin_pot_2 A3  // Джойстик 2
#define pin_btn_A 2   // Направление 1
#define pin_btn_B 12   // <...> Не назначена
#define pin_btn_C 4   // Направление 2
#define pin_btn_D 7   // Выстрел
#define pin_btn_SM 5   // slow motion, на версиях младше 2022 поменяйте пины кнопок, которые будут снаружи пульта
#define pin_btn_FM 3   // forsage, кнопку не проверял
#define PIN_CE  9  //  пина Arduino, к которому подключен вывод CE радиомодуля
#define PIN_CSN 10 // Номер пина Arduino, к которому подключен вывод CSN радиомодуля


RF24 radio(PIN_CE, PIN_CSN); //(пульт, так что 9 <-> 10) Создаём объект radio с указанием выводов CE и CSN
//---
RF24Network network(radio); // Включает радиомодуль в создаваемую сеть
const uint16_t this_node = 01;   // адресс используемого мк в восьмеричной системе ( 04,031, и т.п.)
const uint16_t master00 = 0;    // адресс ответного мк (ведущего в нашем случае) в восьмеричной системе
//---

int potValue[8]; // Создаём массив для передачи значений потенциометра и кнопок

//---
const unsigned long interval = 2;  //ms  // How often to send data to the other unit
unsigned long last_sent;            // When did we last send?
//---

void setup() {
  //инициализация пинов кнопок
  pinMode(pin_btn_A,INPUT);
  pinMode(pin_btn_C,INPUT);
  pinMode(pin_btn_D,INPUT);
  pinMode(pin_btn_SM,INPUT_PULLUP);
  pinMode(pin_btn_FM,INPUT_PULLUP);
  //настройка монитора порта, чтобы отслеживать, на каком этапе ошибка
  Serial.begin(9600);
  
  //настройка радио
  //radio.begin();  // Инициализация модуля NRF24L01
  //radio.setChannel(0x6a); 
  //---
  SPI.begin();
  radio.begin();
  network.begin(90, this_node); // Обмен данными будет вестись на 16 канале (2,416 ГГц). Это объясняется тем, что каналы кодируются не в десятеричной системе счисления
  radio.setDataRate (RF24_2MBPS); // Скорость обмена данными 1 Мбит/сек
  //---
  //radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  //radio.openWritingPipe(0x6363636363LL); // Открываем трубу с уникальным ID
}


void loop() {
  //считывание 
  potValue[0] = analogRead(pin_pot_1)/4; // Считываем показания потенциометра. Делим на 4 здесь, а не в 'map' из программы пульта, только чтобы не забыть
  potValue[1] = analogRead(pin_pot_2)/4; // Аналогично для второго колеса
  potValue[2] = digitalRead(pin_btn_A);
  potValue[3] = digitalRead(pin_btn_C);
  potValue[4] = digitalRead(pin_btn_D); // Кнопка выстрела
  potValue[5] = digitalRead(pin_btn_SM);
  potValue[6] = digitalRead(pin_btn_FM);
  potValue[7] = 109;


  network.update();
  //===== Sending =====//
  unsigned long now = millis();
  if (now - last_sent >= interval) {   // If it's time to send a data, send it!
    last_sent = now;
    RF24NetworkHeader header(master00);   // (Address where the data is going)
    bool ok = network.write(header, potValue, sizeof(potValue)); // Send the data

    //Serial.print(potValue[2]);
    //Serial.print(' ');
    //Serial.print(potValue[5]);
    //Serial.print(' ');
    //Serial.println(potValue[6]);
  }
  //radio.write(potValue, sizeof(potValue)); // Отправляем считанные показания по радиоканалу
}
