#include "EasyMFRC522.h"

EasyMFRC522 rfidReader(4, 3); // The MFRC522 reader, with the SDA and RST pins given

// Variable created to store the unique identifier of the RFID card
uint8_t cardUID[10]; // The array of identifiers (10 bytes)

String UID = "";
String goodID = "e3b42cd000000";
int gear = 0;
int before = 0;
bool locked = true;


// Define PIN names with PIN numbers
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
  Serial.setTimeout(20000); // Waits for up to 20 seconds for "read" functions
  
  while (!Serial)
    ;

  // The initialization function call is necessary!
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

    case 6:            /// only "."
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

    default:        ///   only "-"
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
  //Serial.println(gear);
  if(digitalRead(downshift) == HIGH && gear == 0){
    gear = 1;
  }
  else if (digitalRead(upshift) == HIGH && gear == 0){
    gear = 2;
  }
  else if(digitalRead(downshift) == HIGH && gear > 2){gear--;}
  else if(digitalRead(upshift) == HIGH && gear < 5){gear++;}
  //else {gear = 6;}
  Write(gear);
}

void RFID_Check() {
  Serial.println();
  Serial.println("Identify the Mifare card. Please wait...");

  bool success;
  do {
    // True if a Mifare card is detected
    success = rfidReader.detectTag();
    delay(50); // Wait for 0.05 seconds
  } while (!success);

  Serial.println("--> CARD DETECTED!\n");
  int result;

  {
    // Reading and storing the unique identifier
    MFRC522* device = rfidReader.getMFRC522();
    memcpy(cardUID, device->uid.uidByte, 10); // Copy the identifier to the cardUID array
    // Print the read identifier to the serial monitor
    Serial.println("Read identifier:");
    for (byte i = 0; i < 10; i++) {
      Serial.print(cardUID[i], HEX);
      UID+=String(cardUID[i], HEX);
      Serial.print(" ");
    }
    Serial.println(); // New line
    Serial.println();

    if (UID == goodID){
      Serial.println("OK");
      Serial.println(locked);
      locked = !locked;
    }
    else{
      Serial.println("NOT OK");
      Lock();
      }
    UID="";
    delay(500);
  }

  while (Serial.available() > 0) {  // Clear "garbage" input from serial
    Serial.read();
  }

  // Call this function after performing all desired operations on the tag
  rfidReader.unselectMifareTag();
  
  Serial.println();
  Serial.println("Operation finished!");
  Serial.println();
  delay(3000);
}

void Lock(){
  digitalWrite(relayPIN, LOW);
  locked=true;
  //Buzzer low double beep
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
  digitalWrite(relayPIN, HIGH);
  locked=false;
  //buzzer one higher beep
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
    Write(6);
    Locked();
    delay(250);
    Write(7);
    delay(500);
  }
  else {
  Unlocked();
  }

}
