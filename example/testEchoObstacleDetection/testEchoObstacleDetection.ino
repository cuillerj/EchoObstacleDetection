#include <EchoObstacleDetection.h>
#define pinEcho0 20
#define pinTrigger0 22
#define pinEcho1 21
#define pinTrigger1 23
#define pinInterrupt 3
#define echoCycleDuration 0.5
#define echo2 false  // does not exit
#define echo2Alert false  // does not exit
#define echo3 false  // does not exit
#define echo3Alert false  // does not exit
boolean echo0 = true;
boolean echo0Alert = true; // does not exit
boolean echo1 = true;
boolean echo1Alert = false; // does not exit
EchoObstacleDetection echo(pinEcho0, pinTrigger0, pinEcho1, pinTrigger1, 0, 0, 0, 0, pinInterrupt);
boolean boot = true;
int i = 0;
void setup() {
  Serial.begin(38400);
  Serial.println("start");
  attachInterrupt(digitalPinToInterrupt(pinInterrupt), alertInterrupt, RISING);
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  if (boot)
  {
    Serial.println("start detection");
    echo.StartDetection(echo0, echo1, echo2, echo3, echoCycleDuration);
    boot = false;
  }
  delay(2000);
  if (echo0)
  {
    Serial.print("distance 0:");
    Serial.println(echo.GetDistance(0));
  }
  if (echo1)
  {
    Serial.print("distance 1:");
    Serial.println(echo.GetDistance(1));
  }
  if (i == 5)
  {
    echo1 = false;
    echo.StopDetection(echo0, echo1, echo2, echo3);
    echo.SetAlertOn(echo0Alert, 20, echo1Alert, 0, echo2Alert, 0, echo3Alert, 0);
  }
  if (i == 20)
  {
    echo0Alert=false;
    echo.SetAlertOff(echo0Alert, echo0Alert, echo0Alert, echo0Alert);
  }

  i++;
  //echo.StopDetection(false,true,false,false);
}
void alertInterrupt()
{
  Serial.print("obstacle:");
  Serial.println(echo.GetDistance(echo.GetAlertEchoNumber()));
  digitalWrite(pinInterrupt, LOW);
}



