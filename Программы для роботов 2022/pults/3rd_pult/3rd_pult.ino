#include <SPI.h>  // Подключаем библиотеку для работы с SPI-интерфейсом
#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01

#define pin_pot_1 A1  // Джойстик 1
#define pin_pot_2 A3  // Джойстик 2
#define pin_btn_A 2   // Направление 1
#define pin_btn_B 3   // <...> Не назначена
#define pin_btn_C 4   // Направление 2
#define pin_btn_D 7   // Выстрел
#define pin_btn_E 8   // slow motion
#define PIN_CE  9  //  пина Arduino, к которому подключен вывод CE радиомодуля
#define PIN_CSN 10 // Номер пина Arduino, к которому подключен вывод CSN радиомодуля

RF24 radio(PIN_CE, PIN_CSN); //(пульт, так что 9 <-> 10) Создаём объект radio с указанием выводов CE и CSN

int potValue[6]; // Создаём массив для передачи значений потенциометра и кнопок

void setup() {
  //инициализация пинов кнопок
  pinMode(pin_btn_A,INPUT);
  pinMode(pin_btn_C,INPUT);
  pinMode(pin_btn_D,INPUT);
  pinMode(pin_btn_E,INPUT);
  //настройка монитора порта, чтобы отслеживать, на каком этапе ошибка
  Serial.begin(9600);
  
  //настройка радио
  radio.begin();  // Инициализация модуля NRF24L01
  radio.setChannel(20); // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setDataRate (RF24_1MBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openWritingPipe(0x5858585858LL); // Открываем трубу с уникальным ID
}


void loop() {
  //считывание 
  potValue[0] = analogRead(pin_pot_1)/4; // Считываем показания потенциометра
  potValue[1] = analogRead(pin_pot_2)/4;
  potValue[2] = digitalRead(pin_btn_A);
  potValue[3] = digitalRead(pin_btn_C);
  potValue[4] = digitalRead(pin_btn_D); // Кнопка выстрела
  potValue[5] = digitalRead(pin_btn_E);
  
  Serial.print(potValue[2]);
  Serial.print(' ');
  Serial.print(potValue[3]);
  Serial.print(' ');
  Serial.println(potValue[5]);

  
  radio.write(potValue, sizeof(potValue)); // Отправляем считанные показания по радиоканалу
}
