
#include <MsTimer2.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

#define TX 5
#define RX 4


LiquidCrystal_I2C lcd1(0x24, 16, 2);
LiquidCrystal_I2C lcd2(0x27, 16, 2);

SoftwareSerial soft_Serial(TX, RX);

volatile byte state_l = HIGH;
volatile byte state_c = HIGH;
const int duration = 1000;
unsigned long pre_time = 0;
unsigned long cur_time = 0;

char *DATA1[4]={NULL,};  // 상태값 수신 데이터
char *DATA2[7]={NULL,};  // 시간값 수신 데이터
char *ptr;

String str1;
String str2 ;
String str3 ;
String str4 ;
String Time;
String month;
String day;
String year;
String hour;
String minute;
String seconds;

//=================================================================================
// 전등 스위치 ON/OFF
//=================================================================================
void light()
{
  cur_time = millis();

  if (cur_time - pre_time >= duration)
  {
    if (state_l == HIGH)
    {
    soft_Serial.print("LightON");
    Serial.println("LightON");
    }
    else if (state_l == LOW)
    {
    soft_Serial.print("LightOFF");
    Serial.println("LightOFF");
    }
    
  pre_time = cur_time;
  state_l = !state_l;
  }  
}

//=================================================================================
// 블라인드 UP & DOWN
//=================================================================================
void curtain()
{
  cur_time = millis();

  if (cur_time - pre_time >= duration)
  {
    if (state_c == HIGH)
    {
    soft_Serial.print("OpenCt");
    Serial.println("OpenCt");
    }
    else if (state_c == LOW)
    {
    soft_Serial.print("CloseCt");
    Serial.println("CloseCt");
    }
    
  pre_time = cur_time;
  state_c = !state_c;
  }
}


//================================================================================


void setup() {
  // put your setup code here, to run once:

//=================================================================================
// LCD 화면 1 초기화면 출력 : 전등 및 커튼 상태
//=================================================================================
  lcd1.init();
  lcd1.backlight();
  lcd1.print("    WELCOME!   ");
  lcd1.setCursor(0, 1);
  lcd1.print("===SWEET HOME===");
  delay(500);
  lcd1.noBacklight();
  delay(50);
  lcd1.backlight();


  
//=================================================================================
// LCD 화면 2 초기화면 출력 : 경비 및 보안 장치
//=================================================================================

  lcd2.init();
  lcd2.backlight();
  lcd2.print("    WELCOME!   ");
  lcd2.setCursor(0, 1);
  lcd2.print("====SECURITY====");
  delay(500);
  lcd2.noBacklight();
  delay(50);
  lcd2.backlight();
  lcd2.clear();
  lcd2.print("Security:");
  lcd1.setCursor(0, 1);

//=================================================================================
// 전등, 커튼 스위치, 조도센서 휴대폰충전확인(인터럽트)
//=================================================================================

    // 전등 스위치
  attachInterrupt(0, light, FALLING);

  // 커튼
  attachInterrupt(1, curtain, FALLING);  

  // 충전 여부 확인 조도센서
  pinMode(A0, INPUT);
  
  
  Serial.begin(9600);
  soft_Serial.begin(9600);
  soft_Serial.setTimeout(20);
}

//=================================================================================

//=================================================================================

void loop() {
  // put your main code here, to run repeatedly:
//=====================================================
// 데이터 파싱
//=====================================================
 if(soft_Serial.available() > 0)
  {
          String status_now = soft_Serial.readString(); 
          
          int first_word = status_now.indexOf("#");
          String temp = status_now.substring(0,first_word);  // 첫번째 데이터 잘라서 임시 저장
          
          if( temp == "status")
          {
            int first = status_now.indexOf("#");
            int second = status_now.indexOf("#",first+1);
            int third = status_now.indexOf("#",second+1);
  
            int length = status_now.length();
  
            str1 = status_now.substring(0,first);
            str2 = status_now.substring(first+1,second);
            str3 = status_now.substring(second+1,third);
            str4 = status_now.substring(third+1,length);
  
          }
          else if(temp == "Time")
          {
            int first = status_now.indexOf("#");
            int second = status_now.indexOf("#",first+1);
            int third = status_now.indexOf("#",second+1);
            int fourth = status_now.indexOf("#",third+1);
            int fifth = status_now.indexOf("#",fourth+1);
            int sixth = status_now.indexOf("#",fifth+1);
  
            int length = status_now.length();
            
            Time = status_now.substring(0,first);
            month = status_now.substring(first+1,second);
            day = status_now.substring(second+1,third);
            year = status_now.substring(third+1,fourth);
            hour  = status_now.substring(fourth+1,fifth);
            minute = status_now.substring(fifth+1,sixth);
            seconds = status_now.substring(sixth+1,length);
           }
    }
  
//=============================================================================================================================
//    String status_now = soft_Serial.readString();    
//    delay(20);
//    int status_now_len = status_now.length() + 1; 
//    char str[status_now_len];                    
//    status_now.toCharArray(str, status_now_len);   // String으로 받은 데이터를 문자형 배열로 변환
//    ptr = strtok(str, "#");
//    
//    int i = 0;  
//   
//      if( *ptr == 's')
//      {
//
//          while( ptr1 != NULL )
//          {
//            //Serial.println(ptr);
//            DATA1[i] = ptr1;              // DATA1[1] : 전등 상태 / DATA1[2] : 커튼 상태 / DATA2[3] : 경비 상태
//            i++;
//            ptr1 = strtok(NULL, "#");
//            delay(10);
//          }
//          for(int i = 0 ; i < 4 ; i++)
//          {
//            Serial.println(DATA1[i]);
//            delay(10);
//          }                
//                        //파싱된 데이터 확인
//      }
//      
//      if( *ptr == 'T')
//      {
//
//          while( ptr != NULL )
//          {
//            DATA2[i] = ptr2;                 // DATA2[1] : MONTH / DATA2[2] : Day / DATA2[3] : Year
//            i++;                            // DATA2[4] : Hour  / DATA2[5] : Minute / DATA2[6] : second
//            ptr2 = strtok(NULL, "#");
//            delay(20);
//          }
//          
//          for(int i = 0 ; i < 7 ; i++)
//          {
//            Serial.println(DATA2[i]);
//            delay(10);
//          }                               //파싱된 데이터 확인
//       }
//=============================================================================================================================
//=====================================================
// LCD1 화면에 전등 상태 출력
//=====================================================
        lcd1.clear();
        lcd1.print("LIGHT:");
        lcd1.setCursor(6, 0);
        lcd1.print(str2);
        lcd1.setCursor(0, 1);
        lcd1.print("CURTAIN:");  
        lcd1.setCursor(8, 1);
        lcd1.print(str3);


//===========================================
//< 조도 센서 >
      int charger = analogRead(A0);
    
      //  충전 중일 때 표시
      if (charger < 80)
      {
        lcd1.setCursor(10, 0);
        lcd1.print("CHARGE");
      }
      // 충전 중이 아닐 때 표시
      else
      {
        lcd1.setCursor(10, 0);
        lcd1.print(" EMPTY");
      }

//======================================================
// LCD2 화면
//======================================================
        lcd2.clear();
        lcd2.print("Security:");
        lcd2.setCursor(11, 0);
        lcd2.print(str4);                 //security 상태
        lcd2.setCursor(0, 1);
        lcd2.print(year);
        lcd2.setCursor(4, 1);
        lcd2.print("-");
        lcd2.setCursor(5, 1);
        lcd2.print(month);
        lcd2.setCursor(7, 1);
        lcd2.print(day);
        lcd2.setCursor(9, 1);
        lcd2.print(" ");
        lcd2.setCursor(10, 1);
        lcd2.print(hour);
        lcd2.setCursor(12, 1);                
        lcd2.print(minute); 
        lcd2.setCursor(14, 1);                    
        lcd2.print(seconds);
    
}


  





  
