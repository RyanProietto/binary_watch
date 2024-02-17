#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>

// Definitions
const int LEDPins[] = {3, 4, 5, 6}; // Pins for LEDs (4 LEDs)
const int buttonPin = 2;            // Pin to push button (interrupt pin)

// Global Volatile Variables
volatile int seconds = 0;
volatile int minutes = 42;
volatile int hours = 8;
volatile bool buttonPressed = false;

void setup() 
{
  // Set LED pins as outputs
  for (int i = 0; i < sizeof(LEDPins) / sizeof(LEDPins[0]); i++) 
  {
    pinMode(LEDPins[i], OUTPUT);
  }
  pinMode(buttonPin,INPUT_PULLUP); // Set the button as an input

  // Set up Timer1 (16-bit Timer/Counter1)
  TCCR1A = 0; // Set Timer/Counter1 Control Register A to 0
  TCCR1B = (1 << CS12) | (1 << CS11) | (1 << CS10); // Set prescaler to 1024
  TIMSK1 = (1 << TOIE1); // Enable Timer/Counter1 Overflow Interrupt

  // Set up external interrupt for button
  attachInterrupt(digitalPinToInterrupt(buttonPin), wake, LOW);

  // Enable sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
}

void wake() 
{
  sleep_disable(); // Disable sleep mode
}

void displayBinary(int value, int delayTime) 
{
  // Display value in binary on specified pins
  for (int i = 0; i < sizeof(LEDPins) / sizeof(LEDPins[0]); i++) 
  {
    digitalWrite(LEDPins[i], (value >> i) & 1);
  }

  // Measure the time taken to set LEDs and subtract it from delayTime
  unsigned long startTime = millis();
  unsigned long elapsedTime = 0;
  do 
  {
    elapsedTime = millis() - startTime;
  } while (elapsedTime < delayTime);
}

ISR(TIMER1_OVF_vect) 
{
  seconds++; // Increment seconds
  if (seconds >= 60) { // If 60 seconds have elapsed
    seconds = 0; // Reset seconds
    minutes++; // Increment minutes
    if (minutes >= 60) { // If 60 minutes have elapsed
      minutes = 0; // Reset minutes
      hours++; // Increment hours
      if (hours >= 24) { // If 24 hours have elapsed
        hours = 0; // Reset hours
      }
    }
  }
}

void checkButton() 
{
  if (digitalRead(buttonPin) == LOW) 
  { // If button is pressed
    buttonPressed = true;
  }
}

void displayTime() 
{
    // Display hours in binary
    displayBinary(hours, 1000); // 2-second delay
    
    // Display tens place of minutes in binary
    displayBinary(minutes / 10, 1000); // 2-second delay
    
    // Display ones place of minutes in binary
    displayBinary(minutes % 10, 1000); // 2-second delay

    for (int pin : LEDPins) 
    {
      digitalWrite(pin, LOW);
    }
}

void loop() 
{
  sleep_mode(); // Enter sleep mode
  
  checkButton(); // Check if button is pressed
  
  if (buttonPressed) 
  {
    displayTime(); // Show time if button is pressed
  }
  buttonPressed = false; // Reset button flag
}