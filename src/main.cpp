/* @toffanetto

  ESP 01 Smart Switch

  Author: Gabriel Toffanetto
  Date: Oct 2021

  Single light smart control with IoT and phisical switch.

  -> Using Sinric Pro Server
  -> Alexa + Google Home

*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h> 

#include "SinricPro.h"
#include "SinricProSwitch.h"

#define RELAY 0 
#define SWITCH 2

#define APP_KEY           "YOUR-APP-KEY"      
#define APP_SECRET        "YOUR-APP-SECRET"   
#define SWITCH_ID         "YOUR-DEVICE-ID"   
#define SWITCH_TIME 500

bool switch_state = 0;
bool light_state = LOW;

long long int time_delay = 0;

void relaySwitch(){
  light_state = !light_state;
  digitalWrite(RELAY, !light_state);
  SinricProSwitch& mySwitch = SinricPro[SWITCH_ID];   
  mySwitch.sendPowerStateEvent(light_state);
  Serial.print("RELAY ");
  Serial.println(light_state);
}

void relaySwitch(bool action){
  light_state = action;
  digitalWrite(RELAY, !light_state);
  SinricProSwitch& mySwitch = SinricPro[SWITCH_ID];  
  mySwitch.sendPowerStateEvent(light_state);
  Serial.print("RELAY ");
  Serial.println(light_state);
}
 
bool onPowerState(const String &deviceId, bool &state){
  digitalWrite(RELAY, !state);            
  light_state = state;
  Serial.print("Estado do relé alterado por Sinric para: ");
  Serial.println(state);
  return true;                                
}


void setup(){

  Serial.begin(9600);

  Serial.println("TOFFANETTO SMART SWITCH -- V0");
 
  pinMode(RELAY,OUTPUT); 
  pinMode(SWITCH,INPUT);
  
  switch_state = digitalRead(SWITCH);
  
  relaySwitch(LOW);

  WiFiManager wifiManager;

  wifiManager.autoConnect("SmartRelayESP", "toffanetto");

  Serial.println("Iniciando SinricPro");
  SinricProSwitch& mySwitch = SinricPro[SWITCH_ID];   
  mySwitch.onPowerState(onPowerState);                
  SinricPro.begin(APP_KEY, APP_SECRET);               

}
 
void loop(){

  SinricPro.handle();                         
  
  if(millis()-time_delay > SWITCH_TIME){
    bool switch_state_now = digitalRead(SWITCH);
    if(switch_state != switch_state_now){
      relaySwitch();
    }
    switch_state=switch_state_now;
    time_delay = millis();
  }

}
