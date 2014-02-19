/// by Igor KIO
//  This sketch reads all input SMS messages
//  Write them to Log
//  turns LED ON  - on "123" message
//  turns LED OFF - on "321" message
//
//  you can use it for any interaction via sms with your arduino
//  Cheers!

#include <stdio.h>

#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
#include "call.h"

// setup  DHT server
#include "DHT.h"
#define DHTPIN A0     // what pin we're connected to
#define DHTTYPE 11   // DHT 11 
DHT dht(DHTPIN, DHTTYPE);

// Define sms to responde to
#define SMS_ON   "123"
#define SMS_OFF  "321"
#define SMS_TEMP "?"
#define SMS_BALANCE "111"
#define CHECK_BALANCE "*110*10*03#"

char sendNumber[]="0932287117";
float temp;
float *humidity;

SMSGSM sms;
CallGSM call;
char number[30];
char messageRead[180];
//char messageSend[40];
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
 float h;
 humidity = &h;
 temp = readSensor(humidity);
// Serial.print("Temperature is: +");
// Serial.println(temp);
// Serial.print("Humidity is: ");
// Serial.println(*humidity);
// Serial.println("\n\n");
  
 pos=sms.IsSMSPresent(SMS_ALL);
 if (!pos){
//    Serial.println("No messages\n\n");
  }
 else {
   Serial.print("Message found!\nOn position:");
   Serial.println((int)pos);
   Serial.println("Message text:");
   Serial.println(messageRead);
   messageRead[0]='\0';
   sms.GetSMS((int)pos,number,messageRead,180);
   checkSendSMSWithTemp(messageRead);
   checkSwitchLED(messageRead);
   checkBALANCE(messageRead);
   sms.DeleteSMS((int)pos); // Deleting founded message
 }
 delay(500);
};

void checkSendSMSWithTemp(char *messageToCheck){
     p=strstr(messageToCheck, SMS_TEMP);
     if(p){
      char messageToSend[120 + 1];  //buffer used to format a line (+1 is for trailing 0)
      int t = temp;
      int h = *humidity;
      sprintf(messageToSend,"Hi, I'm ok!\n Temperature is: +%d*C\nHumidity: %d%%",t, h);   
      Serial.println("Sending Message:");
      Serial.println(messageToSend);
//       char message[ ] = "Temp:";
       sms.SendSMS(number,messageToSend);
     }
}

void checkBALANCE(char *messageToCheck){
     p=strstr(messageToCheck, SMS_BALANCE);
     if(p){
       if(call.CallStatus()!= CALL_ACTIVE_VOICE){
         Serial.println("Checking Balance!");
         call.Call(number);
       }
       else {
        Serial.println("Can't check balance, try later"); 
        sms.SendSMS(number,"Can't check balance, try later");
       }
     }
      p=strstr(messageToCheck, "MTS");
     if(p){
       Serial.println("Balance is:");
       Serial.print(messageToCheck);
     }
}


void checkSwitchLED(char *messageToCheck){
     p=strstr(messageToCheck, SMS_ON); //  turn led on
     if(p){
       Serial.println("LED ON");
       digitalWrite(led,HIGH);
       delay(1000);
     }
     else{
       p=strstr(messageRead,SMS_OFF);
       if(p){
         Serial.println("LED OFF"); // turn led off
         digitalWrite(led,LOW);
         delay(1000);
       }
     }
}

float readSensor(float *humid){
   // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  temp = t;
//  humidity = h;
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\n");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
  } 
  *humid = h;
  return t; // Warning float to int cast
}
