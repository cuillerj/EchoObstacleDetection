/*
designed for atmega
use timer 4 so pin 6 7 8 can not be used as PMW sources
object can manage from 1 to 4 HCSR module (tested with HC SR04) and eventually can interrupt the main code
first instantiate the object to define pins used for triger and echo measurement and define the pin to be interrupt	
	4 x couples(echo pin,trigger pin) must be defined - 0 meaning HCRS does not exist or will not used	
	1 x interrupt pin must be defined - 0 meaning interrupt will not be used
Start Detection is called to activate HCSR devices	
	1 to 4 HSCR can be activated simultaneously but echo must not interact eachother (true active, false inactive)	
	cycleDuration define delay between 2 triggers en seconds - must remain between 0.1s to 4.9s
SetAlertOn is used to activate interruption (the main code must attach this interrupt) when distances are below defined thresholds
	4 x couples (true, threshold) must be defined - (false,0) meaning not used
GetAlertEchoNumber provide which is the last echo system that was under threshold
GetDistance provide last distance mesurment for the specified HCSR
SetAlertOff switch off alert for HSCR that are no longer in true state
StopDetection stop detection for HSCR that are no longer in true state

*/
#include <Arduino.h>
#ifndef echoObstacleDetection_h_included
#define echoObstacleDetection_h_included
#define echoCm 58           // sound speed 
#define echoMaxValue 450    // 450 cm max for hsr04
#define minTimerCycle 0.1   // echo round trip limitation

class EchoObstacleDetection
{
public:
EchoObstacleDetection(uint8_t pinEcho1 ,uint8_t pinTrigger1 ,uint8_t pinEcho2 ,uint8_t pinTrigger2 ,uint8_t pinEcho3 ,
					uint8_t pinTrigger3 ,uint8_t pinEcho4 ,uint8_t pinTrigger4 ,uint8_t pinInterrupt);
void StartDetection(boolean echo1,boolean echo2,boolean echo3,boolean echo4,float cycleDuration);
void StopDetection(boolean echo1,boolean echo2,boolean echo3,boolean echo4);
void SetAlertOn(boolean echo1,unsigned int dist1,boolean echo2,unsigned int dist2,boolean echo3,unsigned int dist3,boolean echo4,unsigned int dist4);
void SetAlertOff(boolean echo1,boolean echo2,boolean echo3,boolean echo4);
unsigned int GetDistance(uint8_t  echoNumber);
uint8_t GetAlertEchoNumber();

uint8_t _pinEcho1;
uint8_t _pinTrigger1;
uint8_t _pinEcho2;
uint8_t _pinTrigger2;
uint8_t _pinEcho3;
uint8_t _pinTrigger3;
uint8_t _pinEcho4;
uint8_t _pinTrigger4;

};
#endif