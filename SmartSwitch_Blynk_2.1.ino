#define BLYNK_TEMPLATE_ID "Your Template ID"
#define BLYNK_DEVICE_NAME "Your Device Name"
#define BLYNK_AUTH_TOKEN "Your Auth Token"


#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <BlynkSimpleEsp8266.h>

uint8_t relayPin = 0; //GPIO0
uint8_t indicatorPin = 1; //GPIO1 /TX
uint8_t switchExternal = 2; //GPIO2
uint8_t resetPinSSID = 3; //GPIO3 /RX

int resetPin = 1; 
int wifiMode = 0;
int toggleSwitch = 1;
int pushSwitch = 0;

int switch_status; 
int last_switch_status = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

unsigned long buttonTimer = 0;
unsigned long longPressTime = 500;

boolean buttonActive = false;
boolean longPressActive = false;

WiFiManager wifiManager;
      
BlynkTimer timer;


void with_int()
{
  int readingSwitch = digitalRead(switchExternal);
  if(readingSwitch != last_switch_status)
  {
    lastDebounceTime = millis();
  }
  if((millis() - lastDebounceTime) > debounceDelay)
  {
    if(readingSwitch != switch_status)
    {
      switch_status = readingSwitch;
      if(switch_status == HIGH)
      {
        pushSwitch++;
        if(pushSwitch>1)
        {
          pushSwitch=0;
        }
        if(pushSwitch==0)
        {
          toggleSwitch=0;
          
        }
        if(pushSwitch==1)
        {
          toggleSwitch=1;
        }
      }
    }
  }
  digitalWrite(relayPin,toggleSwitch);
  Blynk.virtualWrite(V3,toggleSwitch);
  last_switch_status = readingSwitch;   
}


void without_int()
{
  int readingSwitch = digitalRead(switchExternal);
  if(readingSwitch != last_switch_status)
  {
    lastDebounceTime = millis();
  }
  if((millis() - lastDebounceTime) > debounceDelay)
  {
    if(readingSwitch != switch_status)
    {
      switch_status = readingSwitch;
      if(switch_status == HIGH)
      {
        pushSwitch++;
        if(pushSwitch>1)
        {
          pushSwitch=0;
        }
        if(pushSwitch==0)
        {
          toggleSwitch=0;
          
        }
        if(pushSwitch==1)
        {
          toggleSwitch=1;
        }
      }
    }
  }
  digitalWrite(relayPin,toggleSwitch);

  last_switch_status = readingSwitch;  
}


BLYNK_CONNECTED() 
{
  Blynk.syncAll();
}

BLYNK_WRITE(V3)
{
  toggleSwitch = param.asInt();
  digitalWrite(relayPin,toggleSwitch);
}

void checkBlynkStatus()
{
 bool isconnected = Blynk.connected();
  if (isconnected == false) 
  {
    wifiMode = 1;
  }
  if (isconnected == true) 
  {
    wifiMode = 0;
  }
}


void longButtonPress()
{
   resetPin = digitalRead(resetPinSSID);
   if(resetPin == LOW)
   {
     if(buttonActive == false)
     {
      buttonActive = true;
      buttonTimer = millis();
     }
     if ((millis() - buttonTimer > longPressTime) && (longPressActive == false))
     {
      longPressActive = true;
      wifiManager.resetSettings();  
      ESP.restart(); 
    }
   }

   else
   {
      buttonActive = false;
      longPressActive = false;
   }
}

void setup()
{
  pinMode(relayPin,OUTPUT);
  digitalWrite(relayPin, HIGH);
  
  pinMode(resetPinSSID,INPUT);   
  digitalWrite(resetPinSSID,LOW);

  pinMode(indicatorPin,OUTPUT);
  digitalWrite(indicatorPin, LOW);

  pinMode(switchExternal, INPUT_PULLUP);

  wifiManager.autoConnect("ravenusa");
  WiFiManagerParameter custom_blynk_token("Blynk", "blynk_token", BLYNK_AUTH_TOKEN, 34);
  wifiManager.addParameter(&custom_blynk_token);
  wifiManager.autoConnect("Blynk");
  Blynk.config(custom_blynk_token.getValue());
  Blynk.config(BLYNK_AUTH_TOKEN);
  
  timer.setInterval(3000L, checkBlynkStatus);
}


void loop()
{
  longButtonPress();
  if (WiFi.status() == WL_CONNECTED)
  {
    Blynk.run();
    digitalWrite(indicatorPin,HIGH);
  }
  timer.run();
 
  if (wifiMode == 0)
    with_int();
  else
    without_int();

}
