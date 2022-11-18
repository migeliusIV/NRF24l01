#include <SPI.h>  // Подключаем библиотеку для работы с SPI-интерфейсом
//#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01
#include <RF24Network.h> // БИБЛИОТЕКА, БЛАГОДАРЯ КОТОРОЙ ОТРГАНИЗОВАНА СЕТЬ 

#define PIN_CE  9  // Номер пина Arduino, к которому подключен вывод CE радиомодуля
#define PIN_CSN 10 // Номер пина Arduino, к которому подключен вывод CSN радиомодуля

RF24 radio(PIN_CE, PIN_CSN); //(пульт, так что 9 <-> 10) Создаём объект radio с указанием выводов CE и CSN
RF24Network network(radio); // Включает радиомодуль в создаваемую сеть
const uint16_t this_node = 02;   // адресс используемого мк в восьмеричной системе ( 04,031, и т.п.)
const uint16_t node_pult = 01;    // адресс ответного мк (ведущего в нашем случае) в восьмеричной системе
const uint16_t node_robot = 0;


void setup() {
  Serial.begin(9600);
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);  //(channel, node address)
  radio.setDataRate(RF24_2MBPS);
}

void loop() {
  network.update();
  //===== Receiving =====//
  while (network.available() ) {     // Is there any incoming data?
    RF24NetworkHeader header;
    unsigned long incomingData;
    network.read(header, &incomingData, sizeof(incomingData)); // Read the incoming data
    if (header.from_node == 0){
      Serial.println(incomingData);// PWM output to LED 01 (dimming)
    }
  }
}
