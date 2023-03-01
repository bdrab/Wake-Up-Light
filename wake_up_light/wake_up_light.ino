#include <RBDdimmer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "secret.h"
#include "page_content.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

#define outputPin  D2
#define zerocross  D1

ESP8266WebServer server(80);
dimmerLamp dimmer(outputPin, zerocross);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
unsigned long previous_time = millis();
unsigned long current_time;
unsigned long previous_time_brightening;
bool alarm_set = false; 
bool brightening_start = false; 

int alarm_hour;
int alarm_minute;
int alarm_offset;

int start_brightening_hour;
int start_brightening_minute;
int brightening_steps;

void main_page_function(){
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
    alarm_hour = server.arg("hour").toInt();
    alarm_minute = server.arg("minute").toInt();
    alarm_offset = server.arg("offset").toInt();
    alarm_set = true;
    brightening_steps = int(alarm_offset*60*1000/90);
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
  
  Serial.print("IP address of the server: ");
  Serial.println(WiFi.localIP());
  
  server.on("/", main_page_function);
  server.on("/set_light", set_light_function);
  server.on("/light_off", light_off_function);
  server.on("/set_alarm", set_time_function);
  server.on("/unset_alarm", unset_time_function);

  server.begin();

  timeClient.begin();
  timeClient.setTimeOffset(3600);
}

void loop() {
  
  current_time = millis();
  server.handleClient();

// code to manualy set light level using serial
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
    // brightening the lamp 
    if(brightening_start){
      if(current_time-previous_time_brightening >= brightening_steps){
        int level = dimmer.getPower() + 1;
        if((level <= 100) && (level >= 10)){
          dimmer.setPower(level);
        }
      previous_time_brightening = millis(); 
      }
    }
    // checking if it's the correct time to start brightening the lamp
    if(current_time-previous_time >= 3000){
      timeClient.update();
      int current_hour = timeClient.getHours();
      int current_minute = timeClient.getMinutes();
      if(alarm_minute - alarm_offset < 0){
        start_brightening_minute = (alarm_minute - alarm_offset) + 60;
        if(alarm_hour - 1 < 0){
          start_brightening_hour = 23;
          }else{
            start_brightening_hour = alarm_hour -1;
            }
        }else{
          start_brightening_minute = alarm_minute - alarm_offset;
          start_brightening_hour = alarm_hour;
        }
      if((current_hour>=start_brightening_hour) &&(current_minute>=start_brightening_minute)){
        brightening_start = true;
        }
      if((current_hour==alarm_hour) &&(current_minute==alarm_minute)){
        brightening_start = false;
        alarm_set = false;
      }
            previous_time = current_time;
    }
  }
}
