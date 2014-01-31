#include "SIM900.h"
 
#include <SoftwareSerial.h>
 
#include "sms.h"
 
SMSGSM sms;
 
char number[]="0932287117";
 
int value=0;
 
int value_old=0;
 
char message[40];
 
char value_str[4];
 
int pin=A0;
 
void setup()
 
{
 
 Serial.begin(9600);
 
 if (gsm.begin(2400))
 
   Serial.println("\nstatus=READY");
 
 else Serial.println("\nstatus=IDLE");
 
 value=analogRead(pin);
 
 value_old=analogRead(pin);
 
};
 
void loop()
 
{
 
 value=analogRead(pin);
 
 Serial.println(value);
 
 if(value<50&&value_old>=50){
 
   message[0]='\0';
 
   strcat(message,"ALLARME LOW, VAL=");
 
   itoa(value,value_str,10);
 
   strcat(message,value_str);
 
   sms.SendSMS(number,message);
 
 }
 
 value_old=value;
 
 delay(1000);
 
};
