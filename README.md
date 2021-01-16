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
This is my first project with both  `ESP32 microcontroller` and `Raspberry Pi 4 single-board computer`. The initial reason that I came up with this project was due to the ineffectiveness of other approaches that has been used to reduce the spread of COVID 19. In Canada, beside other physical methods such as social distancing and sanitization, the only popular technological methods to slow the virus spread was `COVID Alert App`. `COVID Alert App` is a mobile app on both iOS and Android platforms. It can only work when the user enables Bluetooth on their smartphone. The smartphone will interact with nearby smartphone by an encrypted bluetooth message and store all the connection on the smartphone only. When an app user got infected by virus, he/she has to declare this to the app and the app will send alert from its database to other app users who have close contacted with that person.  
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
