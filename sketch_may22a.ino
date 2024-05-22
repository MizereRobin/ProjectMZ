#include "EasyMFRC522.h"

EasyMFRC522 rfidReader(4, 3); // MFRC522 读卡器，给出了 SDA 和 RST 引脚
// 创建变量用于存储 RFID 卡的唯一标识符
uint8_t cardUID[10]; // 标识符数组identifier9（10 字节）

String UID = "";
String goodID = "da32dd84000000";
int gear = 0;
int before = 0;
bool locked = true;


// 使用 PIN 码定义 PIN 名称
#define topleft 2
#define top 7
#define topright 6
#define middle 5
#define lowerleft A3
#define low A2
#define lowerright 8
#define dot 9
#define upshift A0
#define downshift A1
#define BuzzerPIN 10
#define relayPIN A4



void setup() {
  Serial.begin(9600);
  Serial.setTimeout(20000); // “读取”功能最多等待 20 秒

  
  while (!Serial)
    ;

  // 初始化函数的调用是必要的！
  rfidReader.init(); 

  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A4, OUTPUT);

  digitalWrite(dot, HIGH);
  tone(BuzzerPIN, 1000);
  delay(250);
  tone(BuzzerPIN, 200);
  delay(250);
  noTone(BuzzerPIN);
  digitalWrite(dot, LOW);
}


void Write(int character) {
    // 0 -> "0"
    // 1 -> "1"
    // 2 -> "2"
    // 3 -> "3"
    // 4 -> "4"
    // 5 -> "5"
    // 6 -> "."
    // 7 -> ""
    // other -> "-"

  switch (character) {
    case 0:
      digitalWrite(topleft, HIGH);
      digitalWrite(top, HIGH);
      digitalWrite(topright, HIGH);
      digitalWrite(middle, LOW);
      digitalWrite(lowerleft, HIGH);
      digitalWrite(low, HIGH);
      digitalWrite(lowerright, HIGH);
      digitalWrite(dot, LOW);
    break;

    case 1:
      digitalWrite(topleft, LOW);
      digitalWrite(top, LOW);
      digitalWrite(topright, HIGH);
      digitalWrite(middle, LOW);
      digitalWrite(lowerleft, LOW);
      digitalWrite(low, LOW);
      digitalWrite(lowerright, HIGH);
      digitalWrite(dot, LOW);
    break;
    
    case 2:
      digitalWrite(topleft, LOW);
      digitalWrite(top, HIGH);
      digitalWrite(topright, HIGH);
      digitalWrite(middle, HIGH);
      digitalWrite(lowerleft, HIGH);
      digitalWrite(low, HIGH);
      digitalWrite(lowerright, LOW);
      digitalWrite(dot, LOW);
    break;

    case 3:
      digitalWrite(topleft, LOW);
      digitalWrite(top, HIGH);
      digitalWrite(topright, HIGH);
      digitalWrite(middle, HIGH);
      digitalWrite(lowerleft, LOW);
      digitalWrite(low, HIGH);
      digitalWrite(lowerright, HIGH);
      digitalWrite(dot, LOW);
    break;
  
    case 4:
      digitalWrite(topleft, HIGH);
      digitalWrite(top, LOW);
      digitalWrite(topright, HIGH);
      digitalWrite(middle, HIGH);
      digitalWrite(lowerleft, LOW);
      digitalWrite(low, LOW);
      digitalWrite(lowerright, HIGH);
      digitalWrite(dot, LOW);
    break;
  
    case 5:
      digitalWrite(topleft, HIGH);
      digitalWrite(top, HIGH);
      digitalWrite(topright, LOW);
      digitalWrite(middle, HIGH);
      digitalWrite(lowerleft, LOW);
      digitalWrite(low, HIGH);
      digitalWrite(lowerright, HIGH);
      digitalWrite(dot, LOW);
    break;

    case 6:            /// 只有 "."
      digitalWrite(topleft, LOW);
      digitalWrite(top, LOW);
      digitalWrite(topright, LOW);
      digitalWrite(middle, LOW);
      digitalWrite(lowerleft, LOW);
      digitalWrite(low, LOW);
      digitalWrite(lowerright, LOW);
      digitalWrite(dot, HIGH);
    break;
    case 7:            /// nothing
      digitalWrite(topleft, LOW);
      digitalWrite(top, LOW);
      digitalWrite(topright, LOW);
      digitalWrite(middle, LOW);
      digitalWrite(lowerleft, LOW);
      digitalWrite(low, LOW);
      digitalWrite(lowerright, LOW);
      digitalWrite(dot, LOW);
    break;

    default:        ///   只有 "-"
      digitalWrite(topleft, LOW);
      digitalWrite(top, LOW);
      digitalWrite(topright, LOW);
      digitalWrite(middle, HIGH);
      digitalWrite(lowerleft, LOW);
      digitalWrite(low, LOW);
      digitalWrite(lowerright, LOW);
      digitalWrite(dot, LOW);
    break;
  
  }
}

void shifter() {
  before=gear;
  delay(750);
  //Serial.println(齿轮);
  if(digitalRead(downshift) == HIGH && gear == 0){
    gear = 1;
  }
  else if (digitalRead(upshift) == HIGH && gear == 0){
    gear = 2;
  }
  else if(digitalRead(downshift) == HIGH && gear == 2){
    gear = 0;
  }
  else if (digitalRead(upshift) == HIGH && gear == 1){
    gear = 0;
  }
  else if(digitalRead(downshift) == HIGH && gear > 2){gear--;}
  else if(digitalRead(upshift) == HIGH && gear < 5){gear++;}
  //else {gear = 6;}就是0档
  Write(gear);
  Serial.println(gear);
}

void RFID_Check() {
  Serial.println();
  Serial.println("Identify the Mifare card. Please wait...");

  bool success;
  float timer = 0;
  do {
    // 如果检测到 Mifare 卡则为 True
    success = rfidReader.detectTag();
    delay(50); // 等0.05秒
    timer+=0.5;
    if(!locked && timer>=10){break;}
  } while (!success);
if(success){
  Serial.println("--> CARD DETECTED!\n");
  int result;

  {
    // 读取并存储唯一标识符identifier
    MFRC522* device = rfidReader.getMFRC522();
    memcpy(cardUID, device->uid.uidByte, 10); // 将标识符复制到cardUID数组
    // 将读取标识符打印到串行监视器
    Serial.println("Read identifier:");
    for (byte i = 0; i < 10; i++) {
      Serial.print(cardUID[i], HEX);
      UID+=String(cardUID[i], HEX);
      Serial.print(" ");
    }
    Serial.println(); // 新的一行
    Serial.println();

    if (UID == goodID){
      UID="";
      Serial.println("OK");
      Serial.println(locked);
      UID="";
      if(locked) {Unlock();}
      else{Lock();}
      UID="";
    }
    else if(UID != ""){
      Serial.println("NOT OK");
      Lock();
      }
    UID="";
    delay(500);
  }
  UID="";
  while (Serial.available() > 0) {  // 清除串行输入的“垃圾”
    Serial.read();
  }

  // 对标签执行所有所需操作后调用此函数
  rfidReader.unselectMifareTag();
  
  Serial.println();
  Serial.println("Operation finished!");
  Serial.println();
  UID="";
  delay(3000);
}
}

void Lock(){
  locked=true;
  //响两下buzzer就是low
  tone(BuzzerPIN, 200);
  delay(250);
  noTone(BuzzerPIN);
  delay(100);
  tone(BuzzerPIN, 200);
  delay(250);
  noTone(BuzzerPIN);
  Write(7);
}

void Locked(){
  RFID_Check();
}

void Unlock(){
  locked=false;
  //响high一声buzzer就是high
  tone(BuzzerPIN, 1000);
  delay(250);
  noTone(BuzzerPIN);
}

void Unlocked(){
  shifter();
  RFID_Check();
  
}


void loop(){
  if(locked){
    digitalWrite(relayPIN, LOW);
    Write(6);
    Locked();
    delay(250);
    Write(7);
    delay(500);
  }
  else {
  digitalWrite(relayPIN, HIGH);
  Unlocked();
  }
}
