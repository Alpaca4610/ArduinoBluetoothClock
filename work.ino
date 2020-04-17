int xueronghua[] {
  330, 393, 589,
  525, 393, 350,
  330, 330, 330, 350, 393,
  441, 393,
  330, 393, 589,
  525, 393, 350,
  330, 393, 393, 441, 495,
  525, 525, 525, 589, 393, 393,
  495, 441, 393, 330, 393, 525,
  441, 525, 589, 525, 495,
  393, 330, 393, 589, 525, 393,
  350, 330, 393, 393, 441, 495,
  525, 525, 525
};
float xrhbeat[] {
  1 + 1, 1, 1 + 1 + 1,
  1 + 1, 1, 1 + 1 + 1,
  1 + 1, 1, 1, 1, 1,
  1 + 1 + 1, 1 + 1 + 1,
  1 + 1, 1, 1 + 1 + 1,
  1 + 1, 1, 1 + 1 + 1,
  1 + 1, 1, 1, 1, 1,
  1 + 1, 1, 1 + 1 + 1,
  1 + 1, 0.5, 0.5,
  1, 1, 1, 1 + 1, 1, 1 + 1 + 1,
  1 + 1, 1, 1 + 1, 1, 1 + 1 + 1,
  1 + 1 + 1, 1 + 1, 1, 1 + 1 + 1,
  1 + 1, 1, 1 + 1 + 1, 1 + 1, 1,
  1.3, 1.3, 1.3,
  1 + 1.5, 1 + 1 + 1
};


#include <SCoop.h>//引入头文件
#include <LiquidCrystal_I2C.h>
#include<Timeget.h>
LiquidCrystal_I2C lcd(0x20, 16, 2);

String comdata = "";
int cflag = 0;
int val;

int ALARM_PIN = 4;                                               //定义蜂鸣器的接口为4
String recv = "";
int LED = 13;

boolean light_flag;                                              //定义lcd屏亮暗标志，true表示亮屏，false表示暗屏
boolean buzzer_flag;


//定义全局时间变量--年 月 日 时 分 秒
int Y1, Y2, Y3, Y4;                                              //定义年份的四位，从高到低
int M1, M2;                                                      //定义月份的两位，从高到低
int D1, D2;                                                      //定义日期的两位，从高到低
int H1, H2;                                                      //定义小时的两位，从高到低
int MI1, MI2;                                                    //定义分钟的两位，从高到低
int S1, S2;                                                      //定义秒数的两位，从高到低
int Date;                                                        //定义星期


//定义全局闹铃时间变量--月 日 时 分
int Alarm_M1, Alarm_M2;                                                      //定义月份的两位，从高到低
int Alarm_D1, Alarm_D2;                                                      //定义日期的两位，从高到低
int Alarm_H1, Alarm_H2;                                                      //定义小时的两位，从高到低
int Alarm_MI1, Alarm_MI2;                                                    //定义分钟的两位，从高到低
int Alarm_S1, Alarm_S2;                                                      //定义秒数的两位，从高到低


//定义临时变量存储时间
int temp_Y1, temp_Y2, temp_Y3, temp_Y4;                                    //定义年份的四位，从高到低
int temp_M1, temp_M2;                                                      //定义月份的两位，从高到低
int temp_D1, temp_D2;                                                      //定义日期的两位，从高到低
int temp_H1, temp_H2;                                                      //定义小时的两位，从高到低
int temp_MI1, temp_MI2;                                                    //定义分钟的两位，从高到低
int temp_Date;                                                        //定义星期
int temp_S1, temp_S2;                                                 //定义秒数的两位，从高到低


//定义判断闰年的函数
#define ISYEARP(x) x/100!=0&&x%4==0||x/400==0?1:0
//定义每个月的天数
int dayOfMonth[13][2] = {
  0, 0,
  31, 31,
  28, 29,
  31, 31,
  30, 30,
  31, 31,
  30, 30,
  31, 31,
  31, 31,
  30, 30,
  31, 31,
  30, 30,
  31, 31,
};

int x, y;                                                       //定义修改液晶屏的时间数位的横纵坐标




//功能函数定义


void setAlarm();                                                //定义设置闹钟的函数
void tita();                                                    //定义时间流动函数
void date();                                                    //定义日期的流动函数
void calendarInit();                                            //定义万年历初始化函数
void calendarShow();                                            //定义万年历的显示函数
void setalarm();
void clearserial();
void judge();
void load(int, int, int, int *, float *);

defineTaskLoop(Task1)//多线程1
{
  tita();
  delay(1000);
}


//主线程
void setup() {
  mySCoop.start();
  x = 3;
  y = 0;
  light_flag = true;
  buzzer_flag = false;
  Serial.begin(9600);
  pinMode(ALARM_PIN, OUTPUT);
  lcd.init();
  lcd.backlight();
  unsigned long millis_ = millis(); // 返回Arduino系统从启动到现在的时间
  if (millis_ - lastMillis >= 1000)
    lastMillis = millis_;
  initTime();
  dida();
  calendarInit(); //初始化时间
  calendarShow();
}

void loop() {
  unsigned long millis_ = millis(); // 返回Arduino系统从启动到现在的时间
  val = analogRead(A0);
  if (val > 0) {
    digitalWrite(LED, HIGH);
    delay(10000);
  }
  while (Serial.available()) {
    comdata += char(Serial.read());
    delay(2);
  }
  if (comdata == "1\0") {
    Serial.print("Please enter the time");
    cflag = 1;
  }
  comdata = "";

  if (cflag == 1)
    setalarm();
  calendarShow();
  delay(1000);
  judge();
}

//万年历初始化函数，加串口通信读取电脑当前时间
void calendarInit()
{
  Y1 = TY[0], Y2 = TY[1], Y3 = TY[2], Y4 = TY[3];                                 //年份初始化
  M1 = TM[0], M2 =  TM[1];                                                 //月份初始化
  D1 = TD[0], D2 = TD[1];                                                 //日期初始化
  H1 = TH[0], H2 = TH[1];                                                 //小时初始化
  MI1 = TMin[0], MI2 = TMin[1];                                                 //分钟初始化
  S1 = TS[0], S2 = TS[1];                                                 //秒数初始化
  Date = weekDay;                                                       //星期初始化
}
//万年历显示函数
void calendarShow()
{
  lcd.setCursor(3, 0);
  lcd.print(Y1);
  lcd.print(Y2);
  lcd.print(Y3);
  lcd.print(Y4);
  lcd.print("/");
  lcd.print(M1);
  lcd.print(M2);
  lcd.print("/");
  lcd.print(D1);
  lcd.print(D2);
  //lcd.print("日");
  lcd.setCursor(1, 1);
  lcd.print(H1);
  lcd.print(H2);
  lcd.print(":");
  lcd.print(MI1);
  lcd.print(MI2);
  lcd.print(":");
  lcd.print(S1);
  lcd.print(S2);
  lcd.setCursor(10, 1);
  switch (Date + 1) {
    case 1: lcd.print("Mon"); break;
    case 2: lcd.print("Tue"); break;
    case 3: lcd.print("Wes"); break;
    case 4: lcd.print("Sat"); break;
    case 5: lcd.print("Fri"); break;
    case 6: lcd.print("Sat"); break;
    case 7: lcd.print("Sun"); break;
    default: lcd.print("Error!"); break;
  }
}

void setalarm() {
  recv = "";
  while (!Serial.available()) {
    tita();
    delay(1000);
  }
  while (Serial.available())
  {
    while (Serial.available()) {
      recv += char(Serial.read());
      delay(2);
    }
    Alarm_M1 = int(recv[0]) - 48, Alarm_M2 = int(recv[1]) - 48;
    Alarm_D1 = int(recv[3]) - 48, Alarm_D2 = int(recv[4]) - 48;
    Alarm_H1 = int(recv[6]) - 48, Alarm_H2 = int(recv[7]) - 48;
    Alarm_MI1 = int(recv[9]) - 48, Alarm_MI2 = int(recv[10]) - 48;
    Alarm_S1 = int(recv[12]) - 48, Alarm_S2 = int(recv[13]) - 48;
  }
  recv = "";
  Serial.print("闹钟设置成功！请输入备忘录：");
  while (!Serial.available()) {
    tita();
    delay(1000);
  }
  while (Serial.available())
  {
    while (Serial.available()) {
      recv += char(Serial.read());
      delay(2);
    }
  }
  cflag = 0;
}

//时间流动函数
void tita() {
  S2 = S2 + 1;
  if (S2 == 10)
  {
    S2 = 0;
    S1 = S1 + 1;
    if (S1 == 6)
    {
      S1 = 0;
      MI2 = MI2 + 1;
      if (MI2 == 10)
      {
        MI2 = 0;
        MI1 = MI1 + 1;
        if (MI1 == 6)
        {
          MI1 = 0;
          H2 = H2 + 1;
          if (H1 == 2 && H2 == 4)
          {
            H1 = 0;
            H2 = 0;
            date();
          }
          if (H2 == 10)
          {
            H2 = 0;
            H1 = H1 + 1;

          }
        }
      }
    }
  }
  if (Alarm_S1 == S1 && Alarm_S2 == S2 && Alarm_MI2 == MI2 && Alarm_MI1 == MI1 && Alarm_H2 == H2 && Alarm_H1 == H1 && Alarm_M1 == M1 && Alarm_M2 == M2 && Alarm_D1 == D1 && Alarm_D2 == D2) {
    //响闹铃
    Serial.print(Alarm_H1);
    Serial.print(Alarm_H2);
    Serial.print(":");
    Serial.print(Alarm_MI1);
    Serial.print(Alarm_MI2);
    Serial.print(":");
    Serial.print(Alarm_S1);
    Serial.print(Alarm_S2);
    buzzer_flag = true;  //启动蜂鸣器
  }
}


//日期流动函数
void date() {
  Date = (Date + 1) % 7;
  D2 = D2 + 1;
  if ( D2 + D1 * 10 > (dayOfMonth[M1 * 10 + M2][ISYEARP(Y1 * 1000 + Y2 * 100 + Y3 * 10 + Y4)]))
  {
    D1 = 0;
    D2 = 1;
    M2 = M2 + 1;
    if (M1 == 1 && M2 == 3)
    {
      M1 = 0;
      M2 = 1;
      Y4 = Y4 + 1;
      if (Y4 == 10)
      {
        Y4 = 0;
        Y3 = Y3 + 1;
        if (Y3 == 10)
        {
          Y3 = 0;
          Y2 = Y2 + 1;
          if (Y2 == 10)
          {
            Y2 = 0;
            Y1 = Y1 + 1;
            if (Y1 == 10)
            {
              Y1 = 0;
            }
          }
        }
      }
    }
    if (M2 == 10)
    {
      M2 = 0;
      M1 = M1 + 1;
    }
  }
  if (D2 == 10) {
    D2 = 0;
    D1 = D1 + 1;
  }
}

void clearserial() {
  while (Serial.read() >= 0);
}

void judge() {
  if (buzzer_flag == true) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(recv);
    //digitalWrite(4, OUTPUT);
    //digitalWrite(LED, HIGH);
    //print("闹钟响了！");
    //delay(5000);
    int len = sizeof(xueronghua) / sizeof(xueronghua[0]);
    load(len, 4, LED, xueronghua, xrhbeat);
    buzzer_flag = false;
    lcd.clear();
  }
  else {
    digitalWrite(LED, LOW);
    digitalWrite(4, LOW);
  }
}

void load(int len, int buzzpin, int ledpin, int *tune, float *beat)
{
  for (int x = 0; x < len; x++)
  {
    tone(4, tune[x]);
    digitalWrite(ledpin, HIGH);
    delay(500 * beat[x]);
    digitalWrite(ledpin, LOW);
    noTone(buzzpin);
    delay(100);
  }
}
