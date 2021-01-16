#ifndef __FUNCTIONS_HPP__
#include "Arduino.h" 
//---------------------------------------------TIME WORK-------------------------------------------------------------
void initialize_timer();
String get_contact_time();
//---------------------------------------------ESP WORK--------------------------------------------------------------
void Write_Unsigned_Int_Into_EEPROM(int address, unsigned int value);
void Write_Time_Into_EEPROM(int address);
unsigned int Read_Unsigned_Int_From_EEPROM(int address);
String Read_Time_From_EEPROM(int address);
//--------------------------------------------------WIFI WORK-------------------------------------------------------
bool reconnectWiFi();
//--------------------------------------------BLUETOOTH WORK--------------------------------------------------------
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "BLEAddress.h" 
void printDeviceAddress();
int checkAddress(std::string address);
//---------------------------------------------LAMP WORK-------------------------------------------------------------
bool sendToLAMPServer(unsigned int FromDevice, unsigned int ToDevice, String contact_time);
#endif
