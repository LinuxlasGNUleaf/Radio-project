const int max_length = 16;
#include <SPI.h>
#include <RF24.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LCD.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);


RF24 radio(6, 7); // Hardware Konfiguration: RF24L01 Modul SPI mit Pins 6 und 7 verwenden.
byte addresses[][5] = {"ADDR1", "ADDR2"};
char new_Payload[34];
char old_Payload[34];
const int load_len = 4;

byte customBackslash[8] = {
  0b00000,
  0b10000,
  0b01000,
  0b00100,
  0b00010,
  0b00001,
  0b00000,
  0b00000
};

const char loading[load_len] = {'/', '-', 'c', '|'};
int load_pos = 0;
bool received = false;
char point_char = '.';

void setup() {
  //lcd setup
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.createChar(0, customBackslash);
  lcd.setCursor(0, 0);
  lcd.print("scanning...");

  //nRF24L01+ Modul setup
  radio.begin();
  radio.setAutoAck(true);
  radio.setPALevel(RF24_PA_MAX);
  // open a reading and writing pipe on different adresses
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);
  
  // start listening
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    received = true;
    
    while (radio.available())
      // repeat loop while receiving data
      radio.read(&new_Payload, sizeof(new_Payload));

    //lcd update
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(old_Payload);
    lcd.setCursor(0, 1);
    lcd.print(new_Payload);

    for (int i = 0; i < max_length; i++) {
      if (old_Payload[i] != new_Payload[i]) 
        old_Payload[i] = new_Payload[i];    
    }
    
  } else if (!received) {
    lcd.setCursor(8, 0);
    int pos = 8;
    
    for (int i = 0; i < load_len; i++) {
      if (i < load_pos % load_len) {
        lcd.print(point_char);
      } else {
        lcd.print(" ");
      }
      pos++;
      lcd.setCursor(pos, 0);
    }

    lcd.setCursor(15, 0);
    char temp = loading[load_pos % load_len];
    if (temp == 'c')
      lcd.write(byte(0));
    else
      lcd.print(temp);
    load_pos++;
    delay(200);
  }
}
