#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>

long oldMillis = 0;
int scrollDownFlag = 0;
int scrollUpFlag = 0;

#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);

#define SS_PIN D4
#define RST_PIN D3
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 

int code[] = {158,166,224,76}; //This is the stored UID
int code1[] = {158,166,224,78}; //This is the stored UID
int codeRead = 0;
String uidString;
int scroll = 0;
void setup() {
  oldMillis = millis();
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)

  // Clear the buffer.
  display.clearDisplay();
  display.display();
  display.setTextColor(WHITE); // or BLACK);
  display.setTextSize(1);
  display.setCursor(10,0); 
  display.print("Personal Display");
  display.display();
  
}

void loop() {
  if(millis()-oldMillis >200){
    Serial.println("timer");
    oldMillis = millis();
    int value = analogRead(A0);
    Serial.println(value);

    if(value > 900 && !scrollDownFlag){
      scrollDownFlag = 1;
    }
       if(value < 100 && !scrollUpFlag){
      scrollUpFlag = 1;
    }
    //digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN)^1);
  }
  if(  rfid.PICC_IsNewCardPresent())
  {
      readRFID();
  }
  if(scrollDownFlag || scrollUpFlag){
    if(scrollDownFlag){
      scroll++;
    }
    else{
      scroll--;
    }  
    clearUID();
    printUID(scroll*10);
    scrollDownFlag = 0;
    scrollUpFlag = 0;
  }
    delay(100);
  
   
}

void readRFID()
{
  
  rfid.PICC_ReadCardSerial();
  Serial.print(F("\nPICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

    clearUID();
   
    Serial.println("Scanned PICC's UID:");
    printDec(rfid.uid.uidByte, rfid.uid.size);

    uidString = String(rfid.uid.uidByte[0])+" "+String(rfid.uid.uidByte[1])+" "+String(rfid.uid.uidByte[2])+ " "+String(rfid.uid.uidByte[3]);
    
    printUID(10);

    int i = 0;
    boolean match = true;
    while(i<rfid.uid.size)
    {
      if(!(rfid.uid.uidByte[i] == code[i]))
      {
           match = false;
      }
      i++;
    }

    if(match)
    {
      Serial.println("\nI know this card!");
      printUnlockMessage();
    }else
    {
      Serial.println("\nUnknown Card");
    }


    // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

  void clearUID()
  {
    
    display.clearDisplay();
    display.display();
  }

  void printUID(int line)
  {
    display.setTextColor(WHITE); // or BLACK);
    display.setTextSize(1);
    display.setCursor(0,line); 
    display.print("NASA: ");
    display.setCursor(30,line); 
    display.print(uidString);

    display.setCursor(0,line+10); 
    display.print("Top view of enclosed Bioculture System cassette. Image courtesy of Tissue Genesis, Inc ");
    display.display();
  }
  void printUnlockMessage()
  {
    display.display();
    display.setTextColor(BLACK); // or BLACK);
    display.setTextSize(1);
    display.setCursor(10,0);
    display.print("Personal Display");
    display.display();
    
    display.setTextColor(WHITE); // or BLACK);
    display.setTextSize(1);
    display.setCursor(10,0); 
    display.print("");
    display.display();
    
    delay(2000);
    
    display.setTextColor(BLACK); // or BLACK);
    display.setTextSize(1);
    display.setCursor(10,0); 
    display.print("");

    display.setTextColor(WHITE); // or BLACK);
    display.setTextSize(1);
    display.setCursor(10,0); 
    display.print("Personal Display");
    display.display();
  }




