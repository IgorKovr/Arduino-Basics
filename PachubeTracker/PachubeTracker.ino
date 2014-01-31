/*
by Igor K.

*/

#include <SoftwareSerial.h>
#include "SIM900.h"
#include "inetGSM.h"

// setup  DHT server
#include "DHT.h"
#define DHTPIN A0     // what pin we're connected to
#define DHTTYPE 11   // DHT 11 
DHT dht(DHTPIN, DHTTYPE);

// Pachube login information
#define APIKEY         "29XwZBudpy3U8NUg8Vs8IE91VjdUGTMd3GduzC6HzF1Msfcq"  // replace your pachube api key here
#define FEEDID         1214607154                     // replace your feed ID
#define USERAGENT      "theHead"              // user agent is the project name

// PIN Number
#define PINNUMBER ""

//Delay in seconds
#define DELAY 10

// APN data
#define GPRS_APN       "internet" // replace your GPRS APN
#define GPRS_LOGIN     ""    // replace with your GPRS login
#define GPRS_PASSWORD  "" // replace with your GPRS password

InetGSM inet;

char server[] = "https://api.xively.com/";       // name address for Pachube API

// for GSM connecton 
char msg[50];
int numdata;
char inSerial[50];
int i=0;
boolean started=false;


unsigned long lastConnectionTime = 0;           // last time you connected to the server, in milliseconds
boolean lastConnected = false;                  // state of the connection last time through the main loop
const unsigned long postingInterval = 10*1000;  // delay between updates to Pachube.com

void setup() 
{
  //Serial connection.
  Serial.begin(9600);
  Serial.println("GSM Shield testing.");
  //Start configuration of shield with baudrate.
  //For http uses is raccomanded to use 4800 or slower.
  if (gsm.begin(2400)){
    Serial.println("\nstatus=READY");
    started=true;  
  }
  else Serial.println("\nstatus=IDLE");
  
  if(started){
    //GPRS attach, put in order APN, username and password.
    //If no needed auth let them blank.
    if (inet.attachGPRS(GPRS_APN, "", ""))
      Serial.println("status=ATTACHED");
    else Serial.println("status=ERROR");
    delay(1000);
    
    //Read IP address.
    gsm.SimpleWriteln("AT+CIFSR");
    delay(5000);
    int i=0;
    while(i<20){
      gsm.SimpleRead();
      i++;
    }

    //TCP Server. Start the socket connection
    //as server on the assigned port.
    Serial.println(msg);
    delay(5000);
    if (inet.connectTCPServer(80))
      Serial.println("status=TCPSERVERWAIT");
    else Serial.println("ERROR in Server");
  }
};

int readSensor(){
   // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
  } 
  
  return t; // Warning float to int cast
}

void loop()
{
  int i = readSensor();
  sendData(i);
}

// this method makes a HTTP connection to the server
void sendData(int thisData)
{
    Serial.println("sending...");
    
    // send the HTTP PUT request:gsm.SimpleWrite
    gsm.SimpleWrite("PUT /v2/feeds/");
    gsm.SimpleWrite(FEEDID);
    gsm.SimpleWriteln(".csv HTTP/1.1");
    gsm.SimpleWriteln("Host: api.pachube.com");
    gsm.SimpleWrite("X-ApiKey: ");
    gsm.SimpleWriteln(APIKEY);
    gsm.SimpleWrite("User-Agent: ");
    gsm.SimpleWrite(USERAGENT);
    gsm.SimpleWrite("Content-Length: ");

    // calculate the length of the sensor reading in bytes:
    // 8 bytes for "sensor1," + number of digits of the data:
    int thisLength = 8 + getLength(thisData);
    gsm.SimpleWriteln(thisLength);

    // last pieces of the HTTP PUT request
    gsm.SimpleWriteln("Content-Type: text/csv");
    gsm.SimpleWriteln("Connection: close");
    gsm.SimpleWriteln("");
    
    // here's the actual content of the PUT reques
     gsm.SimpleWrite("sensor1,");
     gsm.SimpleWriteln(thisData);
    
    delay(DELAY*1000);
}

/*
  This method calculates the number of digits in the
  sensor reading.  Since each digit of the ASCII decimal
  representation is a byte, the number of digits equals
  the number of bytes.
*/
int getLength(int someValue)
{
  // there's at least one byte:
  int digits = 1;
  
  // continually divide the value by ten, 
  // adding one to the digit count for each
  // time you divide, until you're at 0:
  int dividend = someValue /10;
  while (dividend > 0)
  {
    dividend = dividend /10;
    digits++;
  }
  
  // return the number of digits:
  return digits;
}
