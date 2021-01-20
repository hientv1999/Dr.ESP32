# Dr.ESP32
[***Download full project (contains ...)***](https://github.com/hientv1999/Dr.ESP32/archive/main.zip)  
## Table of Contents
- [The motivation](#the-motivation)  
- [Description](#description)  
- [Table of Contents](#table-of-contents)  
- [Setup Procedure](#setup-procedure)  
  - [ESP32](#esp32)  
    - [Hardware connections](#hardware-connections)  
    - [Programming for ESP32](#programming-for-esp32)  
  - [Raspberry Pi 4](#raspberry-pi-4)  
    - [Operating System installation](#operating-system-installation)  
    - [LAMP server installation](#lamp-server-installation)  
    - [Programming for Raspberry Pi 4](#programming-for-raspberry-pi-4)  
- [Arisen issues and solutions](#arisen-issues-and-solutions)  
- [Gallery](#gallery)  
## The motivation
It was in ***2020***, a year that all people on Earth won't forget about the ***global pandemic***.
In Canada, beside other physical methods such as social distancing and sanitization, the only popular technological methods to slow the virus spread was `COVID Alert App`. `COVID Alert App` is a mobile app on both iOS and Android platforms. It can only work when the user enables Bluetooth on their smartphone. The smartphone will interact with nearby smartphone by exchanging encrypted bluetooth messages and store all the connection in the smartphone only. When an app user got infected by virus, he/she has to declare this to the app and the app will send alert from its database to other app users who have close contacted with that person.  
[Click here to learn more about the COVID Alert App](https://www.canada.ca/en/public-health/services/diseases/coronavirus-disease-covid-19/covid-alert.html).  
  
However, there were many disadvantages that this application cannot perform as expected:
- #### Privacy:  
Many people were concerned about how their private information will be stored and used and who could access them.
- #### Dependency on users' device:  
It could be that people left their phone at home when they go out, the WiFi connection is terrible to get alert in time, or simply they don't have an compatible smartphone
- #### Dependency on user's routine:  
Smartphone users usually turn off the Bluetooth to increase battery life. Therefore, many app users may coincidentially leave the app running without Bluetooth which are useless and ineffective.
- #### Affordability:  
Not everyone can afford a compatible smartphone, and not everyone can have regular WiFi access. This is the biggest challenge if the app is applied in remote areas and developing countries.  
  
  
Therefore, this project arises to overcome all those challenges. Additionally, this is my first project with both  `ESP32 microcontroller` and `Raspberry Pi 4 single-board computer`.
## Description
The entire project consists of: 
- One single `Raspberry Pi 4 single-board computer` as the database server, placed in a secure place with continuous power supply (cooling solutions such as heat sink, fan are recommended for long-term usage)
- Multiple `Dr.ESP32` devices which contain a `ESP32` microcontroller and a `LiFePO4 3.2V 1200mAh` battery as client device, attached to or come along with the user. Each device can run continuously for 14-15 hours under each full charge.  

For example, this system is implemented in a supermarket. Each `Dr.ESP32` is attached to the buggy or cart. The device should be disabled (under waiting state) in the buggy or cart station area. Whenever a new customer enters the supermarket and takes the buggy or cart out of the station, the `Dr.ESP32` is activated (under working state). In this state,`Dr.ESP32` continuously searches for any nearby `Dr.ESP32` by scanning for any Bluetooth Low Energy (BLE) signal. The `Dr.ESP32` will record that `Dr.ESP32` in the contact list with the date and sent them to the database server (or store in EEPROM if it cannot connect to the server) if that Dr.ESP32 transmits a BLE signal that:
- is strong enough ( indicating that a nearby customer is standing closer than 2 meters from the original user )
- has correct BLE Address ( a list of BLE Address of each `Dr.ESP32` is already stored in the sketch to identify if that is really from a `Dr.ESP32` or from smartphone, headphones,... and which `Dr.ESP32` it is )  

When the customer checks out, the security guards will either ask for his/her email and enter the device number to the database so the server can link the customer's email and the `Dr.ESP32` device number together.  
In the future, if one of the past customers is infected, he/she can send the email to the supermarket with his/her virus test result for verification purposes. Once a staff reviews his/her email, they will enter the infected email into the database server. The server will automatically send an alert email to all people who had close contact with him/her.

## Table of Contents
Below table is for a project with 100 `Dr.ESP32` devices. Change is neccessary accordingly to the number of `Dr.ESP32` devices in the real application. Price is not including enclosure.
| Device | Number | Price |
| :-----: | :----: | :----: | 
| ESP32 Microcontroller Development Board | 100 | 800 |
| 1200mAh 3.2V LiFePO4 battery | 100 | 325 |
| LM2596 DC-DC HW-411 Buck Converter | 100 | 200 |
| Raspberry Pi 4 Model B 4GB | 1 | 87 |  

Total cost is ***1412***  
***Unit of currency: CAD***  
## Setup Procedure
### ESP32
#### Hardware connections
Still under review but you can simply connect the USB cable to power the ESP32 on for testing and reviewing purposes. 
#### Programming for ESP32
In this project, I use Arduino IDE for compiling and uploading the program. It is a free software and most tinkerer will be familiar with.
- First of all, if this is the first time you use ESP32 in Arduino IDE, you should install [ESP32 Add-on for Arduino IDE](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)  
- Secondly, you may want to install the library [PubSubClient.h library](https://github.com/knolleary/pubsubclient/archive/master.zip)
- Thirdly the ESP32 can only store 1,310,720 bytes of sketch under default, which is not enough for our program (~ 1.5 KB). You should extend the sketch storage of the ESP32 by repartition the ESP32:
  - Change `default.csv` file
    - Open file `C/Users/”YourName”/AppData/Local/Arduino15/packages/esp32/hardware/esp32/1.0.4/tools/partitions/default.csv`
    - Change it into  
    
    |  #Name  |  Type  | SubType | Offset |  Size  | Flags |
    | :-----: | :----: | :----:  | :----: | :----: |:----: |
    |   nvs   |  data  |   nvs   | 0x9000 | 0x5000 |       |
    | otadata |  data  |   ota   | 0xe000 | 0x2000 |       |
    |   app0  |  app   |  ota_0  | 0x10000|0x340000|       |
    |  eeprom |  data  |  0x99   |0x350000| 0x1000 |       |
    |  spiffs |  data  | spiffs  |0x351000|0xAF000 |       |
    
  - Change `boards.txt` file
    - Open file `C/Users/”YourName”/AppData/Local/Arduino15/packages/esp32/hardware/esp32/1.0.4/boards.txt`
    - Change the first block into
    ```
    esp32.name=ESP32 Max
    
    esp32.upload.tool=esptool_py
    esp32.upload.maximum_size=3407872
    esp32.upload.maximum_data_size=327680
    esp32.upload.wait_for_upload_port=true

    esp32.serial.disableDTR=true
    esp32.serial.disableRTS=true

    esp32.build.mcu=esp32
    esp32.build.core=esp32
    esp32.build.variant=esp32
    esp32.build.board=ESP32_DEV

    esp32.build.f_cpu=240000000L
    esp32.build.flash_size=4MB
    esp32.build.flash_freq=40m
    esp32.build.flash_mode=dio
    esp32.build.boot=dio
    esp32.build.partitions=default
    esp32.build.defines= 
    ```
- Now, you are ready to compile and upload the program code onto the ESP32 microcontroller. There are 3 files in total:
  * [ESP32.ino](https://github.com/hientv1999/Dr.ESP32/blob/main/ESP32/ESP32.ino)
  * [functions.cpp](https://github.com/hientv1999/Dr.ESP32/blob/main/ESP32/functions.cpp)
  * [functions.hpp](https://github.com/hientv1999/Dr.ESP32/blob/main/ESP32/functions.hpp)
- Remember to modify the `ssid` and `pass` variables in the ESP32.ino file corresponding to your WiFi name and WiFi password.
## Raspberry Pi 4
### Operating System Installation
Follow [this tutorial](https://diyi0t.com/raspberry-pi-headless-setup-tutorial/) to install the OS. I recommend to install the Raspberry Pi OS (32-bit) Lite.  

***Notice***: In step sudo nano /etc/dhcpcd.conf, use these lines instead:
```
interface wlan0
static ip_address = A/24
static routers = B
static domain_name_servers = B
```
where A is the Raspberry Pi IP Address (first 8-digit address from the command ```hostname -I```) and B is A with last digit replaced by 1
### LAMP Server Installation
Follow [this tutorial](https://randomnerdtutorials.com/raspberry-pi-apache-mysql-php-lamp-server/) to install the LAMP Server on Raspberry Pi 4.  
### Programming for Raspberry Pi 4
1. Preparing Your MySQL Database:
    * Open your browser and type `http://A/phpmyadmin` where A is the Raspberry Pi IP Address
    * Login by the account you setup before
    * Select the `Databases` menu at the top, complete the `Create database` fields by the drop down menu:
      * esp_data
      * utf8mb4_general_ci
    * Press the `Create` button
    * save your database name as esp_data
2. Create SQL tables:
    * Create SQL table to store contact between devices:
      * In the left sidebar select your database name `esp_data`
      * Open the `SQL` tab, Copy [this SQL query](https://github.com/hientv1999/Dr.ESP32/blob/main/LAMP/COVID_19.sql) into the SQL query field then press the `Go` button to create your table
    * Create SQL table to store customer information:
      * In the left sidebar select your database name `esp_data`
      * Open the `SQL` tab, Copy [this SQL query](https://github.com/hientv1999/Dr.ESP32/blob/main/LAMP/CustomerInfo.sql) into the SQL query field then press the `Go` button to create your table
    * Create SQL table to identify the infected customer :
      * In the left sidebar select your database name `esp_data`
      * Open the `SQL` tab, Copy [this SQL query](https://github.com/hientv1999/Dr.ESP32/blob/main/LAMP/trackInfection.sql) into the SQL query field then press the `Go` button to create your table
3. Create PHP files in `Raspberry Pi 4`:
    * PHP Script HTTP POST - Insert contact data into COVID 19 Database
      * Connecting to Raspberry Pi with an SSH connection, then type `sudo nano /var/www/html/post-esp-data.php`
      * Copy [this PHP script](https://github.com/hientv1999/Dr.ESP32/blob/main/LAMP/post-esp-data.php) to the newly created file
      * Remember to modify the $dbname, $username and $password variables with your own ones
    * PHP Script - Display COVID 19 Database Content  
      * Type `sudo nano /var/www/html/esp-data.php`
      * Copy [this PHP script](https://github.com/hientv1999/Dr.ESP32/blob/main/LAMP/esp-data.php) to the newly created file
      * Remember to modify the $dbname, $username and $password variables with your own ones
    * PHP Script - Insert customer information into CustomerInfo Database 
      * Type `sudo nano /var/www/html/checkOutCustomer.php`
      * Copy [this PHP script](https://github.com/hientv1999/Dr.ESP32/blob/main/LAMP/checkOutCustomer.php) to the newly created file
      * Remember to modify the $dbname, $username and $password variables with your own ones
    * PHP Script - Temporarily store infected customers from the original customer  
      * Type `sudo nano /var/www/html/trackInfection.php`
      * Copy [this PHP script](https://github.com/hientv1999/Dr.ESP32/blob/main/LAMP/trackInfection.php) to the newly created file
      * Remember to modify the $dbname, $username and $password variables with your own ones
4. Create Python file to send email upon request by LAMP Server
    * Connecting to Raspberry Pi with an SSH connection, then type `sudo nano /var/www/html/newmailing.py`
    * Copy [this Python script](https://github.com/hientv1999/Dr.ESP32/blob/main/LAMP/newmailing.py) to the newly created file
    * Remember to modify the `username` and `password` variables. This project will use a Gmail account to send out alert email.
## Arisen issues during development and solutions
1. Too big sketch size
    * As mentioned in [Programming for Raspberry Pi 4](#programming-for-raspberry-pi-4), the default available sketch size is 1,310,720 bytes meanwhile the program sketch size is ~1.5 KB so we have to change the partition setting of ESP32
2. Constantly rebooting when being powered by battery
    * There is a brownout sensor in ESP32 that makes the ESP32 reboot if it detects the supply voltage drops significantly. As our `Dr.ESP32` is powered by battery, during setup, it has to draw huge current to initialize BLE and WiFi, leading to the significant supply voltage drop. Therefore, our `Dr.ESP32` keeps rebooting and cannot work. To overcome this, we have to disable the brownout sensor during starting BLE and WiFi.
    * The solution is by adding these lines of code to disable brownout sensor at the beginning of setup and enable brownout sensor at the ending of setup.
      * ```WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);``` to disable
      * ```WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 1);``` to enable
3. BLE connection is waiting for too long
    * I used to make the `Dr.ESP32` exchange device number to recognize each other. However, there are some moments the BLE connection cannot connect and it just waits for really long time for the connection ( ~50 days in default) until it proceeds to the next instructions.
    * The solution is by changing the waiting time in the `FreeRTO.cpp` file:
      * Go to `C/Users/”YourName”/AppData/Local/Arduino15/packages/esp32/hardware/esp32/1.0.4/libraries/BLE/src/FreeRTO.cpp`
      * Change `xSemaphoreTake(m_semaphore, portMAX_DELAY)` into `xSemaphoreTake(m_semaphore, 3000)`
## Gallery
