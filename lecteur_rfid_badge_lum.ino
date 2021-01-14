#include <SPI.h>
#include <MFRC522.h>
 // bibliotheque adafruit neopixel
#include <Adafruit_NeoPixel.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9
// pin ou est branché la commande dataIn du ring
#define LED_PIN  6

// Nombre de led
#define LED_COUNT  12

// NeoPixel luminosite, 0 (min) to 255 (max)
#define BRIGHTNESS 20


#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif


LiquidCrystal_I2C lcd(0x20,16,2);  // set the LCD address to 0x20 for a 16 chars and 2 line display


int backlightState = LOW;
long previousMillis = 0;
long interval = 1000;

byte CodeVerif=0; 
byte Badge1[4]={0xC4, 0x71, 0x1A, 0xB8}; 
byte Badge2[4]={0xEC, 0x56, 0x38, 0x3C};


char message[19]="on m'appelle l'ovni";
char message2[18]="Le J c'est le S";
char message3[5]="JCVD";
char message4[18]="C'est pas des LOL";



MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;


// Declare votre neopixel Ring
Adafruit_NeoPixel ring(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Nombre de Pixel dans le Ring
// Argument 2 = Wemos Pin
// Argument 3 = Pixel type
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Led cablé selon GRB (most NeoPixel products)
//   NEO_GRBW    Led cablé selon GRB + W (most NeoPixel products) + Led blanche
//   NEO_RGB     Led cablé selon RGB (v1 FLORA pixels, not v2)
//   NEO_RGBW   led cablé selon  RGBW (NeoPixel RGBW products)

// Init array that will store new NUID 
byte nuidPICC[4];

byte GetAccesState(byte *CodeAcces,byte *NewCode) 
{
  byte StateAcces=0; 
  if ((CodeAcces[0]==NewCode[0])&&(CodeAcces[1]==NewCode[1])&&
  (CodeAcces[2]==NewCode[2])&& (CodeAcces[3]==NewCode[3])){
    return StateAcces=1; }
  else{
    return StateAcces=0; }
}



void setup() 
{ 
  // Init RS232
  Serial.begin(9600);

  // Init SPI bus
  SPI.begin(); 

  // Init MFRC522 
  rfid.PCD_Init(); 

  lcd.init();        // initialize the lcd
  lcd.backlight();
  lcd.begin(16, 2);

  lcd.home();


  
  ring.begin();           // INITIALIZE NeoPixel ring object (REQUIRED)
 ring.setBrightness(BRIGHTNESS); // Set BRIGHTNESS to about 1/5 (max = 255)
 
 /*
 ring.fill(ring.Color(0,0,255),1,3);   // allume la led 2,3 et 4  en bleu
 ring.setPixelColor(6, ring.Color(255,0,0));   // allume la led 7 en rouge
 ring.setPixelColor(7, ring.Color(255,255,255));   // allume la led 8 en blanc
 ring.setPixelColor(8, ring.Color(0,0,255));   // allume la led 9 en bleu
 ring.setPixelColor(11,ring.Color(0,255,0));   // allume la led 12 en vert
 */
 ring.fill(ring.Color(40,40,40),0,11);
 ring.show();            // pour afficher les modifications

   // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }


}
 
void loop() 
{
  byte sector         = 1;
  byte blockAddr      = 4;
  byte count = 0;
  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);
  byte trailerBlock   = 7;

  
  // Code de l'écran LCD
  lcd.clear();

  int len = strlen(message);
  int len2 = strlen(message2);
  int len3 = strlen(message3);
  int len4 = strlen(message4);


  
    // Initialisé la boucle si aucun badge n'est présent 
  if ( !rfid.PICC_IsNewCardPresent())
    return;
    
  // Vérifier la présence d'un nouveau badge 
  if ( !rfid.PICC_ReadCardSerial())
    return;

  // Afffichage 
  Serial.println(F("Un badge est détecté"));

  
  // Enregistrer l’ID du badge (4 octets) 
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }

  // Vérification du code 
    if (GetAccesState(Badge1,nuidPICC)==1){
    // Affichage e,zrg,kz,g,zk,gk,zek,gzk
    ring.fill(ring.Color(30,0,30),0,11);
    ring.show();       // pour afficher les modifications
    for (int i = 0; i < 16; i++) {
    lcd.print(message[i]);
      }
    lcd.setCursor(0, 1);
    for (int i = 16; i < len; i++) {
    lcd.print(message[i]);
  }
  delay(3000);
  lcd.clear();
  for (int i = 0; i < len2; i++) {
    lcd.print(message2[i]);
  }
    delay(3000);
    lcd.clear();
    }
    
  if (GetAccesState(Badge2,nuidPICC)  == 1)
  {
    for (int i = 0; i < len3; i++) {
    lcd.print(message3[i]);
  }
  delay(3000);
  lcd.clear();
  for (int i = 0; i < 16; i++) {
    lcd.print(message4[i]);
  }
    lcd.setCursor(0, 1);
    for (int i = 16; i < len4; i++) {
    lcd.print(message4[i]);
  }
    delay(3000);
    lcd.clear();

    Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) rfid.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(rfid.GetStatusCodeName(status));
    }
    byte count = 0;
    for (byte i = 0; i < 16; i++) {
        if (buffer[i] == 0x1){
            count++;
            ring.setPixelColor(i, 0, 30, 0);
            ring.show();            // pour afficher les modifications
            delay(1000);
         }
         else {
            count++;
            ring.setPixelColor(i, 0,0,0);
            ring.show();
            delay(1000);
         }
  }
  }
   else {
      Serial.println("Code érroné");
    }
  
  // Affichage de l'identifiant 
  Serial.println(" L'UID du tag est:");
  for (byte i = 0; i < 4; i++) 
  {
    Serial.print(nuidPICC[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  

  // Re-Init RFID
  rfid.PICC_HaltA(); // Halt PICC
  rfid.PCD_StopCrypto1(); // Stop encryption on PCD

 
}
