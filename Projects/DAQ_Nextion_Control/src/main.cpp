#include <Arduino.h>

// We assigned a name LED pin to pin number 22
// this will assign the name PushButton to pin numer 15

const int SelectBTN = 3;
const int rotaryS1 = A0;
const int rotaryS2 = A1;

int SelectState = 0;
int S1val = 0;
int S2val = 0;
int PowerPercentage = 0;

int incomingByte = 0;

//float prevPercentage = 0.0;

bool buttonPressed = false;
String currPage = "idle";




// This Setup function is used to initialize everything 
void setup() {

  Serial.begin(115200); //using arduino baud rate of 115200
  pinMode(SelectBTN, INPUT_PULLUP); // This statement will declare pin 15 as digital input 
  pinMode(rotaryS1, INPUT);
  pinMode(rotaryS2, INPUT);

  delay(2000);
  SelectState = digitalRead(SelectBTN);
  S1val = analogRead(rotaryS1);
  S2val = analogRead(rotaryS2);

  Serial.print("page 1");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
  currPage = "page 1";
  Serial.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial.write(0xff);  
  Serial.write(0xff); 
}

void loop() {

  if (Serial.available() > 0) {
    incomingByte = Serial.read();
  }
  // digitalRead function stores the digital val of the switch
  //analogRead stores the analog val of the rotary switch
  // in variable push_button_state
  SelectState = digitalRead(SelectBTN);
  S1val = analogRead(rotaryS1);
  S2val = analogRead(rotaryS2);

  /*
  Serial.print(S1val);
  Serial.print(", ");
  Serial.println(S2val);
  */
  if (!(S1val == 0 && S2val > 1000) != !(S2val == 0 && S1val > 1000)) { //XOR comparator
    if(S2val == 0) {
      //Serial.println("clockwise");
      if(PowerPercentage < 100) {
        PowerPercentage += 1;
      } 
    } else {
      //Serial.println("counter-clockwise");
      if(PowerPercentage > 0) {
        PowerPercentage -= 1; 
        }
    }
    if(currPage == "page 2") {
      Serial.print("powerLim.val=");  //
      Serial.print(PowerPercentage);
      Serial.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial.write(0xff);  
      Serial.write(0xff);
      Serial.print("powerPercent.txt=");  //
      Serial.print("\"");
      Serial.print(PowerPercentage);
      Serial.print("\"");
      Serial.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial.write(0xff);  
      Serial.write(0xff);
    } else if(currPage == "page 4") {
      if(PowerPercentage >= 0 && PowerPercentage < 20) {
        Serial.print("modeIndicator.txt=");  //
        Serial.print("\"");
        Serial.print("ACCELERATION");
        Serial.print("\"");
        Serial.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial.write(0xff);  
        Serial.write(0xff);
        Serial.print("modeIndicator.bco=");  //
        Serial.print("63488");
        Serial.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial.write(0xff);  
        Serial.write(0xff);
        
      } else if (PowerPercentage >= 20 && PowerPercentage < 40) {
        Serial.print("modeIndicator.txt=");  //
        Serial.print("\"");
        Serial.print("AUTOCROSS");
        Serial.print("\"");
        Serial.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial.write(0xff);  
        Serial.write(0xff);
        Serial.print("modeIndicator.bco=");  //
        Serial.print("2047");
        Serial.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial.write(0xff);  
        Serial.write(0xff);
      } else if (PowerPercentage >= 40 && PowerPercentage < 60) {
        Serial.print("modeIndicator.txt=");  //
        Serial.print("\"");
        Serial.print("ENDURANCE");
        Serial.print("\"");
        Serial.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial.write(0xff);  
        Serial.write(0xff);
        Serial.print("modeIndicator.bco=");  //
        Serial.print("64512");
        Serial.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial.write(0xff);  
        Serial.write(0xff);
      } else if (PowerPercentage >= 60 && PowerPercentage <= 80) {
        Serial.print("modeIndicator.txt=");  //
        Serial.print("\"");
        Serial.print("EFFICIENCY");
        Serial.print("\"");
        Serial.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial.write(0xff);  
        Serial.write(0xff);
        Serial.print("modeIndicator.bco=");  //
        Serial.print("2016");
        Serial.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial.write(0xff);  
        Serial.write(0xff);
      } else if (PowerPercentage >= 80 && PowerPercentage <= 100) {
        Serial.print("modeIndicator.txt=");  //
        Serial.print("\"");
        Serial.print("SKID-PAD");
        Serial.print("\"");
        Serial.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial.write(0xff);  
        Serial.write(0xff);
        Serial.print("modeIndicator.bco=");  //
        Serial.print("33808");
        Serial.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial.write(0xff);  
        Serial.write(0xff);
      } 
    }
  }


  
  //Serial.println(PowerPercentage);

  
  // if condition checks if push button is pressed
  // if pressed LED will turn on otherwise remain off 
  if (SelectState == LOW && !buttonPressed) { // button is pressed when state is low since it is pulled to GND
    //enter button on nextion
    if (currPage == "page 1") {
        buttonPressed = true; 
        Serial.print("page 2");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
        currPage = "page 2";
        Serial.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial.write(0xff);  
        Serial.write(0xff);   
        delay(200); 
    } else if (currPage == "page 2") {
        buttonPressed = true; 
        Serial.print("page 3");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
        currPage = "page 3";
        Serial.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial.write(0xff);  
        Serial.write(0xff);   
        delay(200);       
    } else if (currPage == "page 3") {
        buttonPressed = true; 
        Serial.print("page 4");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
        currPage = "page 4";
        Serial.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial.write(0xff);  
        Serial.write(0xff);   
        delay(200);  
    } else if (currPage == "page 4") {
        buttonPressed = true; 
        Serial.print("page 1");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
        currPage = "page 1";
        Serial.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial.write(0xff);  
        Serial.write(0xff);   
        delay(200);  
    }
  } else if (SelectState == HIGH) {
    buttonPressed = false;   
  }
  





}