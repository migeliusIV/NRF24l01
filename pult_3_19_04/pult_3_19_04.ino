#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#define axis_X 0
#define axis_Y 1
#define but_pin 5
#define BUTTON_DOWN 4

#define timeout 200
#define MOTOR_MAX 255
#define JOY_MAX 1024

#define ChannelNUM 0x79

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно
//RF24 radio(9,53); // для Меги

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

byte transmit_data[6]; // массив, хранящий передаваемые данные
byte latest_data[6]; // массив, хранящий последние переданные данные
boolean flag; // флажок отправки данных
int deadZone = 5;
bool r_tow, l_tow;

void setup() {
  Serial.begin(9600); //открываем порт для связи с ПК

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 25);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(6);     //размер пакета, в байтах

  radio.openWritingPipe(address[0]);   //мы - труба 0, открываем канал для передачи данных
  radio.setChannel(ChannelNUM);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.stopListening();  //не слушаем радиоэфир, мы передатчик

  pinMode(but_pin, INPUT);
  digitalWrite(but_pin, HIGH);
  pinMode(BUTTON_DOWN, INPUT);
  digitalWrite(BUTTON_DOWN, HIGH);
}
void loop() {

  /* transmit_data[0] = analogRead(0);
    transmit_data[1] = analogRead(1);
    transmit_data[2] = !digitalRead(3);
    M11 0
    M21 1
    M12 2
    M22 3
  */

  int dataX = analogRead(axis_X);
  int dataY = analogRead(axis_Y);
  int  xSpeed = map(dataX, 0, 1024, -100, 100); //map val between 0 and 200
  int  ySpeed = map(dataY, 0, 1024, -100, 100);
  int xyABS = sqrt(ySpeed * ySpeed + xSpeed * xSpeed);
  int motorSpeed = map(xyABS, 0, 144, 0, 255);
  if (abs(xSpeed) < 5 && abs(ySpeed) < 5) {   // если мы в "мёртвой" зоне
    transmit_data[0] = 0;
    transmit_data[1] = 0;
    transmit_data[2] = 0;
    transmit_data[3] = 0;
  } else {
    if ((ySpeed > deadZone && xSpeed > deadZone)) //forward right
    {
      transmit_data[0] = 2;
      transmit_data[1] = motorSpeed - xSpeed;
      transmit_data[2] = 2;
      transmit_data[3] = motorSpeed;
    }
    else if ((ySpeed > deadZone && xSpeed < -deadZone)) //forward left
    {
      transmit_data[0] = 2;
      transmit_data[1] = motorSpeed;
      transmit_data[2] = 2;
      transmit_data[3] = motorSpeed - xSpeed;
    }
    else if ((ySpeed > deadZone && xSpeed > -deadZone && xSpeed < deadZone)) // forward
    {
      transmit_data[0] = 2;
      transmit_data[1] = motorSpeed;
      transmit_data[2] = 2;
      transmit_data[3] = motorSpeed;
    }
    else if ((abs(ySpeed) < deadZone && xSpeed < -deadZone)) // left
    {
      transmit_data[0] = 2;
      transmit_data[1] = motorSpeed;
      transmit_data[2] = 0;
      transmit_data[3] = 0;
    }
    else if ((abs(ySpeed) < deadZone && xSpeed > deadZone)) // right
    {
      transmit_data[0] = 0;
      transmit_data[1] = 0;
      transmit_data[2] = 2;
      transmit_data[3] = motorSpeed;
    }
    else if ((abs(xSpeed) < deadZone && ySpeed < -deadZone)) // backward
    {
      transmit_data[0] = 1;
      transmit_data[1] = motorSpeed;
      transmit_data[2] = 1;
      transmit_data[3] = motorSpeed;
    }
    else if ((ySpeed < -deadZone && xSpeed > deadZone)) // backward right
    {
      transmit_data[0] = 1;
      transmit_data[1] = motorSpeed - xSpeed;
      transmit_data[2] = 1;
      transmit_data[3] = motorSpeed;
    }
    else if ((xSpeed < -deadZone && ySpeed < -deadZone)) // backward left
    {
      transmit_data[0] = 1;
      transmit_data[1] = motorSpeed;
      transmit_data[2] = 1;
      transmit_data[3] = motorSpeed - xSpeed;
    }
  }
  /*
        M11 0
    M21 1
    M12 2
    M22 3
  */
  

  transmit_data[4] = !digitalRead(3) ? 255 : 0; // КНОПКА УДАРА
  transmit_data[5] = !digitalRead(BUTTON_DOWN) ? 255 : 0; // КНОПКА ФОРСАЖА

  /*for (int i = 0; i < 6; i++) { // в цикле от 0 до числа каналов
    if (transmit_data[i] != latest_data[i]) { // если есть изменения в transmit_data
      flag = 1; // поднять флаг отправки по радио
      latest_data[i] = transmit_data[i]; // запомнить последнее изменение
    }
  }*/
  flag = 1;

  if (flag == 1) {
    radio.powerUp(); // включить передатчик
    radio.write(&transmit_data, sizeof(transmit_data)); // отправить по радио
    flag = 0; //опустить флаг
    radio.powerDown(); // выключить передатчик
   /* Serial.print(transmit_data[0]);
    Serial.print(' ');
    Serial.print(transmit_data[1]);
    Serial.print(' ');
    Serial.print(transmit_data[2]);
    Serial.print(' ');
    Serial.print(transmit_data[3]);
    Serial.print(' ');
    Serial.print(transmit_data[4]);
    Serial.print(' ');
    Serial.println(transmit_data[5]);*/
  }
}
