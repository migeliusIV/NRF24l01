#include <SPI.h>  // Подключаем библиотеку для работы с SPI-интерфейсом
#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01

#define PIN_ENA 3 // Вывод управления скоростью вращения мотора №1
#define PIN_ENB 6 // Вывод управления скоростью вращения мотора №2
#define PIN_IN1 2 // Вывод управления направлением вращения мотора №1
#define PIN_IN2 4 // Вывод управления направлением вращения мотора №1
#define PIN_IN3 5 // Вывод управления направлением вращения мотора №2
#define PIN_IN4 7 // Вывод управления направлением вращения мотора №2

#define PIN_CE  9  // Номер пина Arduino, к которому подключен вывод CE радиомодуля
#define PIN_CSN 10 // Номер пина Arduino, к которому подключен вывод CSN радиомодуля

RF24 radio(PIN_CE, PIN_CSN); // Создаём объект radio с указанием выводов CE и CSN

int potValue[4]; // Создаём массив для приёма значений потенциометра
bool flag = true;
uint8_t power = 0; // Значение ШИМ (или скорости вращения)



void setup() {
  Serial.begin(9600);
  
  // Установка всех управляющих пинов в режим выхода
  pinMode(PIN_ENA, OUTPUT); //левое колесо на аналог
  pinMode(PIN_ENB, OUTPUT); //правое колесо на аналог
  pinMode(PIN_IN1, OUTPUT); //
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
  radio.setChannel(5); // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setDataRate (RF24_1MBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openReadingPipe (1, 0x7878787878LL); // Открываем трубу ID передатчика
  radio.startListening(); // Начинаем прослушивать открываемую трубу
}



void loop() {
  if(radio.available()){    // Если в буфер приёмника поступили данные
    radio.read(&potValue, sizeof(potValue));    // Читаем показания с пульта
    //изначально было два варианта. первый - прописать действия в зависимости от джойстика для каждого положения, но так моторы работали ассинхронно.
    // поэтому было решено прописать действия ситуационно:  4 случая, приведённые ниже
    if (potValue[2] == 0){
      flag = true;
    } else if (potValue[3] == 0) {
      flag = false;
    } else {
      flag = flag;
    }
    
    if (flag) {
      if (potValue[0] > 125 && potValue[1] > 125){
          analogWrite(PIN_ENA, map(potValue[1],126,255,0,255)); 
          analogWrite(PIN_ENB, map(potValue[0],126,255,0,255));
          
          // Задаём направление для 1-го мотора
          digitalWrite(PIN_IN1, HIGH);
          digitalWrite(PIN_IN2, LOW);
          // Задаём направление для 2-го мотора
          digitalWrite(PIN_IN3, HIGH);
          digitalWrite(PIN_IN4, LOW);
          
        } else if (potValue[1] < 125 && potValue[0] > 125) {
          analogWrite(PIN_ENA, map(potValue[1],0,125,255,0)); 
          analogWrite(PIN_ENB, map(potValue[0],126,255,0,255));
          
          // Задаём направление для 1-го мотора
          digitalWrite(PIN_IN1, LOW);
          digitalWrite(PIN_IN2, HIGH);
        
          // Задаём направление для 2-го мотора
          digitalWrite(PIN_IN3, HIGH);
          digitalWrite(PIN_IN4, LOW);
        } else if (potValue[1] > 125 && potValue[0] < 125) {
          analogWrite(PIN_ENA, map(potValue[1],126,255,0,255)); 
          analogWrite(PIN_ENB, map(potValue[0],0,125,255,0));
          
          // Задаём направление для 1-го мотора
          digitalWrite(PIN_IN1, HIGH);
          digitalWrite(PIN_IN2, LOW);
        
          // Задаём направление для 2-го мотора
          digitalWrite(PIN_IN3, LOW);
          digitalWrite(PIN_IN4, HIGH);
        } else if (potValue[0] < 125 && potValue[1] < 125) {
          analogWrite(PIN_ENA, map(potValue[1],0,125,255,0)); 
          analogWrite(PIN_ENB, map(potValue[0],0,125,255,0));
          
          // Задаём направление для 1-го мотора
          digitalWrite(PIN_IN1, LOW);
          digitalWrite(PIN_IN2, HIGH);
        
          // Задаём направление для 2-го мотора
          digitalWrite(PIN_IN3, LOW);
          digitalWrite(PIN_IN4, HIGH);
        }
    } else {
      if (potValue[0] > 125 && potValue[1] > 125){
          analogWrite(PIN_ENA, map(potValue[0],126,255,0,255)); 
          analogWrite(PIN_ENB, map(potValue[1],126,255,0,255));
          
          // Задаём направление для 1-го мотора
          digitalWrite(PIN_IN1, LOW);
          digitalWrite(PIN_IN2, HIGH);
        
          // Задаём направление для 2-го мотора
          digitalWrite(PIN_IN3, LOW);
          digitalWrite(PIN_IN4, HIGH);
        } else if (potValue[0] < 125 && potValue[1] > 125) {
          analogWrite(PIN_ENA, map(potValue[0],0,125,255,0)); 
          analogWrite(PIN_ENB, map(potValue[1],126,255,0,255));
          
          // Задаём направление для 1-го мотора
          digitalWrite(PIN_IN1, HIGH);
          digitalWrite(PIN_IN2, LOW);
        
          // Задаём направление для 2-го мотора
          digitalWrite(PIN_IN3, LOW);
          digitalWrite(PIN_IN4, HIGH);
        } else if (potValue[0] > 125 && potValue[1] < 125) {
          analogWrite(PIN_ENA, map(potValue[0],126,255,0,255)); 
          analogWrite(PIN_ENB, map(potValue[1],0,125,255,0));
          
          // Задаём направление для 1-го мотора
          digitalWrite(PIN_IN1, LOW);
          digitalWrite(PIN_IN2, HIGH);
        
          // Задаём направление для 2-го мотора
          digitalWrite(PIN_IN3, HIGH);
          digitalWrite(PIN_IN4, LOW);
        } else if (potValue[1] < 125 && potValue[0] < 125) {
          analogWrite(PIN_ENA, map(potValue[0],0,125,255,0)); 
          analogWrite(PIN_ENB, map(potValue[1],0,125,255,0));
            
          // Задаём направление для 1-го мотора
          digitalWrite(PIN_IN1, HIGH);
          digitalWrite(PIN_IN2, LOW);
          
          // Задаём направление для 2-го мотора
          digitalWrite(PIN_IN3, HIGH);
          digitalWrite(PIN_IN4, LOW);
        }
    }
}
}
