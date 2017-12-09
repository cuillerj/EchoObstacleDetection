/*
version 2 release notes
cycle modification to trigger alternativly the echos (previously simultanously)
monitor function added
*/
#include "EchoObstacleDetection.h"
#define maxTimerCycle 4.9   // timer limitation
#define timerFrequency 15625  // 16000000 arduino frequency 1024 prescaler
//#define debugOn

boolean echoOn[4];       // store 1 to 4 echo status on off - added 09/08/2017
uint8_t echoArray[4];       // store 1 to 4 echo pin values
uint8_t triggerArray[4];    // store 1 to 4 trigger pin values
boolean alertArray[4];      // alert flags 
uint8_t actionArray[5];     // store action for each echo and for combination
boolean monitorFlag=false;  // use to differentiate monitor from simple detection mode
uint8_t _pinInterrupt;      // pin used tio interrupt main program
uint8_t lastAlertEchoNumber; // keep last echNumber that was in distance alert  
uint8_t activeEchoNumber=0;  // number of current active echos
uint8_t activeEchoList[4]={0xffffffff};  // keep track of the  of active echos
uint8_t currentTriggerID=0;  // which trigger is currently on
unsigned int tcntEcho ;   // used to init timer overflow for instance 49911 for 1s cycle 
unsigned int _equalMargin;  // margin to check echo equlality with some lenght
unsigned long distArray[5];  // keep last echo distances for each echo pin + a combination
volatile boolean triggerArrayStatus[4];   // 4 switchs that indicate if echo has been triggered on not
volatile unsigned long prevEchoMicro[4];  // 4 timers used to calculate distances
volatile unsigned long lastEchoMicro[5];  // 4 timers that contain last echo duration in ms + combination
volatile uint8_t retryCompareCount[5];  // 4 timers that contain last echo duration in ms + combination
volatile uint8_t retryNACount[5];  // 4 timers that contain last echo duration in ms + combination
void CheckAlert(uint8_t echoNumber);  
void SetInterruptPIN();

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
	if (echoOn[0])
	{
	  if (digitalRead(echoArray[0]) == 1)
	  {
		prevEchoMicro[0] = micros();
	  }
	  else
	  {
		lastEchoMicro[0] = micros() - prevEchoMicro[0];
		CheckAlert(0);
	  }
	}
}

	void Echo2Interrupt()
{
	if (echoOn[1])
	{
	  if (digitalRead(echoArray[1]) == 1)
	  {
		prevEchoMicro[1] = micros();
	  }
	  else
	  {
		lastEchoMicro[1] = micros() - prevEchoMicro[1];
		CheckAlert(1);
	  }
	}
}
	void Echo3Interrupt()
{
	if (echoOn[2])
	{
	  if (digitalRead(echoArray[2]) == 1)
	  {
		prevEchoMicro[2] = micros();
	  }
	  else
	  {
		lastEchoMicro[2] = micros() - prevEchoMicro[2];
		CheckAlert(2);
	  }
	}
}
	void Echo4Interrupt()
{
	if (echoOn[3])
	{
	  if (digitalRead(echoArray[3]) == 1)
	  {
		prevEchoMicro[3] = micros();
	  }
	  else
	  {
		lastEchoMicro[3] = micros() - prevEchoMicro[3];
		CheckAlert(3);		
	  }
	}
}
ISR(TIMER4_OVF_vect)       
/*
 this timer interrupt is used to regurarly trigger echos for active sensors
*/
{
	TCNT4 = tcntEcho;            // preload timer to adjust duration

#if defined(debugOn)
			Serial.print("T:");
			Serial.print(" ");
			Serial.print(activeEchoNumber);
			Serial.print(" ");
			Serial.println(currentTriggerID);
#endif
			  digitalWrite(triggerArray[currentTriggerID], LOW);  //
			  digitalWrite(triggerArray[currentTriggerID], HIGH);  //			  
			  delayMicroseconds(11); 				// 10 micro sec mini for the sensor
			  digitalWrite(triggerArray[currentTriggerID], LOW);  //
			  triggerArrayStatus[currentTriggerID]=true;
	if(activeEchoNumber>0)
	{
		currentTriggerID=(currentTriggerID+1)%(activeEchoNumber);	// set currentTriggerID to the next active one
	}
}

	
   void EchoObstacleDetection::StartDetection(boolean echo1, boolean echo2, boolean echo3, boolean echo4, float cycleDuration)
{
	if (cycleDuration < minTimerCycle)
	{
		cycleDuration = minTimerCycle;
	}
	if (cycleDuration > maxTimerCycle)
	{
		cycleDuration = maxTimerCycle;
	}
	monitorFlag=false;
	tcntEcho=65536-timerFrequency*cycleDuration;  // tcntEcho intialised to such a value that timer overflow will appear after the cycle duration
	activeEchoNumber=0;
	uint8_t activeListIdx=0;	
	if (echo1 && !echoOn[0])
	{
		echoArray[0]=_pinEcho1;
		triggerArray[0]=_pinTrigger1;
		triggerArrayStatus[0]=false;
		echoOn[0]=true;
		activeEchoList[activeListIdx]=0x00;
		activeEchoNumber++;
		activeListIdx++;
	}
	if (echo2 && !echoOn[1])
	{
		echoArray[1]=_pinEcho2;
		triggerArray[1]=_pinTrigger2;
		triggerArrayStatus[1]=false;
		echoOn[1]=true;
		activeEchoList[activeListIdx]=0x01;
		activeEchoNumber++;
		activeListIdx++;
	}
	if (echo3 && !echoOn[2])
	{
		echoArray[2]=_pinEcho3;
		triggerArray[2]=_pinTrigger3;
		triggerArrayStatus[2]=false;
		echoOn[2]=true;
		activeEchoList[activeListIdx]=0x02;
		activeEchoNumber++;
		activeListIdx++;
	}
	if (echo4&& !echoOn[3])
	{
		echoArray[3]=_pinEcho4;
		triggerArray[3]=_pinTrigger4;
		triggerArrayStatus[3]=false;
		echoOn[3]=true;
		activeEchoList[3]=true;
		activeEchoList[activeListIdx]=0x03;
		activeEchoNumber++;
		activeListIdx++;
	}
	for (uint8_t idx=0;idx<4;idx++)
	{
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
	/*
	set timer 4 interrupt parameters
	*/
	noInterrupts(); // disable all interrupts
	TCCR4A = 0;  // set entire TCCR4A register to 0
	TCCR4B = 0;  // set entire TCCR4B register to 0
	TCNT4 = tcntEcho; // set threshold according to the expected cycle
	TCCR4B |= ((1 << CS12) | (1 << CS10)); // 1024 prescaler - frequency=16,000,000Hz/1024 <-> timerFrequency 15625
	TIMSK4 |= (1 << TOIE4); // enable timer overflow interrupt
	interrupts(); // enable all interrupts
}
void EchoObstacleDetection::SetMonitorOn(boolean echo1,unsigned int dist1,uint8_t action1, boolean echo2,unsigned int dist2,uint8_t action2,boolean echo3,
				  unsigned int dist3,uint8_t action3,boolean echo4,unsigned int dist4,uint8_t action4,uint8_t actionSum,unsigned int distSumm,unsigned int equalMargin)
{

#if defined(debugOn)
	Serial.print("*start monitor:");
	Serial.print(echo1);
	Serial.print(" ");
	Serial.print(echo2);
	Serial.print(" ");
    Serial.print(echo3);
	Serial.print(" ");
    Serial.println(echo4);
#endif		
	if (echo1 )
	{
		actionArray[0]=action1;
		alertArray[0]=true;
		distArray[0]=dist1;
		retryCompareCount[0]=0x00;
	}
	if (echo2 )
	{
		actionArray[1]=action2;
		alertArray[1]=true;
		distArray[1]=dist2;
		retryCompareCount[1]=0x00;
	}
	if (echo3 )
	{
		actionArray[2]=action3;
		alertArray[2]=true;
		distArray[2]=dist3;
		retryCompareCount[2]=0x00;
	}
	if (echo4 )
	{
		actionArray[3]=action4;
		alertArray[3]=true;
		distArray[3]=dist4;
		retryCompareCount[3]=0x00;
	}
	actionArray[pseudoEcho]=actionSum;
	distArray[pseudoEcho]=distSumm;
	retryCompareCount[pseudoEcho]=0x00;
	_equalMargin=equalMargin;
	monitorFlag=true;
	for (uint8_t idx=0;idx<4;idx++)
	{
		if (echoArray[idx]!=0)
		{
			pinMode(echoArray[idx], INPUT);
			pinMode(triggerArray[idx], OUTPUT);
#if defined(debugOn)
	Serial.print(echoArray[idx]);
	Serial.print("/");
	Serial.println(triggerArray[idx]);		
#endif	
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

 }
    void EchoObstacleDetection::SetMonitorOff()
	{
		for (uint8_t i=0;i<4;i++)
		{
		alertArray[i]=false;
		distArray[i]=0;
		echoArray[i]=0x00;
		triggerArray[i]=0x00;
		actionArray[i]=0x00;
		triggerArrayStatus[i]=false;
		detachInterrupt(digitalPinToInterrupt(echoArray[i]));
		lastEchoMicro[i]=0;
		alertArray[i]=false;
//		echoOn[i]=false;
		}
		distArray[pseudoEcho]=0;	
		actionArray[pseudoEcho]=0x00;
		monitorFlag=false;
//		activeEchoNumber=0;
	}
   void EchoObstacleDetection::StopDetection()
{
	#if defined(debugOn)
	Serial.print("stop detection");
#endif	
//		if (echo1 && echoOn[0])
//	{
	for (int i=0;i<4;i++)
	{
		echoArray[i]=0x00;
		triggerArray[i]=0x00;
		triggerArrayStatus[i]=false;
		detachInterrupt(digitalPinToInterrupt(echoArray[i]));
		lastEchoMicro[i]=0;
		alertArray[i]=false;
		echoOn[i]=false;
	}
		activeEchoNumber=0;

#if defined(debugOn)
	Serial.print("stop timer interrupt");
#endif	
			noInterrupts(); // disable all interrupts
			TCCR4A = 0;  // set entire TCCR5A register to 0
			TCCR4B = 0;  // set entire TCCR5B register to 0
			TCNT4 = 0; // 
			TCCR4B |= ((0 << CS10) ); // 
			TCCR4B |= ((0 << CS11) ); //
			TCCR4B |= ((0 << CS12) ); // 256 prescaler
			TIMSK4 |= (0 << TOIE4); // enable timer overflow interrupt
			interrupts(); // enable all interrupts
	//		}
}
   void EchoObstacleDetection::SetAlertOn(boolean echo1,unsigned int dist1,boolean echo2,unsigned int dist2,boolean echo3,unsigned int dist3,boolean echo4,unsigned int dist4)
 {
	 if (_pinInterrupt!=0)
	 {
//		pinMode(_pinInterrupt,OUTPUT);
	 }
	if (echo1)
	{
		alertArray[0]=true;
		distArray[0]=dist1;
	}
		if (echo2)
	{
		alertArray[1]=true;
		distArray[1]=dist2;
	}
	
		if (echo3)
	{
		alertArray[2]=true;
		distArray[2]=dist3;
	}

		if (echo4)
	{
		alertArray[3]=true;	
		distArray[3]=dist4;
	}
}
   void EchoObstacleDetection::SetAlertOff(boolean echo1,boolean echo2,boolean echo3,boolean echo4)
 {
	if (!echo1)
	{
		alertArray[0]=false;
		distArray[0]=0;
	}
	if (!echo2)
	{
		alertArray[1]=false;
		distArray[1]=0;
	}
	
	if (!echo3)
	{
		alertArray[2]=false;
		distArray[2]=0;
	}

	if (!echo4)
	{
		alertArray[3]=false;	
		distArray[3]=0;
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
			retCode=999;
		}		
	}

	return retCode;
}
void CheckAlert(uint8_t echoNumber)
{
#if defined(debugOn)
	Serial.print(".");
#endif	
	if (!monitorFlag)     // simple obstacle single detection mode
	{
#if defined(debugOn)
	Serial.print("-");
#endif	
		if (lastEchoMicro[echoNumber]/echoCm <= distArray[echoNumber]) // modified 09082017 to allow monitoring distance becoming 0
		{
			SetInterruptPIN();
		}
	}
	else                  // monitor mode
	{
#if defined(debugOn)
	Serial.print("+");
	Serial.print(echoNumber);
	Serial.print("+");
	Serial.print(lastEchoMicro[echoNumber]);
	Serial.print("a");
	Serial.print(actionArray[echoNumber],HEX);
	Serial.print("r");
	Serial.print(retryCompareCount[echoNumber]);
	Serial.print("d");
	Serial.println(distArray[echoNumber]);
#endif	
		boolean detected=false;
		if( bitRead(actionArray[echoNumber],actionEqualNA))
		{
			if (lastEchoMicro[echoNumber]/echoCm >=echoMaxValue) 
			{
					retryNACount[echoNumber]++;
					if (retryNACount[echoNumber] >=retryCompareNA)
					{
						detected=true;
						lastAlertEchoNumber=echoNumber;
						SetInterruptPIN();
					}
			}
			else
			{
				retryNACount[echoNumber]=0x00;
			}	
		}
		if( bitRead(actionArray[echoNumber],actionUpper))
		{
			if (lastEchoMicro[echoNumber]/echoCm >= distArray[echoNumber]) 
			{
					retryCompareCount[echoNumber]++;
					if (retryCompareCount[echoNumber] >=retryCompareValue)
					{
						detected=true;
						lastAlertEchoNumber=echoNumber;
						SetInterruptPIN();
					}					

			}
			else
			{
				retryCompareCount[echoNumber]=0x00;
			}				
		}
		if( bitRead(actionArray[echoNumber],actionLower))
		{
			if (lastEchoMicro[echoNumber]/echoCm <= distArray[echoNumber]) 
			{
					retryCompareCount[echoNumber]++;
					if (retryCompareCount[echoNumber] >=retryCompareValue)
					{
						detected=true;
						lastAlertEchoNumber=echoNumber;
						SetInterruptPIN();
					}					

			}			
			else
			{
				retryCompareCount[echoNumber]=0x00;
			}	
		}
		/*
		below code for echo combination using pseudo echo
		actionArray[pseudoEcho] bits map with the echos to summ
		*/		
		if( bitRead(actionArray[pseudoEcho],echoNumber))  
		{
			int summEcho=0;
			for (uint8_t i=0;i<4;i++)
			{
				if( bitRead(actionArray[pseudoEcho],i) && lastEchoMicro[i]!=0)				
				{
					summEcho=summEcho+lastEchoMicro[i]/echoCm;
				}
#if defined(debugOn)
					Serial.print("> ");
					Serial.print(i);
					Serial.print(" ");
					Serial.print(lastEchoMicro[i]/echoCm);
					Serial.print(" ");
					Serial.print("summ: ");
					Serial.print(summEcho);
					Serial.print(" ");
					Serial.print(retryCompareCount[pseudoEcho]);
					Serial.print(" 0x");
					Serial.print(actionArray[pseudoEcho],HEX);
				Serial.println();
#endif
			}

			if (bitRead(actionArray[pseudoEcho],actionLower))
			{
				if (summEcho<= distArray[pseudoEcho]) 
				{
					retryCompareCount[pseudoEcho]++;
					if (retryCompareCount[pseudoEcho]=retryCompareValue)
					{
						detected=true;	
					}
				}
				else{
					retryCompareCount[pseudoEcho]=0x00;
				}
			}	
			
			if (bitRead(actionArray[pseudoEcho],actionUpper) && !detected)
			{
				if (summEcho>= distArray[pseudoEcho]) 
				{
					retryCompareCount[pseudoEcho]++;
					if (retryCompareCount[pseudoEcho]>=retryCompareValue)
					{
						detected=true;	
					}
				}
				else{
					retryCompareCount[pseudoEcho]=0x00;
				}
			}	

			if (bitRead(actionArray[pseudoEcho],actionEqual)&& !detected)
			{
				if (summEcho<= distArray[pseudoEcho]+_equalMargin && summEcho>= distArray[pseudoEcho]-_equalMargin) 
				{
					retryCompareCount[pseudoEcho]++;
					if (retryCompareCount[pseudoEcho]>=retryCompareValue)
					{
						detected=true;	
					}
				}
				else{
					retryCompareCount[pseudoEcho]=0x00;
				}
			}				
	
			if (bitRead(actionArray[pseudoEcho],actionNotEqual)&& !detected)
			{
				if ((summEcho>= distArray[pseudoEcho]+_equalMargin || summEcho<= distArray[pseudoEcho]-_equalMargin)) 
				{
					retryCompareCount[pseudoEcho]++;
					if (retryCompareCount[pseudoEcho]>=retryCompareValue)
					{
						detected=true;
					}
				}
				else{
					retryCompareCount[pseudoEcho]=0x00;
				}
			}	

			if (detected)
			{
					lastAlertEchoNumber=pseudoEcho;
					lastEchoMicro[pseudoEcho]=summEcho*echoCm;
					SetInterruptPIN();
			}			
		}
	
	}
}
void SetInterruptPIN()
{
#if defined(debugOn)
	Serial.println("I");
#endif
	if (_pinInterrupt!=0)
		{
		pinMode(_pinInterrupt,OUTPUT);		
		digitalWrite(_pinInterrupt,1);
		delay(1);
		digitalWrite(_pinInterrupt,0);
		}
}
uint8_t EchoObstacleDetection::GetAlertEchoNumber()
{
	return lastAlertEchoNumber;
}
unsigned int EchoObstacleDetection::GetEchoThreshold(uint8_t echoNumber)
{
	return distArray[echoNumber];
}