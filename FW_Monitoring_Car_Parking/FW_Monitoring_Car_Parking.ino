#include <avr/wdt.h>
#include <iNut.h>

iNut sensor;
//#define DEBUG
#define TimeCouter 2000
#define LED_GREEN 2
#define LED_RED 5

#define resetFunc()        \
  do                          \
  {                           \
    wdt_enable(WDTO_15MS);  \
    for(;;)                 \
    {                       \
    }                       \
  } while(0)

void setup() {
  wdt_disable();
  wdt_enable(WDTO_8S);
  // put your setup code here, to run once:
  Serial.begin (9600);
  pinMode (LED_RED, OUTPUT);
  pinMode (LED_GREEN, OUTPUT);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);

  sensor.setup(2);

#ifdef DEBUG
  Serial.println("Da san sang - state_sensor_infrared_1");
  Serial.println("Da san sang -state_sensor_infrared_2 ");
  Serial.println("==================================== ");
#endif
  wdt_reset();
  wdt_enable(WDTO_4S);
}

void loop() {
  int state_sensor_infrared = digitalRead(4);
  int state_sensor_infrared_2 = digitalRead(7);
  static unsigned long timer = 0;
  static unsigned long timerWatchDog = 0UL;
  static unsigned long RealTime = millis();

#define State_Sensor_Unsignal            0
#define State_Sensor_Receive_Signal      1
#define State_Sensor_Done_Receive_Singal 2
  //define all state of sensor
  static int State_Sensor = State_Sensor_Unsignal;

  switch (State_Sensor) {
    case State_Sensor_Unsignal: {
#ifdef DEBUG
        Serial.println(F("Không Làm Gì"));
#endif
        //do nothing;
        State_Sensor = State_Sensor_Receive_Signal;
        break;
      }
    case State_Sensor_Receive_Signal: {
        if (state_sensor_infrared == 1 || state_sensor_infrared_2 == 1) {
          digitalWrite(LED_GREEN, HIGH);
          digitalWrite(LED_RED, LOW);
#ifdef DEBUG
          Serial.println("DATA - Không Có Xe Trong Bãi");
          Serial.print("Data Sensor: ");
          Serial.println(state_sensor_infrared);
          Serial.println();
#endif
        }
        sensor.setValue(0, state_sensor_infrared);
        sensor.setValue(1, state_sensor_infrared_2);
        State_Sensor = State_Sensor_Done_Receive_Singal;
        break;
      }
    case State_Sensor_Done_Receive_Singal: {
        if (state_sensor_infrared == 0 || state_sensor_infrared_2 == 0) {
          digitalWrite(LED_GREEN, LOW);
          digitalWrite(LED_RED, HIGH);
#ifdef DEBUG
          Serial.println("DATA - Có Xe Trong Bãi");
          Serial.print("Data Sensor: ");
          Serial.println(state_sensor_infrared);
          Serial.println();
#endif
        }
        sensor.setValue(0, state_sensor_infrared);
        sensor.setValue(1, state_sensor_infrared_2);
        State_Sensor = State_Sensor_Unsignal;
        break;
      }
  }



  if (RealTime > TimeCouter) {
#ifdef DEBUG
    Serial.print("Dữ Liệu Cảm Biến Thứ Nhất: ");
    Serial.println(state_sensor_infrared);
    RealTime = millis();
#endif

  }
  if (millis() - timerWatchDog > 2000UL) {
    wdt_reset();
    timerWatchDog = millis();

#ifdef DEBUG
    Serial.println('.');
#endif
  }

  sensor.loop();
  getMemoryFree();
  wdt_reset();
  if (millis() - timer > 1000UL) {
    timer = millis();
    //    Serial.print ("Dữ Liệu Số Của Cảm Biến:  ");
    //    Serial.println(state_sensor_infrared);
    if (state_sensor_infrared == 1 && state_sensor_infrared_2 == 1 ) {
      Serial.println("Đéo có cái đen xe nào hết địt mẹ");
      Serial.println();
    }
    else if (state_sensor_infrared == 0) {
      Serial.println("Có cái xe đen đậu ngu vãi lồn");
      Serial.println();
    }
    else {
      Serial.println("Có cái xe vàng đậu ngu vãi lồn");
      Serial.println();
    }
  }
  //= 0 => có vật cản, bằng 1 => 0 có vật cản

}

int getMemoryFree() {
  extern int _heap_start;
  extern int *_brkval;
  //get free memory from function
  return (int) SP - (_brkval == 0 ? (int) &_heap_start : (int) _brkval);
}
