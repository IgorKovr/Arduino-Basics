/// by Igor KIO
//  This sketch reads all input SMS messages
//  Write them to Log
//  turns LED ON  - on "123" message
//  turns LED OFF - on "321" message
//
//  you can use it for any interaction via sms with your arduino
//  Cheers!

#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
SMSGSM sms;
char number[30];
char message[180];
char pos;
char *p;
// Pin 13 has an LED connected on most Arduino boards.messagemessagemessagemessage
// give it a name:
int led = 13;
 
void setup()
{
 Serial.begin(9600);
 if (gsm.begin(2400))
   Serial.println("\nstatus=READY");
 else 
   Serial.println("\nstatus=IDLE");
     // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
};


void loop() // Main thread
{
 pos=sms.IsSMSPresent(SMS_ALL);   
 if (!pos)
    Serial.println("No messages");
 else {
   Serial.print("Message found!\nOn position:");
   Serial.println((int)pos);
   Serial.println("Message text:");
   Serial.println(message);
   message[0]='\0';
   sms.GetSMS((int)pos,number,message,180);
   p=strstr(message,"123"); // 123 to turn led on
     if(p){
       Serial.println("LED ON");
       digitalWrite(led,HIGH);
       delay(1000);
     }
     else{
       p=strstr(message,"321");
       if(p){
         Serial.println("LED OFF"); // 321 to turn led off
         digitalWrite(led,LOW);
         delay(1000);
       }
     }
   sms.DeleteSMS((int)pos); // Deleting founded message
 }
 delay(5000);
};
