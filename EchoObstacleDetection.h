#include <Arduino.h>

#ifndef echoObstacleDetection_h_included
#define echoObstacleDetection_h_included
class EchoObstacleDetection
{
public:
EchoObstacleDetection(uint8_t pinEcho1 ,uint8_t pinTrigger1 ,uint8_t pinEcho2 ,uint8_t pinTrigger2 ,uint8_t pinEcho3 ,uint8_t pinTrigger3 ,uint8_t pinEcho4 ,uint8_t pinTrigger4 ,uint8_t pinInterrupt);
void StartDetection(boolean echo1,boolean echo2,boolean echo3,boolean echo4,unsigned int frequency);
void StopDetection(boolean echo1,boolean echo2,boolean echo3,boolean echo4);
void SetAlertOn(boolean echo1,unsigned int dist1,boolean echo2,unsigned int dist2,boolean echo3,unsigned int dist3,boolean echo4,unsigned int dist4);
unsigned int GetDistance(uint8_t  echoNumber);
//void CheckAlert(uint8_t echoNumber);
uint8_t _pinEcho1;
uint8_t _pinTrigger1;
uint8_t _pinEcho2;
uint8_t _pinTrigger2;
uint8_t _pinEcho3;
uint8_t _pinTrigger3;
uint8_t _pinEcho4;
uint8_t _pinTrigger4;

//int _echoNumber;
unsigned int _frequency;
//void Echo1Interrupt();

};
#endif