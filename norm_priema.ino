#include <SPI.h>  // Подключаем библиотеку для работы с SPI-интерфейсом
#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01

#define PIN_bright 3  // Номер пина Arduino, к которому подключен светодиод
#define pin_state 5
#define pin_btn_st 2 
#define pin_btn_nd 4
#define PIN_CE  10  // Номер пина Arduino, к которому подключен вывод CE радиомодуля
#define PIN_CSN 9 // Номер пина Arduino, к которому подключен вывод CSN радиомодуля

RF24 radio(PIN_CE, PIN_CSN); // Создаём объект radio с указанием выводов CE и CSN

int potValue[4]; // Создаём массив для приёма значений потенциометра

void setup() {
  Serial.begin(9600);
  pinMode(pin_btn_st, OUTPUT);
  pinMode(pin_btn_nd, OUTPUT);
  pinMode(PIN_bright, OUTPUT); // Настраиваем на выход пин светодиода
  pinMode(pin_state, OUTPUT); 
  radio.begin();  // Инициализация модуля NRF24L01
  radio.setChannel(5); // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setDataRate (RF24_1MBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openReadingPipe (1, 0x7878787878LL); // Открываем трубу ID передатчика
  radio.startListening(); // Начинаем прослушивать открываемую трубу
}

void loop() {
  if(radio.available()){ // Если в буфер приёмника поступили данные
    radio.read(&potValue, sizeof(potValue)); // Читаем показания потенциометра
    analogWrite(PIN_bright, potValue[0]); // Регулируем яркость диода
    analogWrite(pin_state, potValue[1]);
    digitalWrite(pin_btn_st,potValue[2]);
    digitalWrite(pin_btn_nd,potValue[3]);
  }
}
