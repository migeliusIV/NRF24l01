#include <SPI.h>  // Подключаем библиотеку для работы с SPI-интерфейсом
#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01

#define PIN_POT A1  // номера пинов Arduino, к которым подключены потенциометры
#define pin_secpot A3
#define pin_btn 7   //кнопки
#define pin_zapas 3
#define PIN_CE  9  //  пина Arduino, к которому подключен вывод CE радиомодуля
#define PIN_CSN 10 // Номер пина Arduino, к которому подключен вывод CSN радиомодуля

RF24 radio(PIN_CE, PIN_CSN); //(пульт, так что 9 <-> 10) Создаём объект radio с указанием выводов CE и CSN

int potValue[4]; // Создаём массив для передачи значений потенциометра


void setup() {
  //инициализация пинов кнопок
  pinMode(pin_btn,INPUT);
  pinMode(pin_zapas,INPUT);
  
  //настройка монитора порта, чтобы отслеживать, на каком этапе ошибка
  Serial.begin(9600);
  
  //настройка радио
  radio.begin();  // Инициализация модуля NRF24L01
  radio.setChannel(5); // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setDataRate (RF24_1MBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openWritingPipe(0x7878787878LL); // Открываем трубу с уникальным ID
}


void loop() {
  //считывание 
  potValue[0] = analogRead(PIN_POT)/4; // Считываем показания потенциометра
  potValue[1] = analogRead(pin_secpot)/4;
  potValue[2] = digitalRead(pin_btn);
  potValue[3] = digitalRead(pin_zapas);
  
  Serial.println(potValue[0]);
  Serial.println(potValue[1]);
  
  radio.write(potValue, sizeof(potValue)); // Отправляем считанные показания по радиоканалу
}
