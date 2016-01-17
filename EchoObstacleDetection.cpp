#include "EchoObstacleDetection.h"
//#include <avr/wdt.h>
#define echoCm 58
#define echoMaxValue 500
uint8_t echoArray[4];
uint8_t triggerArray[4];
boolean alertArray[4];
uint8_t _pinInterrupt;
unsigned long distArray[4];
volatile boolean triggerArrayStatus[4];
volatile unsigned long prevEchoMicro[4];
volatile unsigned long lastEchoMicro[4];
void CheckAlert(uint8_t echoNumber);
EchoObstacleDetection::EchoObstacleDetection(uint8_t pinEcho1 ,uint8_t pinTrigger1 ,uint8_t pinEcho2 ,uint8_t pinTrigger2 ,uint8_t pinEcho3 ,
												uint8_t pinTrigger3 ,uint8_t pinEcho4 ,uint8_t pinTrigger4 ,uint8_t pinInterrupt)
{
  _pinEcho1 = pinEcho1;
  _pinTrigger1 = pinTrigger1;
  _pinEcho2 = pinEcho2;
  _pinTrigger2 = pinTrigger2;
  _pinEcho3 = pinEcho3;
  _pinTrigger3 = pinTrigger3;
  _pinEcho4 = pinEcho4;
  _pinTrigger4 = pinTrigger4;
  _pinInterrupt=pinInterrupt;
  
}
	void Echo1Interrupt()
{
 // Serial.print("echo1:");
//  Serial.println(micros());
//  Serial.println(digitalRead(echoArray[0]));
  if (digitalRead(echoArray[0]) == 1)
  {
    prevEchoMicro[0] = micros();
  }
  else
  {
    lastEchoMicro[0] = micros() - prevEchoMicro[0];
	CheckAlert(0);
	//   Serial.print("change:");
//    Serial.println(lastEchoMicro[0]/58);
  }

}

	void Echo2Interrupt()
{

  if (digitalRead(echoArray[1]) == 1)
  {
    prevEchoMicro[1] = micros();
  }
  else
  {
    lastEchoMicro[1] = micros() - prevEchoMicro[1];

  }
}
	void Echo3Interrupt()
{
  if (digitalRead(echoArray[2]) == 1)
  {
    prevEchoMicro[2] = micros();
  }
  else
  {
    lastEchoMicro[2] = micros() - prevEchoMicro[2];
  }
}
	void Echo4Interrupt()
{
  if (digitalRead(echoArray[3]) == 1)
  {
    prevEchoMicro[3] = micros();
  }
  else
  {
    lastEchoMicro[3] = micros() - prevEchoMicro[3];
  }
}
ISR(TIMER5_OVF_vect)        
{
  TCNT5 = 1000;            // preload timer
  digitalWrite(13, !digitalRead(13));
	for (uint8_t idx=0;idx<4;idx++)
	{
		if (echoArray[idx]!=0)
		{
	//		if (triggerArrayStatus[idx]==false)
	//		{
			  digitalWrite(triggerArray[idx], LOW);  //
			  delayMicroseconds(2);    
			  digitalWrite(triggerArray[idx], HIGH);  //			  
			  delayMicroseconds(15); // 10 micro sec mini
			  digitalWrite(triggerArray[idx], HIGH);  //
			  triggerArrayStatus[idx]=true;
	//		  Serial.print("trig:");
	//		  Serial.println(idx+1); 
	//		}
		

		}
	}
}

	
   void EchoObstacleDetection::StartDetection(boolean echo1, boolean echo2, boolean echo3, boolean echo4, unsigned int frequency)
{
	if (echo1)
	{
		echoArray[0]=_pinEcho1;
		triggerArray[0]=_pinTrigger1;
		triggerArrayStatus[0]=false;
//		CheckAlert(0);
	}
	if (echo2)
	{
		echoArray[1]=_pinEcho2;
		triggerArray[1]=_pinTrigger2;
		triggerArrayStatus[1]=false;
	}
	if (echo3)
	{
		echoArray[2]=_pinEcho3;
		triggerArray[2]=_pinTrigger3;
		triggerArrayStatus[2]=false;
	}
	if (echo4)
	{
		echoArray[3]=_pinEcho4;
		triggerArray[3]=_pinTrigger4;
		triggerArrayStatus[3]=false;
	}
	for (uint8_t idx=0;idx<4;idx++)
	{
		Serial.println(echoArray[idx]);
		if (echoArray[idx]!=0)
		{
			pinMode(echoArray[idx], INPUT);
			pinMode(triggerArray[idx], OUTPUT);
		}	
		if (echoArray[0]!=0)
		{
			attachInterrupt(digitalPinToInterrupt(echoArray[0]), Echo1Interrupt, CHANGE);
		}
		if (echoArray[1]!=0)
		{
			attachInterrupt(digitalPinToInterrupt(echoArray[1]), Echo2Interrupt, CHANGE);
		}
		if (echoArray[2]!=0)
		{
			attachInterrupt(digitalPinToInterrupt(echoArray[2]), Echo3Interrupt, CHANGE);
		}
			if (echoArray[3]!=0)
		{
			attachInterrupt(digitalPinToInterrupt(echoArray[3]), Echo4Interrupt, CHANGE);
		}
	}
	pinMode(13, OUTPUT);
	noInterrupts(); // disable all interrupts
	TCCR5A = 0;
	TCCR5B = 0;
	TCNT5 = 1000; // preload timer 65536-16MHz/256/2Hz
	TCCR5B |= (1 << CS12); // 256 prescaler
	TIMSK5 |= (1 << TOIE5); // enable timer overflow interrupt
	interrupts(); // enable all interrupts
//	ActivateTriggers();
}
   void EchoObstacleDetection::StopDetection(boolean echo1, boolean echo2, boolean echo3, boolean echo4)
{
		if (echo1)
	{
		echoArray[0]=0x00;
		triggerArray[0]=0x00;
		triggerArrayStatus[0]=false;
		detachInterrupt(digitalPinToInterrupt(echoArray[0]));
		lastEchoMicro[0]=0;
	}
		if (echo2)
	{
		echoArray[1]=0x00;
		triggerArray[1]=0x00;
		triggerArrayStatus[1]=false;
		detachInterrupt(digitalPinToInterrupt(echoArray[1]));
		lastEchoMicro[1]=0;
	}
		if (echo3)
	{
		echoArray[2]=0x00;
		triggerArray[2]=0x00;
		triggerArrayStatus[2]=false;
		detachInterrupt(digitalPinToInterrupt(echoArray[2]));
		lastEchoMicro[2]=0;
	}
		if (echo4)
	{
		echoArray[3]=0x00;
		triggerArray[3]=0x00;
		triggerArrayStatus[3]=false;
		detachInterrupt(digitalPinToInterrupt(echoArray[3]));
		lastEchoMicro[3]=0;
	}
}
   void EchoObstacleDetection::SetAlertOn(boolean echo1,unsigned int dist1,boolean echo2,unsigned int dist2,boolean echo3,unsigned int dist3,boolean echo4,unsigned int dist4)
 {
	if (echo1)
	{
		alertArray[0]=true;
		distArray[0]=dist1;
	}
		if (echo2)
	{
		alertArray[1]=true;
		distArray[1]=dist1;
	}
	
		if (echo3)
	{
		alertArray[2]=true;
		distArray[2]=dist1;
	}

		if (echo4)
	{
		alertArray[3]=true;	
		distArray[3]=dist1;
	}
}
unsigned int EchoObstacleDetection::GetDistance(uint8_t echoNumber)
{
	unsigned int retCode=0;
	if (echoArray[echoNumber!=0])
	{
		retCode= lastEchoMicro[echoNumber]/echoCm;
		if (retCode > echoMaxValue)
		{
			retCode=0;
		}
		
	}

	return retCode;

}
void CheckAlert(uint8_t echoNumber)
{
	if (lastEchoMicro[echoNumber]/echoCm < distArray[echoNumber])
	{
		Serial.println("alert");
		if (_pinInterrupt!=0)
		{
			pinMode(_pinInterrupt,OUTPUT);		
			digitalWrite(_pinInterrupt,1);
		}
		digitalWrite(_pinInterrupt,1);
	}
}
