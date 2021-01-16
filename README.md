# Dr.ESP32
[***Download full project (contains ...)***](https://github.com/hientv1999/Dr.ESP32/archive/main.zip)  
## Table of Contents
- [The motivation](#the-motivation)  
- [Description](#description)  
- [Required components](#required-components)  
- [Setup Procedure](#setup-procedure)  
  - [ESP32](#esp32)  
    - [Hardware connections](#hardware-connections)  
    - [Programming for ESP32](#programming-for-esp32)  
  - [Raspberry Pi 4](#raspberry-pi-4)  
    - [Operating System installation](#operating-system-installation)  
    - [LAMP server installation](#lamp-server-installation)  
    - [Programming for Raspberry Pi 4](#programming-for-raspberry-pi-4)  
- [Arisen issues and solutions](#arisen-issues-and-solution)  
- [Gallery](#gallery)  
## The motivation
***It was in 2020, a year that all people on Earth won't forget about the global pandemic.***
In Canada, beside other physical methods such as social distancing and sanitization, the only popular technological methods to slow the virus spread was `COVID Alert App`. `COVID Alert App` is a mobile app on both iOS and Android platforms. It can only work when the user enables Bluetooth on their smartphone. The smartphone will interact with nearby smartphone by an encrypted bluetooth message and store all the connection on the smartphone only. When an app user got infected by virus, he/she has to declare this to the app and the app will send alert from its database to other app users who have close contacted with that person.  
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
- Multiple `Dr.ESP32` devices as client device, somehow attached to or come along with the user. Each device can run continuously for 14-15 hours under each full charge.  

We will take the *supermarket* as an example. Each `Dr.ESP32` device will be mounted on the buggy or cart. Any `Dr.ESP32` in the buggy or cart station area should be disabled (under waiting state). Whenever a new customer enters the supermarket and takes the buggy or cart out of the station, the `Dr.ESP32` is then activated (under working state). In this state, `Dr.ESP32` constantly search for any nearby `Dr.ESP32` by scanning for any Bluetooth Low Energy (BLE) signal. If there is a BLE signal that:
- is strong enough ( indicating that a nearby customer is standing closer than 2 meters from the original user )
- has correct BLE Address ( a list of BLE Address of each `Dr.ESP32` is already stored in the sketch to identify if that is really from a `Dr.ESP32` or from smartphone, headphones,... and which `Dr.ESP32` it is )  

then the `Dr.ESP32` will record that `Dr.ESP32` in the contact list with the date. When the customer check out, the cashiers will either scan his supermarket member card (since the member card is linked to the customer's email), or ask for his email to know who has used the `Dr.ESP32`. Also, the cashiers will scan the barcode which reveals the `Dr.ESP32` device number on the buggy or cart so the database server can link the customer's email and the `Dr.ESP32` device number together.  
In the future, if one of the past customer has virus infection, he/she can just send the email to the supermarket with his/her virus test result for verficiation purpose. Once a staff reviews his/her email, they will enter the infected email into the database server. Then the server will automatically send alert email to all people who have close contacted with him/her, so they can have proper treatment and test.  

## Required components
Below table is for a project with 100 `Dr.ESP32` devices. Change is neccessary accordingly to the number of `Dr.ESP32` devices in the real application. Price is not including enclosure.
| Device | Number | Price |
| :-----: | :----: | :----: | 
| ESP32 Microcontroller Development Board | 100 | 800 |
| 1200mAh 3.2V LiFePO4 battery | 100 | 325 |
| LM2596 DC-DC HW-411 Buck Converter | 100 | 200 |
| Raspberry Pi 4 Model B 4GB | 1 | 87 |
Total cost is 1412
***Unit of currency: CAD***
