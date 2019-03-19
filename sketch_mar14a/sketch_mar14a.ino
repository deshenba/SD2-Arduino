/*
  9 Mar 2019 2359
  Added push button to increase production. Addes production button. Removed keypad for production.  Sell button now does only the transmit of transaction.

Need to clean up the code again.


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
const int pButton = 15; // gray wire Yellow/Blue button
const int cButton = 16; // orange wire Red Button
const int sButton = 17; // brown wire Green button

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
int var, kWh, consumekWh, kWhBalance, Balance, numKey, seller = 0;
int self = 1;//ids the local device number
int mode = 0; //for switch state consume vs produce
char key = '0'; //char variable to bring in keypresses.
int A[3] = {1, 0, 0};// transmit array
int B[3] = {0, 0, 0};// receive array
int pButtonState, cButtonState;
int pButtonLastState, cButtonLastState = 1;
String str;

// for reading in serial
const byte numChars = 7; //max 7 bit array incoming
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
  pinMode(sButton, INPUT_PULLUP);
  pinMode(pButton, INPUT_PULLUP);

  lcd.begin(); //establishes the LCD grid 20X4
  lcd.clear();
  for (int i = 4; i >= 0; i--) { //startup flash
    digitalWrite(ledYel, HIGH);
    delay(50);
    digitalWrite(ledYel, LOW);
    delay(50);
  }//end startup flash
  delay (2000);

}//end void setup

void loop()
{
  //A[1] = 0;


  key = keypad.getKey();
  keyPad();

  /* Input of kWh by button press */

  pButtonState = digitalRead(pButton);
  if (pButtonState != pButtonLastState) {
    // if the state has changed, increment the counter
    if (pButtonState == LOW) {
      // if the current state is HIGH then the button went from off to on:
      if (kWh ==0) dirtyScreen=true;
      kWh++;

      // Delay a little bit to avoid bouncing
      delay(25);
    } //end count if button is pressed
    // save the current state as the last state, for next time through the loop
    pButtonLastState = pButtonState;
  }//end if checking that current state = laststate

  /*  Code to display system values for code checking  */

  // lcd.setCursor(0, 2);
  // lcd.print (String("B:") + String(B[0]) + " " + String(B[1]) );
  // lcd.setCursor(7, 2);
  // lcd.print(String(B[2]));
  //  lcd.setCursor(0, 3);
  //  lcd.print (String("A:") + String(A[0]) + " " + String(A[1]) );
  //lcd.print (String("B:") + String(B[0]) + " " + String(B[1]) );
  //  lcd.setCursor(7, 3);
  //  lcd.print(String(A[2]));
  //lcd.print(String(B[2]));
  //    lcd.setCursor(8, 3);
  //    lcd.print(String("p:") + String(digitalRead(pButton)) + String("c:") + String(digitalRead(cButton)) + String("s:") + String(digitalRead(sButton)) + String("m:") + String(mode));


  if (digitalRead(cButton) == LOW && digitalRead(sButton) == HIGH && (consumekWh > 0) ) { //ready to transmit
    mode = 2;
  }
  else if (digitalRead(cButton) == LOW  && consumekWh == 0 ) { //need kWh to consume
    lcd.clear();
    while ( digitalRead(cButton) == HIGH || A[1] == 0 ) {
      key = keypad.getKey();
      keyPad();
      if (consumekWh == 0) {
        if (dirtyScreen == true) {
          lcd.clear();
          dirtyScreen = false;
        }//end if
        lcd.setCursor(0, 0);
        lcd.print("Enter Energy: " + String("  "));

        lcd.setCursor(5, 1);
        lcd.print("Press Buy");
        lcd.setCursor(4, 2);
        lcd.print("to continue");
        lcd.setCursor(3, 3);
        lcd.print("Sell to Reset");

      }

      else if(consumekWh > 0 && consumekWh < 10){
        lcd.setCursor(0, 0);
        lcd.print("Enter Energy: " + String(consumekWh));
        lcd.setCursor(16, 0);
        lcd.print("kWh");
        lcd.setCursor(5, 1);
        lcd.print("Press Buy");
        lcd.setCursor(4, 2);
        lcd.print("to continue");
        lcd.setCursor(3, 3);
        lcd.print("Sell to Reset");
        dirtyScreen = true;
      }

      else{
        if (dirtyScreen == true) {
          lcd.clear();
          dirtyScreen = false;
        }//end if
        lcd.setCursor(0, 0);
        lcd.print("Enter Energy: " + String(consumekWh));
        lcd.setCursor(17, 0);
        lcd.print("kWh");
        lcd.setCursor(5, 1);
        lcd.print("Press Buy");
        lcd.setCursor(4, 2);
        lcd.print("to continue");
        lcd.setCursor(3, 3);
        lcd.print("Sell to Reset");
      }
      if (digitalRead(cButton) == LOW) {

        A[1] = consumekWh;
      }

      if (digitalRead(sButton) == LOW) {

        dirtyScreen=true;
        consumekWh = 0;
        mode = 0;
        break;
      }//end breakout
    }//end while
  }//end else if
  else if (digitalRead(cButton) == HIGH && digitalRead(sButton) == HIGH && (kWh > 0)) { //ready to transmit

    mode = 3;
  }

  switch (mode) {
    case 1:  { // producing
        recvWithEndMarker();
        receivedData();
        lcd.clear();
        //digitalWrite(ledGre, LOW);
        digitalWrite(ledRed, LOW);
        digitalWrite(ledBlu, HIGH);
        str = "Selling: " + String(kWh) + " kWh";
        lcd.setCursor(((20-str.length())/2), 0);
        lcd.print(str);
        lcd.setCursor( 4, 2);
        lcd.print("Transmitting");
        // lcd.setCursor(0, 2);
        // lcd.print("                    ");
        Serial.println(String(self) + " " + String(kWh) + " " + String('0'));
        delay(1250);
        // void intake();

        lcd.setCursor(4, 1);
        lcd.print("Transaction");
        lcd.setCursor(4, 2);
        lcd.print("  Complete    ");
        delay(2000);
        lcd.clear();
        str = "";
        kWh = 0;
        mode = 0;

      }
      break;
    case 2:  { // consuming
        lcd.clear();
        A[1] = consumekWh;
        //digitalWrite(ledGre, LOW);
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledBlu, LOW);
        lcd.setCursor( 1, 0);
        lcd.print(String("Purchasing: ") + String(A[1]) + String(" kWh"));

        while (key == NO_KEY && A[2] == 0 ) {

          if (A[2] == 0) {

            key = keypad.getKey();
            keyPad2();
            lcd.setCursor(1, 1);
            lcd.print("Select Seller: ");
            lcd.setCursor(1, 2);
            lcd.print("or Sell to Cancel");
            if (digitalRead(sButton) == LOW) {
              lcd.clear();
              lcd.setCursor(4, 1);
              lcd.print("Transaction");
              lcd.setCursor(6, 2);
              lcd.print("Canceled");
              dirtyScreen = true;
              mode = 0;
              break;
            }//end breakout

            if (seller > 6) {
              lcd.setCursor(1, 1);
              lcd.print("Invalid Seller");
              lcd.setCursor(0,2);
              lcd.print("                    ");
              seller = 0;
              key = NO_KEY;
              delay(2000);
            }// end not a seller

            else if (seller <= 6) {
              A[2] = seller;
            }//end else if

          }// end seller input

        }//end while
        if (A[2] >= 1 && A[2] <= 6) {

          Serial.println(String(self) + " " + String(A[1]) + " " + String(seller));
          lcd.clear();
          lcd.setCursor( 1, 1);
          lcd.print(String("Purchasing ") + String(A[1]) + String(" kWh"));
          lcd.setCursor(3, 2);
          lcd.print(String("from House ") + String(A[2]));

        }//end sucessful print


        delay (5000);
        lcd.clear();
        consumekWh = 0;
        mode = 0;
        seller = 0;
        A[1] = 0;
        A[2] = 0;
        //        Serial.println("Balance " + String(kWhBalance));
        //        lcd.clear();
      }// end case2
      break;
    case 3: { // Select sell
        recvWithEndMarker();
        receivedData();
        if (dirtyScreen == true) {
          lcd.clear();
          dirtyScreen = false;
        }//end if
        str = "Balance: " + String(kWhBalance) + " kWh";
        lcd.setCursor(((20-str.length())/2), 0);

        lcd.print(str);

        lcd.setCursor(2, 1);
        //        lcd.print("Enter kWh: " + String(kWh));

        lcd.print("Produced: " + String(kWh) + " kWh");
        lcd.setCursor( 2, 2);
        lcd.print("Sell when ready");
        //digitalWrite(ledGre, LOW);
        digitalWrite(ledRed, LOW);
        digitalWrite(ledBlu, LOW);
        if (digitalRead(cButton) == HIGH && digitalRead(sButton) == LOW && kWh > 0)
        { //consume
          mode = 1;
        }// end if
        else if (digitalRead(cButton) == LOW && digitalRead(sButton) == HIGH && kWh > 0)
        { //produce
          mode = 2;
        }//end elseif
        else if (digitalRead(cButton) == HIGH && digitalRead(sButton) == HIGH && kWh == 0) mode = 0;
      }//end case3
      break;

    case 0:  { // default
        recvWithEndMarker();
        receivedData();
        if (dirtyScreen == true) {
          lcd.clear();
          dirtyScreen = false;
        }//end if
        //digitalWrite(ledGre, LOW);
        digitalWrite(ledRed, LOW);
        digitalWrite(ledBlu, LOW);
        str = "Balance: " + String(kWhBalance) + " kWh";

        lcd.setCursor(((20-str.length())/2), 0);

        lcd.print(str);

        lcd.setCursor(0, 1);
        lcd.print("Generate or Purchase");
        if (consumekWh > 0) {
          lcd.setCursor(3, 2);
          lcd.print("Energy " + String(consumekWh) + " kWh");
        }
        else {
          lcd.setCursor(7, 2);
          lcd.print("Energy");
        }

      }//end case0
      break;
      case 5:{
        lcd.clear();
        delay (1000);

      }
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
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {//if numChars
        ndx = numChars - 1;
      }//end if numChars
    }// end if endMarker
    else {
      receivedChars[ndx] = '\0'; // terminate the string
      var = ndx;
      ndx = 0;
      newData = true;
    }// end else
  }// end while
}// end void recvWithEndMarker

void receivedData() {
  if (newData == true) {
    for (int i = 0; i < var; i++) {
      B[0] = receivedChars[0] - '0';
    }; //end for
    B[0] = receivedChars[0] - '0';

    /*  processes incoming char kWh and turns it into an integer */
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

    /* actions with incoming array */
    if (B[0] == self && B[2] == 0) { // update balance
      kWhBalance = B[1]; //writes balance out of array
      if (kWhBalance > 0) digitalWrite(ledGre, HIGH);
      else if(kWhBalance == 0) digitalWrite(ledGre, LOW);
      lcd.clear();
      lcd.setCursor(6, 1);
      lcd.print("Balance");
      lcd.setCursor(6, 2);
      lcd.print("Updated");
      digitalWrite(ledBlu, HIGH);
      delay(5000);
      dirtyScreen = true;
      digitalWrite(ledBlu, LOW);
    }//end update balance

    else if (B[0] == self && B[1] != 0 && B[2] != 0) { // consuming kWh
  for (int i = B[1]; i > 0; i--) { // consuming actions blinks ledRed & countdown
      if (i == B[1] || i==9)lcd.clear();
      lcd.setCursor(4, 1);
      lcd.print(String("From User ") + String(B[2]));

        String str2 = "Consuming: " +String(i) + " kWh";
        lcd.setCursor((20-str2.length())/2, 0);
        lcd.print(str2);

        digitalWrite(ledRed, HIGH);
        delay(250);
        digitalWrite(ledRed, LOW);
        delay(250);
        digitalWrite(ledRed, HIGH);
        delay(250);
        digitalWrite(ledRed, LOW);
        delay(250);
      }// end conusming actions
      lcd.clear();
    }// end consuming kWh
    // else if (B[0] == self && B[1] == 0) { // user doesn't have enough energy
    //   lcd.clear();
    //   lcd.setCursor(0, 0);
    //   //  lcd.print("                    ");//"Seller does not have any energy to sell"
    //   lcd.print("Seller does not have");
    //   lcd.setCursor(0, 1);
    //   lcd.print("any energy to sell");
    //
    //   delay (4000);
    //   dirtyScreen = true;
    // }// end producer doesn't have enough energy

    else {//if data isn't for this device
      B[0] = 0;
      B[1] = 0;
      B[2] = 0;
    }

    kWh = 0;
    newData = false;
  }
}//end void receivedData

void keyPad() {
  if (key == '1')  {
    int i = 1;
    if (consumekWh == 0)
      consumekWh = i;
    else if (consumekWh < 10)
      consumekWh = (consumekWh * 10 + i);

  }//end kWh 1 key

  if (key == '2')  {
    int i = 2;
    if (consumekWh == 0)
      consumekWh = i;
    else if (consumekWh < 10)
      consumekWh = (consumekWh * 10 + i);
  }//end kWh 2 key

  if (key == '3')  {
    int i = 3;
    if (consumekWh == 0)
      consumekWh = i;
    else if (consumekWh < 10)
      consumekWh = (consumekWh * 10 + i);
  }//end kWh 3 key

  if (key == '4')  {
    int i = 4;
    if (consumekWh == 0)
      consumekWh = i;
    else if (consumekWh < 10)
      consumekWh = (consumekWh * 10 + i);
  }//end kWh 4 key

  if (key == '5')  {
    int i = 5;
    if (consumekWh == 0)
      consumekWh = i;
    else if (consumekWh < 10)
      consumekWh = (consumekWh * 10 + i);
  }//end kWh 5 key

  if (key == '6')  {
    int i = 6;
    if (consumekWh == 0)
      consumekWh = i;
    else if (consumekWh < 10)
      consumekWh = (consumekWh * 10 + i);
  }//end kWh 6 key

  if (key == '7')  {
    int i = 7;
    if (consumekWh == 0)
      consumekWh = i;
    else if (consumekWh < 10)
      consumekWh = (consumekWh * 10 + i);
  }//end kWh 7 key

  if (key == '8')  {
    int i = 8;
    if (consumekWh == 0)
      consumekWh = i;
    else if (consumekWh < 10)
      consumekWh = (consumekWh * 10 + i);
  }//end kWh 8 key

  if (key == '9')  {
    int i = 9;
    if (consumekWh == 0)
      consumekWh = i;
    else if (consumekWh < 10)
      consumekWh = (consumekWh * 10 + i);
  }//end kWh 9 key

  if (key == '0')  {
    int i = 0;
    if (consumekWh == 0)
      consumekWh = i;
    else if (consumekWh < 10)
      consumekWh = (consumekWh * 10 + i);
  }//end kWh 0 key

  if (key == 'D')  {
    consumekWh = 00;
    dirtyScreen = true;
  }//end key D;
}//end keyPad

void keyPad2() {
  if (key == '1')  {
    seller = 1;

  }//end kWh 1 key

  if (key == '2')  {
    seller = 2;

  }//end kWh 2 key

  if (key == '3')  {
    seller = 3;

  }//end kWh 3 key

  if (key == '4')  {
    seller = 4;

  }//end kWh 4 key

  if (key == '5')  {
    seller = 5;

  }//end kWh 5 key

  if (key == '6')  {
    seller = 6;

  }//end kWh 6 key

  if (key == '7')  {
    seller = 7;

  }//end kWh 7 key

  if (key == '8')  {
    seller = 8;

  }//end kWh 8 key

  if (key == '9')  {
    seller = 9;

  }//end kWh 9 key

  if (key == '0')  {
    seller = 0;

  }//end kWh 0 key

}//end keyPad
