
#include <Wire.h>                  // zahrnutie kniznic do programu 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
byte tagok = 0;

unsigned long sernum[11] = {0, 2880452397, 2883834667, 1122397444, 3278950471, 0, 0, 0, 0, 0, 1149839889}; //poste 0 N° carte lue, poste 1 à 9 pour UID carte a tester, poste 10 carte master

MFRC522 mfrc522(SS_PIN, RST_PIN);        // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

int led1 = 2;
int led2 = 3;
int led3 = 4;
int led4 = 5;

LiquidCrystal_I2C lcd(0x27, 16, 2);    // inicializacia kniznice (adresa, znaky, riadky)
unsigned long previousMillis1 = 0;
bool step1 = false;
bool step2 = false;
bool step3 = false;
bool step4 = false;
long onTime2 = 3000;
long onTime3 = 6000;
long onTime4 = 9000;
long offTime = 12000;

byte heart[8] = {

  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00100,
  0b00100,
  0b11111
};

byte smiley[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00100,
  0b00100,
  0b11111
};

byte frownie[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00100,
  0b00100,
  0b11111
};


void setup() {

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);

  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  Serial.println("SERIAL OK ");


  lcd.init();
  lcd.backlight();


  // create a new character
  lcd.createChar(0, heart);
  // create a new character
  lcd.createChar(1, smiley);
  // create a new character
  lcd.createChar(2, frownie);
  // create a new character


  int caractere = 0;

  lcd.setCursor(0, 0); // initialise le LCD 16 colonnes x 2 lignes
  lcd.write(1); // affiche le caractère
  lcd.print("   Barduino");
  lcd.setCursor(15, 0); // initialise le LCD 16 colonnes x 2 lignes
  lcd.write(1); // affiche le caractère
  lcd.setCursor(0, 1);
  lcd.print(" ");

}

void loop() {
  unsigned long currentMillis = millis();

  if (
    (step1 == false) &&
    (currentMillis - previousMillis1 < onTime2)
  ) {

    step1 = true;
    step2 = false;
    step3 = false;
    step4 = false;

    lcd.setCursor(0, 1);
    lcd.print("     V 1.0     ") ;
  } else if (
    (step2 == false) &&
    (currentMillis - previousMillis1 < onTime3) &&
    (currentMillis - previousMillis1 > onTime2)
  ) {

    step1 = false;
    step2 = true;
    step3 = false;
    step4 = false;

    lcd.setCursor(0, 1) ; // 10ème col - 2ème ligne - positionne le curseur à l'endroit voulu (colonne, ligne) (1ère=0 !)
    lcd.print("   Temp: 4\337C") ; // affiche la chaîne texte - message de test
  } else if (
    (step3 == false) &&
    (currentMillis - previousMillis1 < onTime4) &&
    (currentMillis - previousMillis1 > onTime3)
  ) {

    step1 = false;
    step2 = false;
    step3 = true;
    step4 = false;
    lcd.setCursor(0, 1) ; // 5ème col - 3ème ligne - positionne le curseur à l'endroit voulu (colonne, ligne) (1ère=0 !)
    lcd.print("    pump ok   ") ; // affiche la chaîne texte - message de test
  } else if (
    (step4 == false) &&
    (currentMillis - previousMillis1 < offTime) &&
    (currentMillis - previousMillis1 > onTime4))
  {
    previousMillis1 = currentMillis;
    step1 = false;
    step2 = false;
    step3 = false;
    step4 = true;

    lcd.setCursor(0, 1) ; // 13ème col - 4ème ligne - positionne le curseur à l'endroit voulu (colonne, ligne) (1ère=0 !)
    lcd.print("  System Ready   ") ; // affiche la chaîne texte - message de test
  } else if ((currentMillis - previousMillis1 > offTime)) {
    step1 = false;
    step2 = false;
    step3 = false;
    step4 = true;
    previousMillis1 = currentMillis;
  }

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())    return;

  Serial.print("Card UID:");    //Dump UID
  for (byte i = 0; i < 4; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }

  sernum[0] = *((unsigned long *)mfrc522.uid.uidByte); //recup num serie lu long unsigned

  Serial.print(" ");
  Serial.print(sernum[0]);
  Serial.print(" ");
  Serial.print(sernum[0], HEX);
  Serial.print(" ");
  tagok = 0;
  for  (byte i = 1; i < 10; i++) {
    if (sernum[0] == sernum[i]) tagok = 1; // carte reconnue dans la liste

  }
  if (sernum[0] == sernum[10]) tagok = 255; // carte master reconnue

  if (tagok == 0) { // faire si carte inconnue
    Serial.print("TAG INCONNU");
  }
  
  if (tagok > 0) //faire si carte reconnue
  {

    Serial.print("TAG OK ");
    if (sernum[0] == sernum[1]) {
      digitalWrite(led1, HIGH); 
      Serial.print("jus d'orange");
      delay(5000);
    }
    if (sernum[0] == sernum[2]) {
      digitalWrite(led2, HIGH);
      Serial.print("biere");
      delay(5000);
    }
    if (sernum[0] == sernum[3]) {
      digitalWrite(led3, HIGH), 
      Serial.print("vin");
      delay(5000);
    }
    if (sernum[0] == sernum[4]) {
      digitalWrite(led4, HIGH);
      Serial.print("eau");
      delay(5000);
    }
  }

  if (tagok == 255) //faire si carte master reconnue
  {

    Serial.print("CARTE MASTER RECONNUE");

  }

  mfrc522.PICC_HaltA(); // Halt PICC
}
