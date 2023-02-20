#include <RBDdimmer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "secret.h"
#include "pageContent.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

#define outputPin  D2
#define zerocross  D1


ESP8266WebServer server(80);
dimmerLamp dimmer(outputPin, zerocross);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
unsigned long previousTime = millis();
unsigned long currentTime;
bool alarm_set = false; 

void webpage()
{
  int dim_level = dimmer.getPower();
  
  for (uint8_t i = 0; i < server.args(); i++) {
    if(server.argName(i)== "light"){
      int light_percentage = server.arg(i).toInt();
      if((light_percentage < 100) && (light_percentage > 10)){
          dimmer.setPower(light_percentage);
          dim_level = light_percentage;
        }
      }
  }
  server.send(200,"text/html", webpageCode(dim_level));
}


void light_off_function(){
  dimmer.setPower(10);
  server.send(200,"text/plain", "Light turned off");
  }

void set_light_function(){
      int light_percentage = server.arg("light").toInt();
      if((light_percentage <= 100) && (light_percentage >= 10)){
          dimmer.setPower(light_percentage);
        }
      server.send(200,"text/plain", "Light set.");
}

void set_time_function(){

  if ((server.arg("hour") != "") && (server.arg("minute") != "") && (server.arg("offset") != "")){
    int hour = server.arg("hour").toInt();
    int minute = server.arg("minute").toInt();
    int offset = server.arg("offset").toInt();
    Serial.print(hour);
    Serial.print(" : ");
    Serial.print(minute);
    Serial.print(". Offset : ");
    Serial.println(offset);
    alarm_set = true; 
  }  
  server.send(200,"text/plain", "Time set.");
}

void unset_time_function(){
  alarm_set = false;   
  server.send(200,"text/plain", "Time unset.");
  }



void setup() {
  Serial.begin(9600); 
  dimmer.begin(NORMAL_MODE, ON);    
  dimmer.setPower(11);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", webpage);
  server.on("/set_light", set_light_function);
  server.on("/light_off", light_off_function);
  server.on("/set_alarm", set_time_function);
  server.on("/unset_alarm", unset_time_function);

  server.begin();


  timeClient.begin();
  timeClient.setTimeOffset(3600);
}

void loop() {
  currentTime = millis();
  server.handleClient();
  
//  if (Serial.available()){
//    int buf = Serial.parseInt();
//    if ((buf >= 10) && (buf <= 100)){
//      Serial.print("ustawiona wartosc ");
//      Serial.println(buf);
//      delay(200);
//      dimmer.setPower(buf);
//    }
//  }
  if(alarm_set){
    if(currentTime-previousTime >= 3000){
      timeClient.update();
      int currentHour = timeClient.getHours();
      int currentMinute = timeClient.getMinutes();
//      Serial.println("Hour: ", currentHour);
//      Serial.println("Minutes: ", currentMinute);
      Serial.println("Alarm set");
      previousTime = currentTime;
    }
  }
}
