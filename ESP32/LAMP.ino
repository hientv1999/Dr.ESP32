unsigned int DeviceOrder = 1;
#define ORDER "1"
#include "functions.hpp"
#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>
int button1{25}, button2{26}, button3{27}, button4{14};
enum St
    {
        WORKING, 
        UPLOAD_ONLY, 
        FULLMEM
    };
St state;
//-----------------------------------------------------------------------------------------------------------------------------------------
//-------------the section below contains the libraries, global variables, data and functions related to data sending----------------
//-----------------------------------------------------------------------------------------------------------------------------------------
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>        
#define SERVICE_UUID            "b596e525-c3c6-45b3-a8bb-b8cddcc62a61"              //channel to send
static BLEUUID serviceUUID("b596e525-c3c6-45b3-a8bb-b8cddcc62a61");
#define CHARACTERISTIC_UUID     "f364c9cb-2d2c-4eb6-92d3-28db399a09ee"  
char auth[] = "wZP5MpIGyFyIpAYvfOFUMLW6YZExU_WO";                
char ssid[] = "wi yeong";                                         
char pass[] = "Ironman@";
//-----------------------------------------------------------------------------------------------------------------------------------------
//----the section below contains the libraries, global variables, data and functions related to BLE advertising, scanning and analysis-----
//-----------------------------------------------------------------------------------------------------------------------------------------
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "BLEAddress.h"                                   //libraries for BLE
#include "esp_bt_main.h"
#include "esp_bt_device.h"       
BLECharacteristic *pCharacteristic2;
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks                //find BLE server
{
    void onResult(BLEAdvertisedDevice advertisedDevice){}
};

class MyClientCallback : public BLEClientCallbacks {                            //for searching
  void onConnect(BLEClient* pclient) {}
  void onDisconnect(BLEClient* pclient) {
      pclient->disconnect();
  }
};
const int CUTOFF = -74;                                  //bluetooth signal strength to be within 2m parameter
//-----------------------------------------------------------------------------------------------------------------------------------------
//-----------------the section below contains the libraries and global variables to write to and read from EEPROM--------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------
#include <esp_log.h>                                      //library to use ESP32 in Arduino IDE
#include "esp32-hal-cpu.h"
#include "sdkconfig.h"
#include "driver/adc.h"
#include <string.h>                                         //library for string type
#include <EEPROM.h>                                       //library to access EEPROM                                      
int EEPROM_usage = 0;                                     //number of BluetoothIDs are being stored in the EEPROM
int EEPROM_uploaded = 0;                                  //address of EEPROM has been uploaded to LAMP server
String close_contact[682];  


//-----------------------------------------------------------------------------------------------------------------------------------------
//-----------------the section below contains the libraries and global variables to display the SSD1306 OLED Display-----------------------
//-----------------------------------------------------------------------------------------------------------------------------------------
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//-----------------------------------------------------------------------------------------------------------------------------------------
#include "WiFi.h"                                         //Enables the ESP32 to connect to the local network (via WiFi)
long long int scanTimer;
//---------------------------------------------------------------------------------------------SETUP----------------------------------------------------------------------------------------------------------------
void setup()
 {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
    Serial.begin(115200);
    EEPROM.begin(4096);                                   //allocate 4096 bytes in EEPROM for:
                                                          //0-3959 for 440 BluetoothIDs
                                                          //3960-3961 for 1 EEPROM_usage
                                                          //3962-3963 for 1 EEPROM_uploaded 
                                                          //3964 for 1 last state
                                                          //3965-3996 for 1 wifi ssid
                                                          //3997-4059 for 1 wifi password
                                                          //4060-4091 for 1 authentication token
                                                          //4092 for 1 pass
                                                          //4093 for 1 wifi name saved: 0 if no wifi, otherwise equal number of character                                                    
                                                          //4094 for 1 wifi pass saved: 0 if no wifi, otherwise equal number of character 
                                                          //4095 for 1 auth saved: 0 or 1
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
      Serial.println(F("SSD1306 allocation failed"));
      for(;;);
    }
    welcome_screen();
    BLEDevice::init("MyESP");                                                    //initialzie the local BLE environment as COVID_Alert_Device client device
    BLEServer *pServer = BLEDevice::createServer();                                 //create server
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *Sending_Characteristic = pService->createCharacteristic(            //characteristic for sending data
                                     CHARACTERISTIC_UUID,
                                     BLECharacteristic::PROPERTY_READ |
                                     BLECharacteristic::PROPERTY_WRITE
    );
    pService->start();
    pServer->getAdvertising()->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    reconnectWiFi();
    Serial.print("WiFi local IP: ");
    Serial.println(WiFi.localIP());
    initialize_timer();
    setCpuFrequencyMhz(80);                               //Set CPU clock to 80MHz to save power
    Serial.print("CPU frequency: ");
    Serial.print(ESP.getCpuFreqMHz());
    Serial.println("MHz");
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 1); //enable brownout detector
 }
//---------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
  switch (state){
    case WORKING:
        print_info("Dr.ESP32 is working  normally", true, 0);
        if (EEPROM_usage > 440){
            state = FULLMEM;
        } else {
            if (scanTimer + 5000 < millis()){
                BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
                BLEScanResults scanResults = pBLEScan->start(3, false);                     //scan for 3 seconds, not continuous
                Serial.print("Number of devices found this loop:");
                Serial.println(scanResults.getCount());
                Serial.println("List of devices found: ");                                                                    
                //-----------------------------------------------------------------------------------------------------------------------------------------
                //---------------------------------the section below is to store BluetoothID to EEPROM if connection is off--------------------------------
                //-----------------------------------------------------------------------------------------------------------------------------------------
                for (int i=0 ; i < scanResults.getCount(); i++){                                        //check each new BluetoothID found
                    BLEAdvertisedDevice device = scanResults.getDevice(i); 
                    Serial.println(device.toString().c_str());                                                  //print to console
                    if (device.getRSSI() > CUTOFF  && device.getServiceUUID().equals(serviceUUID)){                  //check the Bluetooth signal is strong enough to ensure that Bluetooth user is within 2 meter parameter  
                        int ToDevice = checkAddress(device.getAddress().toString());
                        if ( ToDevice != 0){
                            if (sendToLAMPServer(DeviceOrder, ToDevice, get_contact_time())){                                          //if sent to broker successfully
                                Serial.println("Sent to server!");
                            } else {                                                                        //if cannot sent to broker
                                Serial.println("Failed to send to server.");
                                reconnectWiFi();  
                                Write_Unsigned_Int_Into_EEPROM(9*EEPROM_usage, ToDevice);            //write order to EEPROM
                                Write_Time_Into_EEPROM(9*EEPROM_usage + 2);                 //write time to EEPROM
                                EEPROM_usage++;
                            }
                        }
                    } 
                }
                BLEDevice::getScan()->clearResults();                                    //clear old memory of BLE scanned
                scanTimer = millis();
                Serial.print("Number of close contact bluetoothID saved in EEPROM:");
                Serial.println(EEPROM_usage);
            }
            
            //-----------------------------------------------------------------------------------------------------------------------------------------
            //------the section below is check if connection is back on, if yes send the BluetoothID to LAMP server------
            //-----------------------------------------------------------------------------------------------------------------------------------------
            int max_value = EEPROM_uploaded + 2;                                                  
            if (EEPROM_usage  <= max_value && EEPROM_usage  >= EEPROM_uploaded){                      //send less then 2 values per loop                       
                max_value = EEPROM_usage ;         
            }                                                                      
            if (reconnectWiFi()){
                for (int k= EEPROM_uploaded ; k < max_value; k++){
                    unsigned int ToDevice = Read_Unsigned_Int_From_EEPROM(9*k);
                    String contact_time = Read_Time_From_EEPROM(9*k+2);
                    if (sendToLAMPServer(DeviceOrder, ToDevice, contact_time)){
                        EEPROM_uploaded++;
                    }
                }
            }
            Write_Unsigned_Int_Into_EEPROM(3960, EEPROM_usage);
            Write_Unsigned_Int_Into_EEPROM(3962, EEPROM_uploaded); 
                                                                         
        }
    break;
    
    case FULLMEM:
      if (EEPROM_uploaded == EEPROM_usage){                     //if EEPROM full and no data is missed, back to working now
          print_info("EEPROM full. All datais uploaded. Switch  to working now", true, 2000);
          Write_Unsigned_Int_Into_EEPROM(3960, 0);                       //Clear memory for number of EEPROM already used
          Write_Unsigned_Int_Into_EEPROM(3962, 0);                       //Clear memory for number of EEPROM already uploaded
          EEPROM.write(3964, WORKING);                          //move to working state after reset
          EEPROM.commit();
          ESP.restart();
      } else {
          print_info("Switch to upload only mode", true, 2000);
          state = UPLOAD_ONLY;
      }
    break;

    case UPLOAD_ONLY:
      int max_value = EEPROM_uploaded + 2;                                                  
      if (EEPROM_usage  <= max_value && EEPROM_usage  >= EEPROM_uploaded){                                   //send less then 2 values per loop                       
          max_value = EEPROM_usage ;         
      }                                                                      
      if (reconnectWiFi()){                                                 //good connection to WiFi
            print_info("Dr.ESP32 is uploading only", true, 0);
            for (int k= EEPROM_uploaded ; k < max_value; k++){
                unsigned int ToDevice = Read_Unsigned_Int_From_EEPROM(9*k);
                String contact_time = Read_Time_From_EEPROM(9*k+2);
                if (sendToLAMPServer(DeviceOrder, ToDevice, contact_time)){
                    EEPROM_uploaded++;
                }
            }
        } else {                                                             //lose connection to WiFi
          print_info("Cannot connect to WiFi ", true, 0);
        }
        Write_Unsigned_Int_Into_EEPROM(3960, EEPROM_usage);
        Write_Unsigned_Int_Into_EEPROM(3962, EEPROM_uploaded); 
        Serial.println(" ");
        if (EEPROM_usage == EEPROM_uploaded){
          print_info("Uploaded completely!", true, 1000);
          Write_Unsigned_Int_Into_EEPROM(3960, 0);                       //Clear memory for number of EEPROM already used
          Write_Unsigned_Int_Into_EEPROM(3962, 0);                       //Clear memory for number of EEPROM already uploaded
          state = WORKING;
      }
    break;
    //need to make the LOWBAATT case
    //undefined state, will automatically restart within 5 secs
  }
  EEPROM.write(3964, state);
  EEPROM.commit();
  
}