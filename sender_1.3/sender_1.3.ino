#include <SPI.h>
#include <RF24.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LCD.h>

#define MAX_LENGTH 34
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

RF24 radio(6, 7); // Hardware Konfiguration: RF24L01 Modul SPI mit Pins 6 und 7 verwenden.

byte addresses[][5] = {"ADDR1", "ADDR2"};
//const int max_length = 16;

const char alphabet[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '_', '#'};
const int alpha_len = sizeof(alphabet) / sizeof(alphabet[0]);
char temp_char;
char old_temp;

const int encoder[3] = {8, 9, 5};

int alpha_pos = 0;
String input = "";
int PinALast = LOW;
bool sendMsg;

unsigned long cooldown = 0;
bool cooldown_resetted = false;

void setup() {

  //lcd setup
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Input: # to send");

  //nRF24L01+ Modul setup
  radio.begin();
  radio.setAutoAck(true);
  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
  radio.stopListening();

  //encoder setup
  pinMode(encoder[0], INPUT);           //CLK
  pinMode(encoder[1], INPUT);           //DT
  pinMode(encoder[2], INPUT_PULLUP);    //SW

  if (Serial.available())
    Serial.begin(9600);
}

void loop() {
  if (cooldown < millis()) {
    if (not(cooldown_resetted)) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Input: # to send");
      cooldown_resetted = true;
    }

    //handling encoder, updating temp_char
    encoderHandling(&sendMsg);
  }

  //on message send:
  if (sendMsg) {

    //send message and print result
    sendMessage();
    sendMsg = false;
    temp_char = 'a';

    cooldown = millis() + 1000;
  }
}

bool encoderHandling(bool *sendMsg) {

  int PinA = digitalRead(encoder[0]);
  if ((PinALast == LOW) && (PinA == HIGH)) {
    if (digitalRead(encoder[1]) == LOW) {
      alpha_pos--;
    } else {
      alpha_pos++;
    }
  }
  PinALast = PinA;


  if (alpha_pos < 0) { //when scrolling left alpha_pos will be lesser than 0, to prevent error we add the length of the array
    alpha_pos += alpha_len * 2;
  }

  temp_char = alphabet[(alpha_pos / 2 % alpha_len)];

  if (old_temp != temp_char) {
    lcd.setCursor(0, 1);
    lcd.print(input + temp_char);
  }

  if (!digitalRead(encoder[2])) {
    if (temp_char == '_') {
      input += " ";
      *sendMsg = false;

    } else if (temp_char == '#') {
      *sendMsg = true;

    } else {
      input += temp_char;
      *sendMsg = false;
    }
  }
  while (!digitalRead(encoder[2]));
}

void sendMessage() {
  char Payload[34] = "";

  for (int i = 0; i < min(input.length(), MAX_LENGTH); i++) {
    Payload[i] = input[i];
  }

  radio.stopListening();
  bool success = radio.write(&Payload, sizeof(Payload));
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Sent. Response " + String(success));

  input = "";
}

int getEncoderState() {
  if (digitalRead(encoder[0]) and digitalRead(encoder[0]))
    return 0;
  else if (digitalRead(encoder[0]) and digitalRead(encoder[1]))
    return 1;
  else if (digitalRead(encoder[1]) and digitalRead(encoder[0]))
    return 2;
  else if (digitalRead(encoder[1]) and digitalRead(encoder[1]))
    return 3;
}
