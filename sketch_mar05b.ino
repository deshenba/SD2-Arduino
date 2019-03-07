/*
  5 Mar 2019 1715
  Code does the following
  Takes in kWh from user
  lets user select produce or consume
  if consume lets user select who to consume from
  transmits both produce and consume information in 3 integer array out serial
  Takes in 6 char array from serial
  converts 6 char array into 3 integer array
  updates balance if B[0] is self and B[2] is zero
  consumes B[1] worth of kWh (red led flashes for B[1] seconds) if B[0] is self and
  B[2] is not zero

  Todo:
  Need more LCD display clean up.
  Need to test for all potential cases.
  Need to clean up code.
  Need to Git


  DNE
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

//for led and buttons
const int ledRed = 10; // red
const int ledBlu = 11; // blue
const int ledGre = 12; // green
const int ledYel = 13; // currently not used
const int cButton = 16; // orange
const int pButton = 17; // brown

//for numpad
const byte rows = 4;
const byte cols = 4;

bool dirtyScreen = false;
bool pB = false;

// builds the layout of the numpad
const char keys[rows][cols] = {
  {'1', '2', '3', 'A'}, // {'1', '2', '3'},
  {'4', '5', '6', 'B'}, // {'4', '5', '6'},
  {'7', '8', '9', 'C'}, // {'7', '8', '9'},
  {'*', '0', '#', 'D'}  // {'*', '0', '#}
}; // eliminate outer white for 4X3; inner black and outer white for 3X3

//sets the pinout for the numpad on Arduino
byte rowPins[rows] = {9, 8, 7, 6}; //9 is outer black, 6 is inner black
byte colPins[cols] = {5, 4, 3, 2}; //5 is inner white, 2 is outer white

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

// the variables
int var, Number, kWh, kWhBalance, numKey, seller = 0;
int self = 1;//ids the local device number
int mode; //for switch state consume vs produce
char key = '0'; //char variable to bring in keypresses.
int A[3] = {1, 0, 0};// transmit array
int B[3] = {0, 0, 0};// receive array
String str;

// for reading in serial
const byte numChars = 7;
char receivedChars[numChars];   // an array to store the received data
boolean newData = false;

// The setup
void setup()
{
  Serial.begin(9600);
  pinMode(ledRed, OUTPUT);
  pinMode(ledYel, OUTPUT);
  pinMode(ledGre, OUTPUT);
  pinMode(ledBlu, OUTPUT);
  pinMode(cButton, INPUT_PULLUP);
  pinMode(pButton, INPUT_PULLUP);

  lcd.begin(); //establishes the LCD grid 20X4
  lcd.clear();
  for (int i = 4; i >= 0; i--) { //startup flash
    digitalWrite(ledYel, HIGH);
    delay(50);
    digitalWrite(ledYel, LOW);
    delay(50);
  }

}//end void setup

void loop()
{
  A[1] = kWh;
  A[2] = seller;
  key = keypad.getKey();
  keyPad();

  /*  internal status check*/
  lcd.setCursor(9, 3);
  lcd.print (String("B:") + String(B[0]) + " " + String(B[1]) ); //(String("cB:") + String(cB) + String(" pB:") + String(pB) +
  lcd.setCursor(16, 3);
  lcd.print(String(B[2]));
  lcd.setCursor(0, 3);
  lcd.print (String("A:") + String(A[0]) + " " + String(A[1]) ); //(String("cB:") + String(cB) + String(" pB:") + String(pB) +
  lcd.setCursor(7, 3);
  lcd.print(String(A[2]));// + String("M:") + String(mode) + String(" c:") + String(digitalRead(cButton)) + String(" p:") + String(digitalRead(pButton)));

  if (digitalRead(cButton) == HIGH && digitalRead(pButton) == HIGH && (kWh > 0))
  { //ready to transmit
    mode = 3;
  }

  switch (mode) {
    case 1:  { // producing

        digitalWrite(ledGre, LOW);
        digitalWrite(ledRed, LOW);
        digitalWrite(ledBlu, HIGH);
        lcd.setCursor( 0, 0);
        lcd.print(String(" Producing: ") + String(kWh) + String(" kWh     "));
        lcd.setCursor( 0, 1);
        lcd.print(" transmitting       ");
        lcd.setCursor(0, 2);
        lcd.print("                    ");
        Serial.println(String('1') + " " + String(kWh) + " " + String('0'));
        delay(1250);
        // void intake();

        lcd.setCursor(0, 1);
        lcd.print("Transaction Complete");
        //        digitalWrite(ledGre, HIGH);
        //        digitalWrite(ledRed, LOW);
        //        digitalWrite(ledBlu, LOW);
        delay(2000);
        //        kWhBalance = kWh;
        kWh = 0;
        mode = 0;
        lcd.clear();
      }
      break;
    case 2:  { // consuming
        lcd.clear();
        digitalWrite(ledGre, LOW);
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledBlu, LOW);
        lcd.setCursor( 0, 0);
        lcd.print(String(" Consuming: ") + String(kWh) + String(" kWh     "));

        while (key == NO_KEY) {
          key = keypad.getKey();
          keyPad2();
          lcd.setCursor(1, 1);
          lcd.print("Select seller: ");
        }//end while

        Serial.println(String('1') + " " + String(kWh) + " " + String(seller));

        //        if (kWhBalance >= kWh) {
        //          kWhBalance = kWhBalance - kWh;
        //          lcd.setCursor(1, 1);
        //          lcd.print("                    ");//row clear 20 spaces
        //          lcd. setCursor(2, 1);
        //          lcd.print(String("From User ") + String(seller));  ;
        //          for (int i = kWh; i > 0; i--) {
        //
        //            digitalWrite(ledRed, HIGH);
        //            delay(250);
        //            digitalWrite(ledRed, LOW);
        //            delay(250);
        //            digitalWrite(ledRed, HIGH);
        //            delay(250);
        //            digitalWrite(ledRed, LOW);
        //            delay(250);
        //          }// end consuming kWh
        //        }// end if Ballance >= kWh
        //        else {
        //          lcd.setCursor(1, 1);
        //          lcd.print("                    ");//row clear 20 spaces
        //          lcd. setCursor(2, 1);
        //          lcd.print("kWh > balance  ");
        //          delay(2000);
        //        }//end else
        kWh = 0;
        mode = 0;
        seller = 0;
        Serial.println("Balance " + String(kWhBalance));
        lcd.clear();
      }// end case2
      break;
    case 3: { // Select produce or consume
        lcd.setCursor(0, 0);
        str = "  Balance: " + String(kWhBalance);
        lcd.print(str);
        //lcd.print(kWhBalance);
        lcd.setCursor(str.length() + 1, 0);
        lcd.print("kWh  ");
        lcd.setCursor(0, 1);
        lcd.print("Enter kWh: ");
        lcd.setCursor( 1, 2);
        lcd.print("produce or consume");
        digitalWrite(ledGre, HIGH);
        digitalWrite(ledRed, LOW);
        digitalWrite(ledBlu, LOW);
        if (digitalRead(cButton) == HIGH && digitalRead(pButton) == LOW && kWh > 0)
        { //consume
          mode = 1;
        }// end if
        else if (digitalRead(cButton) == LOW && digitalRead(pButton) == HIGH && kWh > 0)
        { //produce
          mode = 2;
        }//end elseif
        else if (digitalRead(cButton) == HIGH && digitalRead(pButton) == HIGH && kWh == 0) mode = 0;
      }//end case3
      break;
    case 0:  { // default
        recvWithEndMarker();
        showNewData();

        digitalWrite(ledGre, LOW);
        digitalWrite(ledRed, LOW);
        digitalWrite(ledBlu, LOW);
        lcd.setCursor(0, 0);
        if (dirtyScreen == true) {
          lcd.clear();
          dirtyScreen = false;
        }//end if
        str = "  Balance: " + String(kWhBalance);
        lcd.print(str);
        //lcd.print(kWhBalance);
        lcd.setCursor(str.length() + 1, 0);
        lcd.print("kWh  ");
        lcd.setCursor(0, 1);
        lcd.print("Enter kWh: ");
      }//end case0
      break;
  }// end switch(mode)
}// end void loop

void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  //char space = ' ';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (rc != endMarker ) {
      //    if (rc != space) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {//if numChars
        ndx = numChars - 1;
      }//end if numChars
      //     }//end if space
    }// end if endMarker
    else {
      receivedChars[ndx] = '\0'; // terminate the string
      var = ndx;
      ndx = 0;
      newData = true;
    }
  }
}

void showNewData() {
  if (newData == true) {
    Serial.print("This just in ... "); // outputs back to serial port
    Serial.println(receivedChars);
    for (int i = 0; i < var; i++) {
      B[0] = receivedChars[0] - '0';

      Serial.print("This just in ... ");
      Serial.println(receivedChars[i]);
    }; //end for
    B[0] = receivedChars[0] - '0';

    if (receivedChars[3] != ' ') {// if kWh incoming is 2 digit
      int x1 = receivedChars[2] - '0';
      int x2 = receivedChars[3] - '0';
      int x3 = receivedChars[5] - '0';
      B[1] = x1 * 10 + x2;
      B[2] = x3;
    }
    else { //if kWh incoming is 1 digit
      B[1] = receivedChars[2] - '0';
      B[2] = receivedChars[4] - '0';
    }
    if (B[0] == self && B[2] == 0) { // update balance
      kWhBalance = B[1];
      lcd.setCursor(0, 2);
      lcd.print("Balance Updated");
      digitalWrite(ledBlu, HIGH);
      delay(5000);
      lcd.clear();
      digitalWrite(ledBlu, LOW);

    }//end update balance
    else if (B[0] == self && B[2] != 0) {
      kWh = B[1];
      lcd.setCursor(1, 1);
      lcd.print("                    ");//row clear 20 spaces
      lcd. setCursor(2, 1);
      lcd.print(String("From User ") + String(seller));  ;
      for (int i = kWh; i > 0; i--) {

        digitalWrite(ledRed, HIGH);
        delay(250);
        digitalWrite(ledRed, LOW);
        delay(250);
        digitalWrite(ledRed, HIGH);
        delay(250);
        digitalWrite(ledRed, LOW);
        delay(250);
      }// end consuming kWh
    }

    else {
      B[0] = 0;
      B[1] = 0;
      B[2] = 0;
    }
    kWh = 0;
    newData = false;
  }
}//end void showNewData

void keyPad() {
  int spa = 11;
  if (key == '1')  {
    int i = 1;
    if (kWh == 0)
      kWh = i;
    else if (kWh < 10)
      kWh = (kWh * 10 + i);
    lcd.setCursor(spa, 1);
    lcd.print(kWh);
    lcd.setCursor(spa + 2, 1);
    lcd.print(" kWh");
  }//end kWh 1 key

  if (key == '2')  {
    int i = 2;
    if (kWh == 0)
      kWh = i;
    else if (kWh < 10)
      kWh = (kWh * 10 + i);
    lcd.setCursor(spa, 1);
    lcd.print(kWh);
    lcd.setCursor(spa + 2, 1);
    lcd.print(" kWh");
  }//end kWh 2 key

  if (key == '3')  {
    int i = 3;
    if (kWh == 0)
      kWh = i;
    else if (kWh < 10)
      kWh = (kWh * 10 + i);
    lcd.setCursor(spa, 1);
    lcd.print(kWh);
    lcd.setCursor(spa + 2, 1);
    lcd.print(" kWh");
  }//end kWh 3 key

  if (key == '4')  {
    int i = 4;
    if (kWh == 0)
      kWh = i;
    else if (kWh < 10)
      kWh = (kWh * 10 + i);
    lcd.setCursor(spa, 1);
    lcd.print(kWh);
    lcd.setCursor(spa + 2, 1);
    lcd.print(" kWh");
  }//end kWh 4 key

  if (key == '5')  {
    int i = 5;
    if (kWh == 0)
      kWh = i;
    else if (kWh < 10)
      kWh = (kWh * 10 + i);
    lcd.setCursor(spa, 1);
    lcd.print(kWh);
    lcd.setCursor(spa + 2, 1);
    lcd.print(" kWh");
  }//end kWh 5 key

  if (key == '6')  {
    int i = 6;
    if (kWh == 0)
      kWh = i;
    else if (kWh < 10)
      kWh = (kWh * 10 + i);
    lcd.setCursor(spa, 1);
    lcd.print(kWh);
    lcd.setCursor(spa + 2, 1);
    lcd.print(" kWh");
  }//end kWh 6 key

  if (key == '7')  {
    int i = 7;
    if (kWh == 0)
      kWh = i;
    else if (kWh < 10)
      kWh = (kWh * 10 + i);
    lcd.setCursor(spa, 1);
    lcd.print(kWh);
    lcd.setCursor(spa + 2, 1);
    lcd.print(" kWh");
  }//end kWh 7 key

  if (key == '8')  {
    int i = 8;
    if (kWh == 0)
      kWh = i;
    else if (kWh < 10)
      kWh = (kWh * 10 + i);
    lcd.setCursor(spa, 1);
    lcd.print(kWh);
    lcd.setCursor(spa + 2, 1);
    lcd.print(" kWh");
  }//end kWh 8 key

  if (key == '9')  {
    int i = 9;
    if (kWh == 0)
      kWh = i;
    else if (kWh < 10)
      kWh = (kWh * 10 + i);
    lcd.setCursor(spa, 1);
    lcd.print(kWh);
    lcd.setCursor(spa + 2, 1);
    lcd.print(" kWh");
  }//end kWh 9 key

  if (key == '0')  {
    int i = 0;
    if (kWh == 0)
      kWh = i;
    else if (kWh < 10)
      kWh = (kWh * 10 + i);
    lcd.setCursor(spa, 1);
    lcd.print(kWh);
    lcd.setCursor(spa + 2, 1);
    lcd.print(" kWh");
  }//end kWh 0 key

  if (key == 'D')  {
    kWh = 0;
    dirtyScreen = true;
  }//end key D;
}//end keyPad

void keyPad2() {
  int spa = 11;
  //  if (key == '1')  {
  //    seller = 1;
  //    lcd.setCursor(spa, 1);
  //    lcd.print(String("seller " + String(1));
  //  }//end kWh 1 key

  if (key == '2')  {
    seller = 2;
    lcd.setCursor(spa, 1);
    lcd.print(String("seller " + String(1)));
  }//end kWh 2 key

  if (key == '3')  {
    seller = 3;
    lcd.setCursor(spa, 1);
    lcd.print(String("seller " + String(1)));
  }//end kWh 3 key

  if (key == '4')  {
    seller = 4;
    lcd.setCursor(spa, 1);
    lcd.print(String("seller " + String(1)));
  }//end kWh 4 key

  if (key == '5')  {
    seller = 5;
    lcd.setCursor(spa, 1);
    lcd.print(String("seller " + String(1)));
  }//end kWh 5 key

  if (key == '6')  {
    seller = 6;
    lcd.setCursor(spa, 1);
    lcd.print(String("seller " + String(1)));
  }//end kWh 6 key

}//end keyPad

