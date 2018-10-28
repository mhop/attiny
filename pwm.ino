/*
 ATtiny85 test code

 Overview:
 This code flashes the LEDs and outputs data on the serial port.
 This is to test the function of the IC at lower voltages.

 This code is designed to run on the ATTiny 25/45/85
 The serial output only works with the larger ATTiny85 IC

 The connections to the ATTiny are as follows:
 ATTiny    Arduino    Info
 Pin  1  - 5          RESET / Rx (Not receiving any data)
 Pin  2  - 3          Tx for serial conenction
 Pin  3  - 4          FET driver (PWM)
 Pin  4  -            GND
 Pin  5  - 0          RED LED (PWM)
 Pin  6  - 1          GREEN LED
 Pin  7  - 2 / A1     Vsensor (Analog)
 Pin  8  -   +Vcc

 See www.samaenergy.org/www.re-innovation.co.uk for more details including flow code

 13/11/13 by Matt Little (matt@re-innovation.co.uk/www.re-innovation.co.uk)

 Updated:
 13/11/13  Added fast PWM frequency - Matt Little


 This example code is in the public domain.
 */

#define F_CPU 1000000  // This is used by delay.h library

#include <stdlib.h>
#include <EEPROM.h>

#include <avr/io.h>        // Adds useful constants
#include <util/delay.h>    // Adds delay_ms and delay_us functions

const int pin[]={-1, 5, 3, 4, -1, 0, 1, 2, -1};
const int ain[] ={-1, 0, 3, 2, -1, -1, -1, 1, -1};

#define INTERNAL2V56NC (6)

const int deviceType = 85;  // 45 = ATTiny45, NO serial output, 85 = AtTiny85, with serial output

const int pwmout = 3;         // attached to here (0, IC pin 5)
const int pot_pin = 2;

float v_out;

// the setup routine runs once when you press reset:
void setup()  {

TCCR0A = 2<<COM0A0 | 2<<COM0B0 | 3<<WGM00;
TCCR0B = 0<<WGM02 | 10<<CS00; // Optional; already set

GTCCR = 1<<PWM1B | 2<<COM1B0;
TCCR1 = 3<<COM1A0 | 10<<CS10;
    /*
    Control Register A for Timer/Counter-0 (Timer/Counter-0 is configured using two registers: A and B)
    TCCR0A is 8 bits: [COM0A1:COM0A0:COM0B1:COM0B0:unused:unused:WGM01:WGM00]
    2<<COM0A0: sets bits COM0A0 and COM0A1, which (in Fast PWM mode) clears OC0A on compare-match, and sets OC0A at BOTTOM
    2<<COM0B0: sets bits COM0B0 and COM0B1, which (in Fast PWM mode) clears OC0B on compare-match, and sets OC0B at BOTTOM
    3<<WGM00: sets bits WGM00 and WGM01, which (when combined with WGM02 from TCCR0B below) enables Fast PWM mode
    */
    //TCCR0A = 2<<COM0A0 | 2<<COM0B0 | 3<<WGM00;


/*
    Control Register B for Timer/Counter-0 (Timer/Counter-0 is configured using two registers: A and B)
    TCCR0B is 8 bits: [FOC0A:FOC0B:unused:unused:WGM02:CS02:CS01:CS00]
    0<<WGM02: bit WGM02 remains clear, which (when combined with WGM00 and WGM01 from TCCR0A above) enables Fast PWM mode
    1<<CS00: sets bits CS01 (leaving CS01 and CS02 clear), which tells Timer/Counter-0 to not use a prescalar
    */
    //TCCR0B = 0<<WGM02 | 1<<CS00;

    /*
    Control Register for Timer/Counter-1 (Timer/Counter-1 is configured with just one register: this one)
    TCCR1 is 8 bits: [CTC1:PWM1A:COM1A1:COM1A0:CS13:CS12:CS11:CS10]
    0<<PWM1A: bit PWM1A remains clear, which prevents Timer/Counter-1 from using pin OC1A (which is shared with OC0B)
    0<<COM1A0: bits COM1A0 and COM1A1 remain clear, which also prevents Timer/Counter-1 from using pin OC1A (see PWM1A above)
    1<<CS10: sets bit CS11 which tells Timer/Counter-1  to not use a prescalar
    */
    //TCCR1 = 0<<PWM1A | 0<<COM1A0 | 1<<CS10;

    /*
    General Control Register for Timer/Counter-1 (this is for Timer/Counter-1 and is a poorly named register)
    GTCCR is 8 bits: [TSM:PWM1B:COM1B1:COM1B0:FOC1B:FOC1A:PSR1:PSR0]
    1<<PWM1B: sets bit PWM1B which enables the use of OC1B (since we disabled using OC1A in TCCR1)
    2<<COM1B0: sets bit COM1B1 and leaves COM1B0 clear, which (when in PWM mode) clears OC1B on compare-match, and sets at BOTTOM
    */
    //GTCCR = 1<<PWM1B | 2<<COM1B0;

  // Set up IO pins
  pinMode(pin[pwmout], OUTPUT);
  pinMode(pin[pot_pin], INPUT);

  //analogReference(INTERNAL2V56NC);  // This sets the internal ref to be 2.56V (or close to this)
  delay(100);  // Wait a while for this to stabilise.
}


// the loop routine runs over and over again forever:
void loop()  {
  int val = analogRead(ain[pot_pin]);

  v_out= map(val, 0, 1024, 0, 255);
  if (v_out==0) v_out=1;
  OCR1B=v_out; // 4
  _delay_ms(1);
}
