#include <Arduino.h>

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200

// Microstepping mode. If you hardwired it to save pins, set to the same value here.
#define MICROSTEPS 1

#define DIR 8
#define STEP 9
#define SLEEP 13
#define LED 4
#define BUTTON 5

#include "DRV8834.h"
#define M0 10
#define M1 11
DRV8834 stepper(MOTOR_STEPS, DIR, STEP, SLEEP, M0, M1);

unsigned long TimeToStop = 0, NextStateTime = 0;

void setup() {
    stepper.begin(200, MICROSTEPS);
    stepper.disable();
    
    pinMode(LED, OUTPUT);
    digitalWrite(LED,HIGH);

    pinMode(BUTTON, INPUT);
    
    Serial.begin(9600);

    TimeToStop = millis();
    NextStateTime = millis();
}

int State = 2;
//State 0 waiting for button press, State 1 when button press
//State 1 wait 1 second before turning on the motors
//State 2 Running motors, State 0 when button held for 1 second

int ButtonDownBefore = 0, LastState = 0;
void loop() {
    
    stepper.disable();
    if(digitalRead(BUTTON) == 1 && State == 0 && ButtonDownBefore == 0) State = 1;

    if(State == 1 && LastState == 0) NextStateTime = millis() + 1000;
    if(State == 1 && NextStateTime < millis()) State = 2;
    
    
    if(State == 2 && digitalRead(BUTTON) == 1 && ButtonDownBefore == 0) TimeToStop = millis() + 1000;
    else if(State == 2 && digitalRead(BUTTON) == 1 && ButtonDownBefore == 1 && TimeToStop < millis()) State = 0;
    else if(ButtonDownBefore == 0) TimeToStop = millis() + 100000;
    ButtonDownBefore = digitalRead(BUTTON);

    digitalWrite(LED,State == 0 || State == 1 ? 0 : 1);
    
    if(State == 2){
      if(LastState == 1) stepper.enable();
      stepper.rotate(10);
    }

    if(State == 0 && LastState == 2) stepper.disable();
    
    LastState = State;
}
