#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Servo.h>
#define in1 2 // 4
#define enA 3 // 5
#define in2 4 // 7
#define in3 5 // 6
#define enB 6 // 4
#define in4 7 // 5
#define m12_cstr 0.6 // множитель ПРАВОГО мотора
#define m34_cstr 1 // множитель ЛЕВОГО мотора

#define axis_X 0
#define axis_Y 1
#define timeout 200
#define MOTOR_MAX 255
#define JOY_MAX 1024

#define ChannelNUM 0x79


Servo myservo;
Servo myservo2;
RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно
//RF24 radio(9,53); // для Меги

byte recieved_data[6]; // массив принятых данных

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

int dutyR, dutyL;
bool r_tow, l_tow, is_ready = false;;
int pos = 0;    //
int pos2 = 0;
int kol = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(13, OUTPUT);
  myservo.attach(11);  //
  myservo2.attach(13);

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 25);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(6);     //размер пакета, в байтах

  radio.openReadingPipe(1, address[0]);     //хотим слушать трубу 0
  radio.setChannel(ChannelNUM);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.startListening();
  for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos2 = 0; pos2 <= 95; pos2 += 1) { //

    myservo2.write(pos2);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  delay(1000);


  for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  kol = 3;
  is_ready = true;
}

void command_get() {
  byte pipeNo;
  while ( radio.available(&pipeNo)) {  // слушаем эфир со всех труб
    radio.read( &recieved_data, sizeof(recieved_data) );
    Serial.print(recieved_data[0]);
    Serial.print(' ');
    Serial.print(recieved_data[1]);
    Serial.print(' ');
    Serial.print(recieved_data[2]);
    Serial.print(' ');
    Serial.print(recieved_data[3]);
    Serial.print(' ');
    Serial.print(recieved_data[4]);
    Serial.print(' ');
    Serial.println(recieved_data[5]);
  }
  if (recieved_data[5]) { // если включен форсаж
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    analogWrite(enA, int(255 * m12_cstr));
    analogWrite(enB, int(255 * m34_cstr));
  } else {
    digitalWrite(in1, (recieved_data[0] == 2 ? HIGH : LOW));
    digitalWrite(in2, (recieved_data[0] == 1 ? HIGH : LOW));
    digitalWrite(in3, (recieved_data[2] == 2 ? HIGH : LOW));
    digitalWrite(in4, (recieved_data[2] == 1 ? HIGH : LOW));
    analogWrite(enA, int(recieved_data[1] * m12_cstr));
    analogWrite(enB, int(recieved_data[3] * m34_cstr));
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  command_get();




  if (not is_ready) {
    switch (kol) {
      case 0:
        for (pos = 90; pos >= 0; pos -= 5) { // goes from 180 degrees to 0 degrees
          myservo.write(pos);
          command_get();// tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
        kol++;
        break;
      case 1:
        for (pos2 = 0; pos2 <= 95; pos2 += 5) { //

          myservo2.write(pos2);
          command_get();// tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
        kol++;
        break;
      case 2:
        for (pos = 0; pos <= 90; pos += 5) { // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          myservo.write(pos);
          command_get();// tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
        kol++;
        is_ready = true;
        break;
    }
  }
  if (recieved_data[4] and is_ready) {
    for (pos2 = 95; pos2 >= 0; pos2 -= 5) { // goes from 180 degrees to 0 degrees
      myservo2.write(pos2);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
    is_ready = false;
    kol = 0;
  }
  //digitalWrite(13, recieved_data[5] > 0 ? HIGH : LOW);



}
