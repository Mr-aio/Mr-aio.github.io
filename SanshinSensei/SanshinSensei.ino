Sanshin Sensei Version 1.0

Authors: Mario DePavia (lines 39-40, 45-50, 73-210) and Clyde A. Lettsome, PhD, PE, MEM (lines 27-38, 41-44, 51-72)
 
 General Description: The following code is for Sanshin Sensei, an attatchable sleeve for the sanshin that helps beginners learn how to play. Use a keyboard app capable of generating stable sinewaves corresponding to notes of Hon chōshi.
 Once the mode button is pressed, the Sanshin Sensei will then light up the finger positions on the sanshin in order corresponding to the notes played on the keyboard. The finger position will itterate every time a note is played. A model 
 of the sleeve is available on my github (https://github.com/Mr-aio/Mr-aio.github.io) under the file name sleeveModel.svg and can be used with a CAD laser cutter to cut your own sleeve out of the material of your choice (I would
 recommend a material with a cushioned underside to prevent scratching your sanshin).

 Materials required: Sleeve, zipties (to mount to sanshin), 11 LEDS, analogue microphone (I used an OSEPP Sound Sensor), Analog-equiped Arduino with at least 11 digital output pins (I used an UNO), wire

 Pin Out: 
 1. Attatch the microphone output to A0, as well as the power and ground pins to the arduino's 5V and GND pins. 
 2. Attatch LEDs in assending order to digital pins, starting at pin 0 and ending at pin 10 (e.g. position 合 to pin 0, 乙 to pin 1, etc.). All LEDs should be grounded by the arduino's digital GND pin.
 3. Attach the mode button to pin 11 and the arduino's 5V output
  
 Note: The arduinoFFT.h library needs to be added to the Arduino IDE before compiling and uploading this script/sketch to an Arduino.

 License: This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License (GPL) version 3, or any later
 version of your choice, as published by the Free Software Foundation.

 I referenced code written by Clyde A. Lettsome to utilize the arduinoFFT library in order to interperet the frequencies of the sinewave keyboard.
 For more information about Dr. Lettsome's contributions, visit https://clydelettsome.com/blog/2019/12/18/my-weekend-project-audio-frequency-detector-using-an-arduino/

*/

#include "arduinoFFT.h"

#define SAMPLES 128             //SAMPLES-pt FFT. Must be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 2048 //Ts = Based on Nyquist, must be 2 times the highest expected frequency.
 
arduinoFFT FFT = arduinoFFT();
 
unsigned int samplingPeriod;
unsigned long microSeconds;
 
double vReal[SAMPLES]; //create vector of size SAMPLES to hold real values
double vImag[SAMPLES]; //create vector of size SAMPLES to hold imaginary values
char notes[200];
int range = 11;
void setup() 
{
    Serial.begin(115200); //Baud rate for the Serial Monitor
    samplingPeriod = round(1000000*(1.0/SAMPLING_FREQUENCY)); //Period in microseconds 
    for(int x = 0; x < range; x++){
        pinMode(x, OUTPUT);
    }
    pinMode(range, INPUT);
}

int readNotes() 
{  

    /*Sample SAMPLES times*/
    for(int i=0; i<SAMPLES; i++)
    {
        microSeconds = micros();    //Returns the number of microseconds since the Arduino board began running the current script. 
     
        vReal[i] = analogRead(0); //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
        vImag[i] = 0; //Makes imaginary term 0 always

        /*remaining wait time between samples if necessary*/
        while(micros() < (microSeconds + samplingPeriod))
        {
          //do nothing
        }
    }
 
    /*Perform FFT on samples*/
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
    int x = 0;
    while(x < sizeof(notes)&& digitalRead(range)==0){
    /*Find peak frequency and print peak*/
    int peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY)/10;
    //The below is code written by Mario and David as well as the above division by 10 so we can account for slight variations in frequency caused by poor sound sensor quality
    /*
     * This if statement is designed to prevent our program for picking up possible background frequencies and registering them as notes. 50 was an arbitrary choice but I found
     * that it was a good threshold based on expiremental readings I took with the sensor.
     */
    if(analogRead(A0)>50){
         switch(peak){
             //low C
             case 25:
                 /*The low C we are using on my piano app outputs a frequency in the 250's (in Hz). Pins 10 through 13 communicate to the pi which note was played digitally (the pi doesn't
                  *have analog input and we didn't have access to an analog-to-digital converter. We needed 12 distinguishable codes so we needed a minimum of 4 pins. 10 is the
                  *most significant pin, and we agreed that low C would have a code of 1000.
                  */
                 notes[x] = 'c';
                 x++;
                 digitalWrite(0, HIGH);
                 break;
             //low D
             case 27:
             case 28:
                 //The low D frequency fluctuates between the 270's and 280's, hence the two cases. We agreed the D's code would be 0100.
                 notes[x] = 'd';  
                 x++;  
                 digitalWrite(0, HIGH);          
                 break;
             //low E
             case 31:
                 //Low E's frequency is in the 310's and its digitalized code is 0010.
                 notes[x] = 'e';
                 x++;
                 digitalWrite(0, HIGH);
                 break;
             //low F
             case 33:
                 //Low F is in the 330's and has a digital code of 0001.
                 notes[x] = 'f';
                 x++;
                 digitalWrite(0, HIGH);
                 break;
             //G
             case 37:
                 //G is in the 370's and has a digital code of 1100.
                 notes[x] = 'g';
                 x++;
                 digitalWrite(0, HIGH);
                 break;
             //A
             case 42:
                 //A is in the 420's and has a digital code of 0110.
                 notes[x] = 'a';
                 x++;
                 digitalWrite(0, HIGH);
                 break;
             //A#
             case 44:
             case 45:
                 //A# fluctuates between the 440's and 450's and has a digital code of 0011.
                 notes[x] = 'A';
                 x++;
                 digitalWrite(0, HIGH);
                 break;
             //high C
             case 50:
                 //High C is in the 500's and has a digital code of 1010.
                 notes[x] = 'C';
                 x++;
                 digitalWrite(0, HIGH);
                 break;
             //high D
             case 56:
                 // High D is in the 560's and has a digital code of 1001.
                 notes[x] = 'D';
                 x++;
                 digitalWrite(0, HIGH);
                 break;
             //high E
             case 63:
                 //High E is in the 630's and has a digital code of 0101.
                notes[x] = 'E';
                 x++;
                 digitalWrite(0, HIGH);
                 break;
             //high F
             case 67:
                 notes[x] = 'F';
                 x++;
                 digitalWrite(0, HIGH);
                 break;
         //If we got this far without writing a code, we picked up a note the sanshin can't play. 
         }
     //ensure the note ends and is not registered twice
     while(analogRead(A0)>50);
     digitalWrite(0, LOW);
     }
    }
    return sizeof(notes);
}

void displayNotes(int numOfNotes){
  int x = 0;
  while(x < numOfNotes){
    switch(notes[x]){
        case 'c':
            digitalWrite(0, HIGH);
        case 'd':
            digitalWrite(1, HIGH);
        case 'e':
            digitalWrite(2, HIGH);
        case 'f':
            digitalWrite(3, HIGH);
        case 'g':
            digitalWrite(4, HIGH);
        case 'a':
            digitalWrite(5, HIGH);
        case 'A':
            digitalWrite(6, HIGH);
        case 'C':
            digitalWrite(7, HIGH);
        case 'D':
            digitalWrite(8, HIGH);
        case 'E':
            digitalWrite(9, HIGH);
        case 'F':
            digitalWrite(10, HIGH);
    }
    while(analogRead(A0) > 50);    
    x++;
  }
}

void loop(){
int numNotes = readNotes();
displayNotes(numNotes);
}
