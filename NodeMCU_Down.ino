#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <WiFiClient.h>
#include <String.h>
#define ACCEPT_TYPE "text/csv" 
WiFiClient client;

const char ssid[] = "Biprajit";
const char password[] = "111100000";
const char put_Thing[] = "";
const char get_Property[] = "";
const char host[] = "dcs.glaitm.org"; 
const int httpsPort = 7080;
const char appKey[]= "1f7c09e1-6303-40ad-ab9e-2a738f20d15b";

const char Thing1[] = "Heart_Rate_Sensor"; //Pulse
const char Property1[] = "HeartRate";

const char Thing2[] = "Temperature_Sensor"; // LM35 Body Temperature
const char Property2[] = "Temperature";

const char Thing5[] = "Position_Sensor"; // MPU6050
const char Property5[] = "Gyroscope";



char e[6];

void setup() 
{
  Wire.begin(4,5);  
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
{
    delay(500);
    Serial.print(".");
 }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 

}


void loop() 
{
  Wire.requestFrom(8, 64);
  char array[64];
  int i=0;
  while (0 < Wire.available())
  {
    char c = Wire.read();
    array[i] = c;
    Serial.print(array[i]);
    i++;
    
  }
  Serial.println("");
  char *strings[8]; 
  char *ptr = NULL;   
  byte index = 0;
  ptr = strtok(array, ",");
  while (ptr != NULL)
  {
    strings[index] = ptr;
    index++;
    ptr = strtok(NULL, ",");
  }
  Serial.println(strings[1]);   //Heart Rate
  Serial.println(strings[2]);   //Temperature
  
  
  Serial.println(strings[5]);   //Position
  
 
  Put(Thing1,Property1,strings[1]);
  Put(Thing2,Property2,strings[2]);
 
  
  Put(Thing5,Property5,strings[5]);
  
 
  String res = Get(put_Thing,get_Property);
 
 // Serial.print(res);
  Serial.println(res[9]);
  
  Wire.beginTransmission(8);
  Wire.write(res[9]);
  Wire.endTransmission(); 

  delay(1000);
}

void Put(String ThingName, String ThingProperty, String Value)
  {

    Serial.println(host);
  if (!client.connect(host, httpsPort))
 {
    Serial.println("connection failed");
    return;
  } else

 {
    Serial.println("Connected to ThingWorx.");
  }
  String url = "/Thingworx/Things/" + ThingName + "/Properties/" + ThingProperty; 
  Serial.print("requesting URL: ");
  Serial.println(url);

  String strPUTReqVal = "{\"" + ThingProperty + "\":\"" + Value + "\"}";  
  Serial.print("PUT Value: ");
  Serial.println(strPUTReqVal);

  client.print(String("PUT ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "appKey:"+ appKey + "\r\n" +
               "x-thingworx-session: false" + "\r\n" +
               "Accept: application/json" + "\r\n" +
               "Connection: close" + "\r\n" +
               "Content-Type: application/json" + "\r\n" +
               "Content-Length: " + String(strPUTReqVal.length()) + "\r\n\r\n" +
               strPUTReqVal + "\r\n\r\n");   

  while (client.connected()) 
{
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  client.stop();
}

String Get(String get_Thing, String get_Property)
{              
        HTTPClient http;
        int httpCode = -1;
        String fullRequestURL = "http://" + String(host) +":"+ String(httpsPort)+ "/Thingworx/Things/" 
                                + get_Thing + "/Properties/" + get_Property + "?appKey=" + appKey;
                                
        Serial.println(fullRequestURL);
        http.begin(client, fullRequestURL);
        http.addHeader("Accept",ACCEPT_TYPE,false,false);        
        httpCode = http.GET();
        Serial.println(httpCode);
        
        String responses;
        if(httpCode > 0)        
        {             
           responses= http.getString(); 
           Serial.println("Connected...");
             //Serial.println(responses);      
             //Serial.print(responses[9]);        
        }
        else         
        {
            Serial.printf("[httpGetPropertry] failed, error: %s\n\n", http.errorToString(httpCode).c_str());
            Serial.println("Not Connected...");
       }
        http.end();
        return responses;        
}
