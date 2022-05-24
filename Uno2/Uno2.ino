#include <MFRC522.h>
#include <SPI.h>
#include <MsTimer2.h>
#include <Stepper.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>


#define trig A2 
#define echo A3     //음파센서
#define RST_PIN 9 
#define SS_PIN 10   //RFID
#define TX A4 
#define RX A5 // Serial 통신선

void ServoControlON();
void ServoControlOFF();
void StepControlON();
void StepControlOFF();
void Psensing();

const int stepsPerRevolution = 2048;
int securitystate = 0 ;   // 경비모드 활성화 여부
const int CLK = 4;                                  
const int DAT = 3;                                  
const int RST = 2;         // RCT 모듈

Servo myServo;
Stepper myStepper(stepsPerRevolution, 8,6, 7, 5);
MFRC522 mfrc(SS_PIN,RST_PIN);
SoftwareSerial soft_Serial(TX, RX);
ThreeWire myWire(3,4,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);


//===========
const int duration = 3000;
unsigned long pre_time = 0;
unsigned long cur_time = 0;

String servodata="OFF";
String stepdata="closed";
String securitydata="OFF";
//===========

void setup() {
  // put your setup code here, to run once:

  
  pinMode( A1 , INPUT); // 인체감지 센서
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT); // 음파 센서

    
  myServo.attach(A0);  
  myServo.write(90);  // 서보모터

  
  myStepper.setSpeed(14); // 스텝 모터 속도 설정(RPM) 

  SPI.begin();
  mfrc.PCD_Init();

  Serial.begin(9600);
  soft_Serial.begin(9600);
  soft_Serial.setTimeout(50);

  Rtc.Begin();

  MsTimer2::set(1000, Psensing); // 사람 감지를 위한 타이머 인터럽트       

}



void loop() {
    // put your main code here, to run repeatedly:
    //=================================================================
    //< 제어 데이터 받기 >

    myServo.detach();
    Serial.println("status#"+ servodata + "#" + stepdata + "#" + securitydata);
    soft_Serial.println("status#"+ servodata + "#" + stepdata + "#" + securitydata);
    delay(500);
    myServo.attach(A0);


    if (soft_Serial.available() > 0)
    {
        String my_String = soft_Serial.readStringUntil('\0');

        if (my_String == "LightON")
        {
            ServoControlON();
            servodata = "ON";
        }
        else if(my_String == "LightOFF")
        {
            ServoControlOFF();
            servodata = "OFF";
        }
        else if(my_String == "OpenCt")
        {
           StepControlON();
           stepdata = "Opened";
        }
        else if(my_String == "CloseCt")
        {
           StepControlOFF();
           stepdata = "closed";
        }
    }

    //=================================================================
    //< RFID >


  if(!mfrc.PICC_IsNewCardPresent()||!mfrc.PICC_ReadCardSerial())
  {
    return ;
  }
  else
  {
    cur_time = millis();

        if(cur_time - pre_time >= duration)
        {
            if( securitystate == 0)
            {              
                  if(mfrc.uid.uidByte[0] == 131 &&
                     mfrc.uid.uidByte[1] == 253 &&
                     mfrc.uid.uidByte[2] == 61 &&
                     mfrc.uid.uidByte[3] == 19)
                  {
                     Serial.println("감시 활성화");
                     MsTimer2::start();
                     securitystate = 1;
                     securitydata = "ON";
                  }
                  else
                  {
                     Serial.println("카드 재입력 요망");
                  }        
            }
            else if( securitystate == 1)
            {             
                  if(mfrc.uid.uidByte[0] == 131 &&
                     mfrc.uid.uidByte[1] == 253 &&
                     mfrc.uid.uidByte[2] == 61 &&
                     mfrc.uid.uidByte[3] == 19)
                  {
                     Serial.println("감시 비활성화");
                     MsTimer2::stop();
                     securitystate = 0;
                     securitydata = "OFF";
                  }
                  else
                  {
                     Serial.println("카드 재입력 요망");
                  }
            }
            pre_time = cur_time;
      }
  }


}

//===================================================================

void ServoControlON(){
  for(int i= 90 ; i > 60 ; i --) // 0~30도 까지 작동
  {
    myServo.write(i); 
    delay(10);
  }
  delay(500);
  for(int i= 60 ; i < 90 ; i ++) //
  {
    myServo.write(i);
    delay(10);
  }
}

void ServoControlOFF(){

  for(int i= 90 ; i < 120 ; i ++) // 0~30도 까지 작동
  {
    myServo.write(i); 
    delay(10);
  }
  delay(100);
  for(int i= 120 ; i > 90 ; i --) //
  {
    myServo.write(i);
    delay(10);
  }
}

//===================================================================

void StepControlON(){
  myStepper.step(stepsPerRevolution*1); //  시계 반대 방향으로 한바퀴 회전
}

void StepControlOFF(){
  myStepper.step(-stepsPerRevolution*1); //  시계 방향으로 한바퀴 회전
}

//===================================================================
//
void Psensing(){
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  int dis = pulseIn(echo, HIGH); //아두이노에서만 쓰는 함수 (에코가 High레벨이 발생한 시간을 측정)
  dis = dis / 29.4 / 2;
  int IRsensor = digitalRead(A1);
  //Serial.println(dis);
  delay(100);
  if(IRsensor == HIGH && dis < 200)
  {
    Serial.println("사람 있음");
    RtcDateTime now = Rtc.GetDateTime();
    printDateTime(now);
    soft_Serial.println();
    Serial.println();
    delay(500);
  }
  else
  {
    Serial.println("사람 없음");
    delay(500);
  }

}
//===================================================================
// 시간 출력
#define countof(a) (sizeof(a) / sizeof(a[0]))
void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u#%02u#%04u#%02u#%02u#%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print("Time#");
    Serial.print(datestring);
    soft_Serial.print("Time#");
    soft_Serial.print(datestring);
    delay(100);
}
//===================================================================
