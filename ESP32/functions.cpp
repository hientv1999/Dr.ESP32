#include "functions.hpp"
#include <string.h> 
//-----------------------------------------------------------------------------------------------------------------
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "PubSubClient.h"
// Define NTP Client to get time                                  //These codes are for getting real time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;
extern int reset;
extern const char* mqtt_server;
extern const char* BLEAddress_topic;
extern const char* mqtt_username;
extern const char* mqtt_password;
extern const char* clientID;
extern WiFiClient wifiClient;
extern PubSubClient client;

void initialize_timer()
{
    timeClient.begin();                                             // Set offset time in seconds to adjust for your timezone, in this case GMT -7 for Victoria, BC
    timeClient.setTimeOffset(-28800);
}

String get_contact_time(){
    timeClient.forceUpdate();
    String formattedDate = timeClient.getFormattedTime();    //YYYY-MM-DDTHH:MM:SSZ
    int splitT = formattedDate.indexOf("T");
    String dayStamp = formattedDate.substring(0, splitT);    // Extract date
    String timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1); // Extract time
    return dayStamp + " " + timeStamp; 
}

BLYNK_WRITE(V2)                                     //reset when needed according to virtual button in Blynk app
{
  reset = param.asInt();
}



//--------------------------------------------------------------------------ESP WORK-----------------------------------------------------------------------------------------------------------


#include <esp_log.h>                                      //library to use ESP32 in Arduino IDE
#include "esp32-hal-cpu.h"
#include "sdkconfig.h"
#include "driver/adc.h"
#include <string.h>                                         //library for string type
#include <EEPROM.h>     
void restartESP(int &reset)
{
  if (reset == 1)
  {
    Blynk.virtualWrite(V1, "clr");                        //Clear the data on the server if reset the ESP32
    EEPROM.write(3960, 0);                                //Clear memory for number of EEPROM already used
    EEPROM.commit();  
    EEPROM.write(3961, 0);
    EEPROM.commit();
    EEPROM.write(3962,0);                                 //Clear memory for number of EEPROM already uploaded
    EEPROM.commit();
    EEPROM.write(3963,0);
    EEPROM.commit();
    ESP.restart();                                        //Restart the ESP32
    reset =0;
  }
}    
                  
#include <ctype.h>

void Write_Unsigned_Int_Into_EEPROM(int address, unsigned int value){
    EEPROM.write(address, value >> 8);
    EEPROM.write(address + 1, value & 0xFF);
}

void Write_Time_Into_EEPROM(int address){
    timeClient.forceUpdate();
    String formattedDate = timeClient.getFormattedTime();    //YYYY-MM-DDTHH:MM:SSZ
    unsigned int Year = formattedDate.substring(0, 3).toInt();
    unsigned int Month = formattedDate.substring(5, 6).toInt();
    unsigned int Day = formattedDate.substring(8, 9).toInt();
    unsigned int Hour = formattedDate.substring(11, 12).toInt();
    unsigned int Minute = formattedDate.substring(14, 15).toInt();
    unsigned int Second = formattedDate.substring(17, 18).toInt();
    Write_Unsigned_Int_Into_EEPROM(address, Year);
    EEPROM.write(address + 2, Month);
    EEPROM.commit();
    EEPROM.write(address + 3, Day);
    EEPROM.commit();
    EEPROM.write(address + 4, Hour);
    EEPROM.commit();
    EEPROM.write(address + 5, Minute);
    EEPROM.commit();
    EEPROM.write(address + 6, Second);
    EEPROM.commit();
}

unsigned int Read_Unsigned_Int_From_EEPROM(int address)
{
    return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}

String Read_Time_From_EEPROM(int address){
    unsigned int Year = Read_Unsigned_Int_From_EEPROM(address);
    unsigned int Month = EEPROM.read(address + 2);
    unsigned int Day = EEPROM.read(address + 3);
    unsigned int Hour = EEPROM.read(address + 4);
    unsigned int Minute = EEPROM.read(address + 5);
    unsigned int Second = EEPROM.read(address +6);
    return String(Year) + "-" + String(Month) + "-" + String(Day) + " " + String(Hour) + ":" + String(Minute) + ":" + String(Second);
} 


//------------------------------------------------------------------------------DISPLAY WORK--------------------------------------------------------------------------------------------------


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMonoBold9pt7b.h>
extern int button1;
extern Adafruit_SSD1306 display;
void print_instruction(const char text[80])
{
  while (!digitalRead(button1))
  {
     display.clearDisplay();
     display.setTextColor(WHITE);
     display.setTextSize(1);
     display.setCursor(0, 0);
     display.println(text);
     display.println("Press 1 to proceed");
     display.display();
  }
  delay(500);
}

void print_info(const char text[80], bool clear, int delay_time){
    if (clear){
        display.clearDisplay();
        display.setCursor(0, 0);
    }
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.print(text); 
    display.display();
    delay(delay_time);
}


//---------------------------------------------------------------------------START WORK------------------------------------------------------------------------------------------------------


extern char auth[32], ssid[32], pass[63];
void welcome_screen(){
    delay(1000);                                     //allow display has enough time to initialize before writing text                       
    display.clearDisplay();                          //clear the display
    display.setTextColor(WHITE);                     //display welcome screen
    display.setCursor(0, 5);                         //everytime the esp32 is turned on
    display.setFont(&FreeMonoBold9pt7b);             //
    display.println(" Welcome to");                  //
    display.println("  Dr.ESP32");                   //
    display.display();                               //
    delay(1000);                                     //
    display.clearDisplay();                          //
    display.setFont();                               //
    display.setTextSize(1);                          //
    display.setCursor(0, 5);                         //
    display.println("  Together we can do");         //
    display.setCursor(55,17);                        //
    display.setTextSize(2);                          //
    display.write(3);                                //
    display.display();                               // 
    delay(750);
}
enum St
    {
        WORKING, 
        UPLOAD_ONLY, 
        FULLMEM
    };
extern St state;  
void retrieve_state(){

                                                
    int last_state = EEPROM.read(3964);                         //state password for first time use
    Serial.print("Last state is: ");
    Serial.println(last_state);
        
    switch (last_state){                                        //retrieve the last state in case not the first time use
        case 1:
        print_info("Before initial Blynk", true, 1000);
        Blynk.begin(auth, ssid, pass);
        print_info("After initial Blynk", true, 2000);
        state = WORKING;
        break;

        case 2:
        Blynk.begin(auth, ssid, pass);
        state = UPLOAD_ONLY;
        break;

        case 3:
        Blynk.begin(auth, ssid, pass);
        state = FULLMEM;
        break; 
    }
}


//-----------------------------------------------------------------------------------WIFI WORK-------------------------------------------------------------------------------------------


bool reconnectWiFi(){
    WiFi.begin(ssid, pass);
    Serial.println("Connecting...");
    long long int start_time = millis();
    while (WiFi.status() != WL_CONNECTED && millis()-start_time < 10000){}
    if (WiFi.status() == WL_CONNECTED){
        Serial.print("Connected to WiFi network with IP Address: ");
        Serial.println(WiFi.localIP());
        return true;
    }
    Serial.println("Time out (over 10 seconds).");
    return false;
}

//----------------------------------------------------------------------------BLUETOOTH WORK------------------------------------------------------------------------------------------------
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "BLEAddress.h"                                   //libraries for BLE
#include "esp_bt_main.h"
#include "esp_bt_device.h"       

void printDeviceAddress() 
{
  const uint8_t* point = esp_bt_dev_get_address();
  Serial.print("BLE address of this device:");
  for (int i = 0; i < 6; i++) {
    char str[3];
    sprintf(str, "%02X", (int)point[i]);
    Serial.print(str);
    if (i < 5){
      Serial.print(":");
    }
  }
  Serial.println(" ");
}

#define SERVICE_UUID            "b596e525-c3c6-45b3-a8bb-b8cddcc62a61"              //channel to send
#define CHARACTERISTIC_UUID     "f364c9cb-2d2c-4eb6-92d3-28db399a09ee" 
int ConnectToESP32(BLEAdvertisedDevice device){
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    

    // Connect to the remove BLE Server.
    pClient->connect(&device);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)

    // Obtain a reference to the service we are after in the remote BLE server.
    
    if (pClient->getService(SERVICE_UUID) == nullptr) {
        pClient->disconnect();
        return 0;
    }
    Serial.println(" - Found our service");
    BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
    // Obtain a reference to the characteristic in the service of the remote BLE server.
    BLERemoteCharacteristic* pRemoteCharacteristic = pRemoteService->getCharacteristic(CHARACTERISTIC_UUID);
    if (pRemoteCharacteristic == nullptr) {
        pClient->disconnect();
        return 0;
    }
    Serial.println(" - Found our characteristic");
    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
        std::string value = pRemoteCharacteristic->readValue();
        Serial.print("The characteristic value was: ");
        Serial.println(value.c_str());
        return atoi(value.c_str());
        pClient->connect(&device);      //to disconnect
    }
    return 0;
}


//----------------------------------------------------------------------------------LAMP WORK------------------------------------------------------------------------------------------------------------
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
String apiKeyValue = "tPmAT5Ab3j7F9";
const char* serverName = "http://10.0.0.173/post-esp-data.php";

bool sendToLAMPServer(unsigned int FromDevice, unsigned int ToDevice, String contact_time){
    if(WiFi.status()== WL_CONNECTED){
        HTTPClient http;
    
        // Your Domain name with URL path or IP address with path
        http.begin(serverName);
    
        // Specify content-type header
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
        // Prepare your HTTP POST request data
        String httpRequestData = "api_key=" + apiKeyValue + "&FromDevice=" + String(FromDevice)
                                + "&ToDevice=" + String(ToDevice) + "&contact_time=" + contact_time;
        Serial.print("httpRequestData: ");
        Serial.println(httpRequestData);
    
        // You can comment the httpRequestData variable above
        // then, use the httpRequestData variable below (for testing purposes without the BME280 sensor)
        //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&location=Office&value1=24.75&value2=49.54&value3=1005.14";

        // Send HTTP POST request
        int httpResponseCode = http.POST(httpRequestData);
     
        // If you need an HTTP request with a content type: text/plain
        //http.addHeader("Content-Type", "text/plain");
        //int httpResponseCode = http.POST("Hello, World!");
    
        // If you need an HTTP request with a content type: application/json, use the following:
        //http.addHeader("Content-Type", "application/json");
        //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");
        
        if (httpResponseCode>0) {
            Serial.print("HTTP Response code: ");
             Serial.println(httpResponseCode);
            return true;
        } else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
            return false;
        }
        // Free resources
        http.end();
    } else {
        Serial.println("WiFi Disconnected");
        return false;
    }
}
