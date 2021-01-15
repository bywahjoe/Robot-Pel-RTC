#include "ardupin.h"
#include <DS3231.h>

#define read1 digitalRead(A12)
#define read2 digitalRead(A11)
#define read3 digitalRead(A10)
#define read4 digitalRead(A9)
#define read5 digitalRead(A8)

DS3231  rtc(SDA, SCL);
bool isRepeat = false;

//Setting Robot Operation-----//
String jam[] = {
  "16:25",
  "16:27",
  "18:01",
  "19:00",

};
String nowTime;
String lastTime;
//----------------------------//

byte bitSensor;
//----------PID---------------//
int error = 0;
int lastError = 0;
byte kp = 22;
byte kd = 100;
byte SPEED = 125;
int MIN_SPEED = -90;
byte MAX_SPEED = 125;
//----------------------------//

void setup() {
  //Serial Communication
  Serial.begin(9600);
  Serial2.begin(9600);

  //Motor
  pinMode(kiri_REN, OUTPUT);
  pinMode(kiri_LEN, OUTPUT);
  pinMode(kiri_RPWM, OUTPUT);
  pinMode(kiri_LPWM, OUTPUT);

  pinMode(kanan_REN, OUTPUT);
  pinMode(kanan_LEN, OUTPUT);
  pinMode(kanan_RPWM, OUTPUT);
  pinMode(kanan_LPWM, OUTPUT);

  //POMPA
  pinMode(pompaIN1, OUTPUT);
  pinMode(pompaIN3, OUTPUT);

  //RELAY
  pinMode(pelKiri, OUTPUT);
  pinMode(pelKanan, OUTPUT); //HIGHKAN
  pinMode(debu, OUTPUT);

  rtc.begin();
  deviceOFF(); //Set Default OFF

  //  for (int i = 0; i < (sizeof(jam) / sizeof(jam[0])); i++) {
  //
  //    Serial.println(jam[i]);
  //  }

  // The following lines can be uncommented to set the date and time
  //  rtc.setDOW(FRIDAY);     // Set Day-of-Week to SUNDAY
  //  rtc.setTime(13, 9, 20);     // Set the time to 12:00:00 (24hr format)
  //  rtc.setDate(15, 1, 2021);   // Set the date to January 1st, 2014

  //  strategi();
}

void loop() {
  //followLine();

  String wktu = rtc.getTimeStr();
  bool cek = cari(wktu);

  if (cek) {
    nowTime = wktu[3] + wktu[4];

    if (!isRepeat) {
      strategi();
      isRepeat = true;
      lastTime = nowTime;
    }
    if (lastTime != nowTime) isRepeat = false;

  } else {
    isRepeat = false;
  }

  delay(50);

}
void strategi() {

  Serial2.println("n");
  deviceON();
  percabangan(1, 1000, 200);
  //  belok_kanan(150, 100);
  //  percabangan(1,1000,0);
  rem();
  deviceOFF();
  Serial2.println("f");

}

//return get search list waktu
boolean cari(String findJam) {
  for (int i = 0; i < (sizeof(jam) / sizeof(jam[0])); i++) {
    if (findJam.startsWith(jam[i])) return true;

    //Serial.println(jam[i]);
  }
  return false;
}

//------DEBUGGGG---------
void testDebug() {
  deviceON();
  setMotor(150, 150);
  delay(10000);
  deviceOFF();
  setMotor(-150, -150);
  delay(10000);
}
void testSerial() {
  Serial2.println("n");
  delay(10000);
  Serial2.println("f");
  delay(10000);
}
//-------------------------

void pompaAirON() {   //L298
  digitalWrite(pompaIN1, HIGH);
  digitalWrite(pompaIN3, HIGH);
}
void pompaAirOFF() {  //L298
  digitalWrite(pompaIN1, LOW);
  digitalWrite(pompaIN3, LOW);
}
void pelON() {
  //LOW Relay nyala
  digitalWrite(pelKiri, LOW);
  digitalWrite(pelKanan, LOW);
}
void pelOFF() {
  //LOW Relay nyala
  digitalWrite(pelKiri, HIGH);
  digitalWrite(pelKanan, HIGH);
}
void penghisapON() {
  digitalWrite(debu, LOW);
}
void penghisapOFF() {
  digitalWrite(debu, HIGH);
}
void deviceON() {
  penghisapON();
  pompaAirON();
  pelON();
}
void deviceOFF() {
  penghisapOFF();
  pompaAirOFF();
  pelOFF();

  //Motor rem
  rem();
}
void motorKiri(int myspeed) {
  int varA = 0, varB = 0;

  digitalWrite(kiri_REN, HIGH);
  digitalWrite(kiri_LEN, HIGH);

  if (myspeed == 0) {
    digitalWrite(kiri_REN, LOW);
    digitalWrite(kiri_LEN, LOW);
  }
  else if (myspeed < 0) {
    varB = abs(myspeed);
  }
  else {
    varA = myspeed;
  }
  analogWrite(kiri_RPWM, varA);
  analogWrite(kiri_LPWM, varB);

}
void motorKanan(int myspeed) {
  int varA = 0, varB = 0;

  digitalWrite(kanan_REN, HIGH);
  digitalWrite(kanan_LEN, HIGH);

  if (myspeed == 0) {
    digitalWrite(kanan_REN, LOW);
    digitalWrite(kanan_LEN, LOW);

  }
  else if (myspeed < 0) {
    varB = abs(myspeed);
  }
  else {
    varA = myspeed;
  }

  analogWrite(kanan_RPWM, varA);
  analogWrite(kanan_LPWM, varB);
}
void setMotor(int kiri, int kanan) {
  motorKiri(kiri);
  motorKanan(kanan);

}
void maju(int myspeed) {
  setMotor(myspeed, myspeed);
}
void stops() {
  setMotor(0, 0);
}
void rem() {
  int valRem = 50;
  digitalWrite(kiri_REN, LOW);
  digitalWrite(kiri_LEN, LOW);
  analogWrite(kiri_RPWM, valRem);
  analogWrite(kiri_LPWM, valRem);

  digitalWrite(kanan_REN, LOW);
  digitalWrite(kanan_LEN, LOW);
  analogWrite(kanan_RPWM, valRem);
  analogWrite(kanan_LPWM, valRem);
}


//LF PROGRAM-------

void remDelay(int waktu) {
  int valRem = 50;
  digitalWrite(kiri_REN, LOW);
  digitalWrite(kiri_LEN, LOW);
  analogWrite(kiri_RPWM, valRem);
  analogWrite(kiri_LPWM, valRem);

  digitalWrite(kanan_REN, LOW);
  digitalWrite(kanan_LEN, LOW);
  analogWrite(kanan_RPWM, valRem);
  analogWrite(kanan_LPWM, valRem);
  delay(waktu);
}
int readSensor() {
  //Serial.println(sensor1);
  bitSensor = ((!read1 * 1) + (!read2 * 2) + (!read3 * 4) + (!read4 * 8)
               + (!read5 * 16));
  return bitSensor;

}
void followLine() {
  int sensor = readSensor();
  //  Serial.println(sensor);
  switch (sensor) {
    case 1: error = -4;  break;
    case 3: error = -3;  break;
    case 2: error = -2;  break;
    case 6: error = -1;  break;
    case 4: error = 0;  break; //tengah
    case 12: error = 1;  break;
    case 8: error = 2;  break;
    case 24: error = 3;  break;
    case 16: error = 4;  break;
  }

  int rateError = error - lastError;
  lastError = error;

  int moveVal = (int)(error * kp) + (rateError * kd);

  int moveLeft = SPEED + moveVal;
  int moveRight = SPEED - moveVal;

  moveLeft = constrain(moveLeft, MIN_SPEED, MAX_SPEED);
  moveRight = constrain(moveRight, MIN_SPEED, MAX_SPEED);
  //    Serial.print(moveLeft);
  //    Serial.print(",");
  //    Serial.println(moveRight);
  setMotor(moveLeft, moveRight);
}

void percabangan(int xcount, int lama_henti, int delayMaju)
{
  int x = 0;
  while (x < xcount)
  {
    followLine();
    if (!read1 && !read2 && !read3 && !read4 && !read5)
    {
      x++;
    }
    while (!read1 && !read2 && !read3 && !read4 && !read5)
    {
      followLine();
      //    maju(80);

    }
  }
  
  //Maju Dikit-----
  if (delayMaju > 0) {
    maju(80);
    delay(delayMaju);
  };
  //---------------
  remDelay(lama_henti);
}
//--------------------------------------------------------------------//

//------------------------belok kiri-------------------------//
void belok_kiri(int kecbelok, int lama_henti) {

  while (!read2 || !read3 || !read4) {
    setMotor(-kecbelok, kecbelok);
  }

  while (read2 && read3 && read4) {
    setMotor(-kecbelok, kecbelok);
  }

  remDelay(lama_henti);
}

void belok_kanan(int kecbelok, int lama_henti) {

  while (!read2 || !read3 || !read4) {
    setMotor(kecbelok, -kecbelok);
  }

  while (read2 && read3 && read4) {
    setMotor(kecbelok, -kecbelok);
  }

  remDelay(lama_henti);
}
