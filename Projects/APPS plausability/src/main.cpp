#include <Arduino.h>

const uint8_t Sensor1 = A0;
const uint8_t Sensor2 = A1;
const uint8_t Brake = A4;
const uint8_t RelaySwitch = 3; //output to relay, needs a resistor to limit current to <40mA to make sure arduino pin dont die

const float PrimaryOffsetLow = 0.60; //Change this based on the voltage readout of the primary throttle pedal sensor
const float SecondaryOffsetLow = 2.40; //change this based on the voltage readout of the secondary throttle pedal sensor
const float PrimaryOffsetHigh = 1.60;
const float SecondaryOffsetHigh = 4.60;



int PrimarySensor = 0;
int SecondarySensor = 0;
float PrimaryVoltage = 0.0;
float SecondaryVoltage = 0.0;
int PrimaryPercentage = 0;
int SecondaryPercentage = 0;
int diff = 0;


int CalculateThrottlePercentage (float SensorVoltage, float low, float high) {
  int Percentage = (SensorVoltage/(high - low)) * 100;

  return Percentage;
}

int CalculatePercentDiff (int PercentOne, int PercentTwo) {
  int PercentDiff = abs(PercentOne - PercentTwo);

  return PercentDiff;
}

float CalculateInputVoltage (float SensorValue, float SensorOffset) {
  float Voltage = (SensorValue * (5.0/1023.0)) - SensorOffset;

  return Voltage;
}

bool ReadBrakeStatus(uint8_t brakePin) {
  int sensVal = analogRead(brakePin);
  float brakeVoltage = CalculateInputVoltage(sensVal, 0);
  if (brakeVoltage <= 2.50) { //if the brake is pressed, will be GND
    return true;
  } 
  return false;
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(RelaySwitch,OUTPUT);
  digitalWrite(RelaySwitch, LOW);

}

void loop() {
  // put your main code here, to run repeatedly:
  PrimarySensor = analogRead(Sensor1);
  SecondarySensor = analogRead(Sensor2);

  PrimaryVoltage = CalculateInputVoltage(PrimarySensor, PrimaryOffsetLow);
  SecondaryVoltage = CalculateInputVoltage(SecondarySensor, SecondaryOffsetLow);

  PrimaryPercentage = CalculateThrottlePercentage(PrimaryVoltage, PrimaryOffsetLow, PrimaryOffsetHigh);
  SecondaryPercentage = CalculateThrottlePercentage(SecondaryVoltage, SecondaryOffsetLow, SecondaryOffsetHigh);

  PrimaryPercentage = (PrimaryVoltage/(PrimaryOffsetHigh - PrimaryOffsetLow)) * 100;
  SecondaryPercentage = (SecondaryVoltage/(SecondaryOffsetHigh - SecondaryOffsetLow)) * 100;

  diff = CalculatePercentDiff(PrimaryPercentage, SecondaryPercentage);

  
  Serial.println(PrimaryVoltage + PrimaryOffsetLow);
  Serial.println(SecondaryVoltage + SecondaryOffsetLow);
  Serial.println(analogRead(Brake)*(5.0/1023.0));
  Serial.println("Voltage Sensor 1, 2, and Brake");
  Serial.println(diff);
  Serial.println("Percentage Difference");
  Serial.println(PrimaryPercentage);
  Serial.println(SecondaryPercentage);
  Serial.println("% Throttle Inputs");
  Serial.println("");
  

  // Plausability Check
  if (diff <= 20) {
    digitalWrite(RelaySwitch, HIGH);

  } else {
    digitalWrite(RelaySwitch, LOW);
    Serial.println("Plausibility FAILED");
    Serial.println("");
    return;
  }
  
  //25% throttle & brake plausibility check=========
  if (PrimaryPercentage > 25 && ReadBrakeStatus(Brake)) { //if the accelator is >25% and the brake is pressed, turn off torque to motor
      while (PrimaryPercentage > 5 || ReadBrakeStatus(Brake)) { //while the throttle is >=5% or thr brake remains pressed, keep the torque to the motor off 
        
        PrimarySensor = analogRead(Sensor1);
        PrimaryVoltage = CalculateInputVoltage(PrimarySensor, PrimaryOffsetLow);
        PrimaryPercentage = (PrimaryVoltage/(PrimaryOffsetHigh - PrimaryOffsetLow)) * 100;

        digitalWrite(RelaySwitch, LOW);
        Serial.println("Accel Cutoff");
    }
  }
  // debug to see what is going on 
}