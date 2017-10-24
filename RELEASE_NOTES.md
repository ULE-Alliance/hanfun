# Release Notes

|             |                            |
| ---------:  | -------------------------- |
| __Project__ | HAN-FUN CI                 |
| __Version__ | v1.5.0                     |
| __Date__    | 24/10/2017                 |

## Overview

This a ULE Alliance's HAN-FUN Common Implementation public release.

This software release allows third-party vendors to easily create devices that are compatible with the ULE Alliance HAN-FUN standard v1.3.0.

Not all of the standard has been implemented so far, please see the *Features* section for further information on what has been implemented and what is missing.

For more information on requirements, building and using this software package, please see the README file present in the software distribution.

## Features

* A - Attribute
* C - %Command

### New

* Core Services & Interfaces
  * Batch Program Management
  * Event Scheduling
  * Group Management
  * Group Table
  * Time
  * Weekly Scheduling

* Profiles
  * Colour Bulb
  * Dimmable Colour Bulb
  * Tracker
  * Simple Keypad
  * Simple Pendant
  * User Interface Lock

* Interfaces
  * Colour Control
  * Simple Light Sensor
  * Simple Keypad

### 1. Protocol [HF-Protocol v1.2.1]

 * General support for all defined features has been implemented.

### 2. Core Services & Interfaces [HF-Services v2.0.0]

#### 2.1 Device Management Service [6.1]

| __Reference__ | __Commands & Attributes__            | __Type__ | __Client__     | __Server__  |
| ------------- | ------------------------------------ | :------: | -------------- | ----------- |
| 6.1.2.1       | Number of Entries                    | A        | Not Applicable | Implemented |
| 6.1.5.1       | Register device                      | C        | Implemented    | Implemented |
| 6.1.5.2       | De-register device                   | C        | Implemented    | Implemented |
| 6.1.5.3       | Start Session Read Registration Info | C        | Implemented    | Implemented |
| 6.1.5.4       | End Session Read Registration Info   | C        | Implemented    | Implemented |
| 6.1.5.5       | Get Entries                          | C        | Implemented    | Implemented |

#### 2.2 Bind Management Service [6.2]

| __Reference__ | __Commands & Attributes__                          | __Type__ | __Client__      | __Server__      |
| ------------- | -------------------------------------------------- | :------: | --------------  | -------------   |
| 6.2.2.1       | Number of Entries                                  | A        | Not Applicable  | Implemented     |
| 6.2.5.1       | Add Bind                                           | C        | Implemented     | Implemented     |
| 6.2.5.2       | Remove Bind                                        | C        | Implemented     | Implemented     |
| 6.2.5.3       | Remove Bind by SRC Device Address                  | C        | Not Implemented | Not Implemented |
| 6.2.5.4       | Remove Bind by SRC Device Address and Unit ID      | C        | Not Implemented | Not Implemented |
| 6.2.5.5       | Remove Bind by DST Address                         | C        | Not Implemented | Not Implemented |
| 6.2.5.6       | Remove Bind by DST Address and Unit ID             | C        | Not Implemented | Not Implemented |
| 6.2.5.7       | Start Session Read Bind Info                       | C        | Implemented     | Implemented     |
| 6.2.5.8       | End Session Read Bind Info                         | C        | Implemented     | Implemented     |
| 6.2.5.9       | Get Bind Entries by Index                          | C        | Implemented     | Implemented     |
| 6.2.5.10      | Get Bind Entries by SRC Device Address             | C        | Not Implemented | Not Implemented |
| 6.2.5.11      | Get Bind Entries by SRC Device Address and Unit ID | C        | Not Implemented | Not Implemented |
| 6.2.5.12      | Get Bind Entries by DST Address                    | C        | Not Implemented | Not Implemented |
| 6.2.5.13      | Get Bind Entries by DST Address and Unit ID        | C        | Not Implemented | Not Implemented |

#### 2.3 Group Management Service [6.3]

| __Reference__ | __Commands & Attributes__ | __Type__ | __Client__     | __Server__    |
| ------------- | ------------------------- | :------: | -------------- | ------------- |
| 6.3.2.1       | Number of Groups          | A        | Not Applicable | Implemented   |
| 6.3.5.1       | Create Group              | C        | Implemented    | Implemented   |
| 6.3.5.2       | Delete Group              | C        | Implemented    | Implemented   |
| 6.3.5.3       | Add to Group              | C        | Implemented    | Implemented   |
| 6.3.5.4       | Remove from Group         | C        | Implemented    | Implemented   |
| 6.3.5.5       | Get Group Info            | C        | Implemented    | Implemented   |

#### 2.4 Identify Device Interface [6.4]

| __Reference__ | __Commands & Attributes__ | __Type__ | __Client__      | __Server__      |
| ------------- | ------------------------- | :------: | --------------  | -------------   |
| 6.4.3.1       | Identification            | C        | Not Implemented | Not Implemented |

#### 2.5 Device Information Service [6.5]

| __Reference__ | __Commands & Attributes__              | __Type__ | __Client__     | __Server__      |
| ------------- | -------------------------------------- | :------: | -------------- | --------------- |
| 6.5.1.1       | Core Release version                   | A        | Not Applicable | Implemented     |
| 6.5.1.2       | Profile Release version                | A        | Not Applicable | Implemented     |
| 6.5.1.3       | Interface Release version              | A        | Not Applicable | Implemented     |
| 6.5.1.4       | Extra Capabilities                     | A        | Not Applicable | Implemented     |
| 6.5.1.5       | Minimum Sleep Time                     | A        | Not Applicable | Not Implemented |
| 6.5.1.6       | Actual Response Time [Paging Interval] | A        | Not Applicable | Not Implemented |
| 6.5.1.7       | Application version                    | A        | Not Applicable | Implemented     |
| 6.5.1.8       | Hardware version                       | A        | Not Applicable | Implemented     |
| 6.5.1.9       | EMC                                    | A        | Not Applicable | Implemented     |
| 6.5.1.10      | RFPI/ IPUI                             | A        | Not Applicable | Not Implemented |
| 6.5.1.11      | Manufacturer Name                      | A        | Not Applicable | Implemented     |
| 6.5.1.12      | Location                               | A        | Not Applicable | Not Implemented |
| 6.5.1.13      | Device Enable                          | A        | Not Applicable | Not Implemented |
| 6.5.1.14      | Friendly Name                          | A        | Not Applicable | Not Implemented |
| 6.5.1.15      | Device Unique Identifier [UID]         | A        | Not Applicable | Implemented     |
| 6.5.1.16      | Device Serial Number                   | A        | Not Applicable | Not Implemented |

#### 2.6 Attribute Reporting Service [6.6]

| __Reference__ | __Commands & Attributes__    | __Type__ | __Client__      | __Server__      |
| ------------- | ---------------------------- | :------: | --------------- | --------------- |
| 6.6.2.1       | Number of Reports            | A        | Not Applicable  | Implemented     |
| 6.6.2.2       | Number of Periodic Reports   | A        | Not Applicable  | Implemented     |
| 6.6.2.3       | Number of Event Reports      | A        | Not Applicable  | Implemented     |
| 6.6.4.1       | Periodic Report Notification | C        | Implemented     | Implemented     |
| 6.6.4.2       | Event Report Notification    | C        | Implemented     | Implemented     |
| 6.6.5.1       | Create Periodic Report       | C        | Implemented     | Implemented     |
| 6.6.5.2       | Create Event Report          | C        | Implemented     | Implemented     |
| 6.6.5.3       | Add Periodic Report Entry    | C        | Implemented     | Implemented     |
| 6.6.5.4       | Add Event Report Entry       | C        | Implemented     | Implemented     |
| 6.6.5.5       | Delete Report                | C        | Implemented     | Implemented     |
| 6.6.5.6       | Get Periodic Report Entries  | C        | Not Implemented | Not Implemented |
| 6.6.5.7       | Get Event Report Entries     | C        | Not Implemented | Not Implemented |

#### 2.7 Batch Program Management [6.7]

| __Reference__ | __Commands & Attributes__    | __Type__ | __Client__      | __Server__      |
| ------------- | ---------------------------- | :------: | --------------- | --------------- |
| 6.7.2.1       | Maximum Number of Entries    | A        | Not Applicable  | Implemented     |
| 6.7.2.2       | Number of Entries            | A        | Not Applicable  | Implemented     |
| 6.7.5.1       | Define Program               | C        | Implemented     | Implemented     |
| 6.7.5.2       | Invoke Program               | C        | Implemented     | Implemented     |
| 6.7.5.3       | Delete Program               | C        | Implemented     | Implemented     |
| 6.7.5.4       | Delete All Programs          | C        | Implemented     | Implemented     |
| 6.7.5.5       | Get Program Actions          | C        | Implemented     | Implemented     |

### 2.8 Event Scheduling Interface [6.8]

| __Reference__ | __Commands & Attributes__    | __Type__ | __Client__      | __Server__      |
| ------------- | ---------------------------- | :------: | --------------- | --------------- |
| 6.8.2.1       | Maximum Number of Entries    | A        | Not Applicable  | Implemented     |
| 6.8.2.2       | Number of Entries            | A        | Not Applicable  | Implemented     |
| 6.8.2.3       | Scheduler Status             | A        | Not Applicable  | Implemented     |
| 6.8.5.1       | Activate Scheduler           | C        | Implemented     | Implemented     |
| 6.8.5.2       | Define Event                 | C        | Implemented     | Implemented     |
| 6.8.5.3       | Update Event Status          | C        | Implemented     | Implemented     |
| 6.8.5.4       | Get Event Entry              | C        | Implemented     | Implemented     |
| 6.8.5.5       | Delete Event                 | C        | Implemented     | Implemented     |
| 6.8.5.6       | Delete All Events            | C        | Implemented     | Implemented     |

### 2.9 Weekly Scheduling Interface [6.9]

| __Reference__ | __Commands & Attributes__    | __Type__ | __Client__      | __Server__      |
| ------------- | ---------------------------- | :------: | --------------- | --------------- |
| 6.9.2.1       | Maximum Number of Entries    | A        | Not Applicable  | Implemented     |
| 6.9.2.2       | Number of Entries            | A        | Not Applicable  | Implemented     |
| 6.9.2.3       | Weekly Scheduler Status      | A        | Not Applicable  | Implemented     |
| 6.9.5.1       | Activate Weekly Scheduler    | C        | Implemented     | Implemented     |
| 6.9.5.2       | Define Weekly Event          | C        | Implemented     | Implemented     |
| 6.9.5.3       | Update Weekly Event Status   | C        | Implemented     | Implemented     |
| 6.9.5.4       | Get Weekly Event Entry       | C        | Implemented     | Implemented     |
| 6.9.5.5       | Delete Weekly Event          | C        | Implemented     | Implemented     |
| 6.9.5.6       | Delete All Weekly Events     | C        | Implemented     | Implemented     |

#### 2.10 Tamper Interface [6.10]

| __Reference__ | __Commands & Attributes__ | __Type__ | __Client__      | __Server__      |
| ------------- | ------------------------- | :------: | --------------- | --------------- |
| 6.10.1.1      | State                     | A        | Not Implemented | Not Implemented |
| 6.10.3.1      | Tamper Alert On           | C        | Not Implemented | Not Implemented |
| 6.10.3.2      | Tamper Alert Off          | C        | Not Implemented | Not Implemented |

#### 2.11 Timer Interface [6.11]

| __Reference__ | __Commands & Attributes__ | __Type__ | __Client__      | __Server__      |
| ------------- | ------------------------- | :------: | --------------- | --------------- |
| 6.11.1.1      | Time                      | A        | Not Applicable  | Implemented |

#### 2.12 Power Interface [6.12]

| __Reference__ | __Commands & Attributes__         | __Type__ | __Client__      | __Server__      |
| ------------- | --------------------------------- | :------: | --------------- | --------------- |
| 6.12.1.1      | Mains Voltage                     | A        | Not Applicable  | Not Implemented |
| 6.12.1.2      | Mains Frequency                   | A        | Not Applicable  | Not Implemented |
| 6.12.1.3      | Mains Alarms                      | A        | Not Applicable  | Not Implemented |
| 6.12.1.4      | Mains Minimum Voltage Threshold   | A        | Not Applicable  | Not Implemented |
| 6.12.1.5      | Mains Maximum Voltage Threshold   | A        | Not Applicable  | Not Implemented |
| 6.12.1.6      | Mains Voltage Dwell Trip Point    | A        | Not Applicable  | Not Implemented |
| 6.12.1.7      | Battery Voltage                   | A        | Not Applicable  | Not Implemented |
| 6.12.1.8      | Battery Manufacturer              | A        | Not Applicable  | Not Implemented |
| 6.12.1.9      | Battery Type                      | A        | Not Applicable  | Not Implemented |
| 6.12.1.10     | Battery AHr Rating                | A        | Not Applicable  | Not Implemented |
| 6.12.1.11     | Battery Cell Quantity             | A        | Not Applicable  | Not Implemented |
| 6.12.1.12     | Battery Rated Voltage             | A        | Not Applicable  | Not Implemented |
| 6.12.1.13     | Battery Status                    | A        | Not Applicable  | Not Implemented |
| 6.12.1.14     | Battery Minimum Voltage Threshold | A        | Not Applicable  | Not Implemented |
| 6.12.1.15     | Battery Maximum Voltage Threshold | A        | Not Applicable  | Not Implemented |
| 6.12.1.16     | Battery Charge Level              | A        | Not Applicable  | Not Implemented |
| 6.12.1.17     | Battery Minimum Charge Level      | A        | Not Applicable  | Not Implemented |
| 6.12.1.18     | Battery Maximum Charge Level      | A        | Not Applicable  | Not Implemented |
| 6.12.1.19     | Power Source                      | A        | Not Applicable  | Not Implemented |
| 6.12.3.1      | Low Mains Voltage Alert           | C        | Not Implemented | Not Implemented |
| 6.12.3.2      | High Mains Voltage Alert          | C        | Not Implemented | Not Implemented |
| 6.12.3.3      | Low Battery Voltage Alert         | C        | Not Implemented | Not Implemented |
| 6.12.3.4      | High Battery Voltage Alert        | C        | Not Implemented | Not Implemented |
| 6.12.3.5      | Low Battery Charge Alert          | C        | Not Implemented | Not Implemented |
| 6.12.3.6      | High Battery Charge Alert         | C        | Not Implemented | Not Implemented |

#### 2.13 RSSI [6.13]

| __Reference__ | __Commands & Attributes__ | __Type__ | __Client__      | __Server__      |
| ------------- | ------------------------- | :------: | --------------- | --------------- |
| 6.13.1.1      | RSSI                      | A        | Not Applicable  | Implemented     |

#### 2.14 Keep Alive [6.14]

| __Reference__ | __Commands & Attributes__ | __Type__ | __Client__      | __Server__      |
| ------------- | ------------------------- | :------: | --------------- | --------------- |
| 6.14.1.1      | Interval                  | A        | Not Applicable  | Not Implemented |
| 6.14.3.1      | I am Alive                | C        | Not Applicable  | Not Implemented |

#### 2.15 SOUTA [6.15]

| __Reference__ | __Commands & Attributes__ | __Type__ | __Client__      | __Server__      |
| ------------- | ------------------------- | :------: | --------------- | --------------- |
| 6.15.3.1      | New Version Available     | C        | Not Applicable  | Implemented     |
| 6.15.4.1      | Check Version             | C        | Implemented     | Not Applicable  |
| 6.15.4.2      | Upgrade Complete          | C        | Implemented     | Not Applicable  |

### 3. Profiles [HF-Profiles v1.4.0]

| __Reference__ | __Profile__                          | __Status__  |
| ------------- | ------------------------------------ | ----------- |
| 8.1.1         | Simple On-Off Switchable             | Implemented |
| 8.1.2         | Simple On-Off Switch                 | Implemented |
| 8.1.3         | Simple Level Controllable            | Implemented |
| 8.1.4         | Simple Level Control                 | Implemented |
| 8.1.5         | Simple Level Controllable Switchable | Implemented |
| 8.1.6         | Simple Level Control switch          | Implemented |
| 8.1.7         | AC Outlet                            | Implemented |
| 8.1.8         | AC Outlet with Simple Power Metering | Implemented |
| 8.1.9         | Simple Light                         | Implemented |
| 8.1.10        | Dimmable Light                       | Implemented |
| 8.1.11        | Dimmer Switch                        | Implemented |
| 8.1.12        | Simple Door Lock                     | Implemented |
| 8.1.13        | Simple Door Bell                     | Implemented |
| 8.1.14        | Simple Power Meter                   | Implemented |
| 8.1.15        | Simple Temperature Sensor            | Implemented |
| 8.1.16        | Simple Humidity Sensor               | Implemented |
| 8.1.17        | Simple Air Pressure Sensor           | Implemented |
| 8.1.18        | Simple Button                        | Implemented |
| 8.1.19        | Controllable Thermostat              | Implemented |
| 8.1.20        | Simple LED                           | Implemented |
| 8.1.21        | Environment Monitor                  | Implemented |
| 8.1.22        | Colour Bulb                          | Implemented |
| 8.1.23        | Dimmable Colour Bulb                 | Implemented |
| 8.1.24        | Tracker                              | Implemented |
| 8.1.25        | Simple Keypad                        | Implemented |
| 8.2.1         | Simple Detector                      | Implemented |
| 8.2.2         | Door Open Close Detector             | Implemented |
| 8.2.3         | Window Open Close Detector           | Implemented |
| 8.2.4         | Motion Detector                      | Implemented |
| 8.2.5         | Smoke Detector                       | Implemented |
| 8.2.6         | Gas Detector                         | Implemented |
| 8.2.7         | Flood Detector                       | Implemented |
| 8.2.8         | Glass Break Detector                 | Implemented |
| 8.2.9         | Vibration Detector                   | Implemented |
| 8.2.10        | Siren                                | Implemented |
| 8.2.11        | Alertable                            | Implemented |
| 8.3.1         | Simple Pendant                       | Implemented |
| 8.4.1         | User Interface Lock                  | Implemented |
| 8.4.2         | User Interface                       | Implemented |
| 8.4.3         | Generic Application Logic            | Implemented |

### 4. Interfaces [HF-Interfaces v1.3.0]

#### 4.1 Alert [5.1]

| __Reference__ | __Commands & Attributes__ | __Type__ | __Client__      | __Server__      |
| ------------- | ------------------------- | :------: | --------------- | --------------- |
| 5.1.1.1       | State                     | A        | Not Applicable  | Implemented     |
| 5.1.1.2       | Enable                    | A        | Not Applicable  | Implemented     |
| 5.1.3.1       | Status                    | C        | Implemented     | Implemented     |

#### 4.2 On-Off [5.2]

| __Reference__ | __Commands & Attributes__ | __Type__ | __Client__      | __Server__      |
| ------------- | ------------------------- | :------: | --------------- | --------------- |
| 5.2.1.1       | State                     | A        | Not Applicable  | Implemented     |
| 5.2.4.1       | On                        | C        | Implemented     | Implemented     |
| 5.2.4.2       | Off                       | C        | Implemented     | Implemented     |
| 5.2.4.3       | Toggle                    | C        | Implemented     | Implemented     |

#### 4.3 Level Control [5.3]

| __Reference__ | __Commands & Attributes__ | __Type__ | __Client__      | __Server__      |
| ------------- | ------------------------- | :------: | --------------- | --------------- |
| 5.3.1.1       | Current Level             | A        | Not Applicable  | Implemented     |
| 5.3.4.1       | Set Level                 | C        | Implemented     | Implemented     |
| 5.3.4.2       | Increase Level            | C        | Implemented     | Implemented     |
| 5.3.4.3       | Decrease Level            | C        | Implemented     | Implemented     |

#### 4.4 Colour Control [5.4]

| __Reference__ | __Commands & Attributes__  | __Type__ | __Client__      | __Server__      |
| ------------- | -------------------------  | :------: | --------------- | --------------- |
| 5.4.1.1       | Supported Colour Modes     | A        | Not Applicable  | Implemented     |
| 5.4.1.2       | Current Colour Mode        | A        | Not Applicable  | Implemented     |
| 5.4.1.3       | Current Hue/Saturation     | A        | Not Applicable  | Implemented     |
| 5.4.1.4       | Current X/Y                | A        | Not Applicable  | Implemented     |
| 5.4.1.5       | Current Colour Temperature | A        | Not Applicable  | Implemented     |
| 5.4.4.1       | Move To Hue                | C        | Implemented     | Implemented     |
| 5.4.4.2       | Move Hue                   | C        | Implemented     | Implemented     |
| 5.4.4.3       | Step Hue                   | C        | Implemented     | Implemented     |
| 5.4.4.4       | Move To Saturation         | C        | Implemented     | Implemented     |
| 5.4.4.5       | Move Saturation            | C        | Implemented     | Implemented     |
| 5.4.4.6       | Step Saturation            | C        | Implemented     | Implemented     |
| 5.4.4.7       | Move To Hue And Saturation | C        | Implemented     | Implemented     |
| 5.4.4.8       | Move To XY                 | C        | Implemented     | Implemented     |
| 5.4.4.9       | Move XY                    | C        | Implemented     | Implemented     |
| 5.4.4.10      | Step XY                    | C        | Implemented     | Implemented     |
| 5.4.4.11      | Move To Colour Temperature | C        | Implemented     | Implemented     |
| 5.4.4.12      | Stop                       | C        | Implemented     | Implemented     |

#### 4.5 Colour Control [5.5]

| __Reference__ | __Commands & Attributes__ | __Type__ | __Client__      | __Server__      |
| ------------- | ------------------------- | :------: | --------------- | --------------- |
| 5.5.3.1       | KeyPressed                | C        | Implemented     | Implemented     |

#### 4.6 Simple Power Meter [5.6]

| __Reference__ | __Commands & Attributes__ | __Type__ | __Client__      | __Server__      |
| ------------- | ------------------------- | :------: | --------------- | --------------- |
| 5.6.1.1       | Energy                    | A        | Not Applicable  | Implemented     |
| 5.6.1.2       | Energy at Last Reset      | A        | Not Applicable  | Implemented     |
| 5.6.1.3       | Time at Last Reset        | A        | Not Applicable  | Implemented     |
| 5.6.1.4       | Instantaneous Power       | A        | Not Applicable  | Implemented     |
| 5.6.1.5       | Average Power             | A        | Not Applicable  | Implemented     |
| 5.6.1.6       | Average Power Interval    | A        | Not Applicable  | Implemented     |
| 5.6.1.7       | Voltage                   | A        | Not Applicable  | Implemented     |
| 5.6.1.8       | Current                   | A        | Not Applicable  | Implemented     |
| 5.6.1.9       | Frequency                 | A        | Not Applicable  | Implemented     |
| 5.6.1.10      | Power Factor              | A        | Not Applicable  | Implemented     |
| 5.6.1.11      | Report Interval           | A        | Not Applicable  | Implemented     |
| 5.6.3.1       | Report                    | C        | Implemented     | Implemented     |
| 5.6.4.1       | Measurement Reset         | C        | Not Implemented | Not Implemented |

#### 4.7 Simple Temperature [5.7]

| __Reference__ | __Commands & Attributes__ | __Type__ | __Client__      | __Server__      |
| ------------- | ------------------------- | :------: | --------------- | --------------- |
| 5.7.1.1       | Measured Temperature      | A        | Not Applicable  | Implemented     |
| 5.7.1.2       | Min. Measurable Temp.     | A        | Not Applicable  | Implemented     |
| 5.7.1.3       | Max. Measurable Temp.     | A        | Not Applicable  | Implemented     |
| 5.7.1.4       | Tolerance                 | A        | Not Applicable  | Implemented     |

#### 4.8 Simple Humidity [5.8]

| __Reference__ | __Commands & Attributes__ | __Type__ | __Client__      | __Server__      |
| ------------- | ------------------------- | :------: | --------------- | --------------- |
| 5.8.1.1       | Measured Humidity         | A        | Not Applicable  | Implemented     |
| 5.8.1.2       | Tolerance                 | A        | Not Applicable  | Implemented     |

#### 4.9 Simple Thermostat [5.9]

| __Reference__ | __Commands & Attributes__          | __Type__ | __Client__      | __Server__      |
| ------------- | ---------------------------------- | :------: | --------------- | --------------- |
| 5.9.1.1       | Supported Modes                    | A        | Not Applicable  | Implemented     |
| 5.9.1.2       | Operating Mode                     | A        | Not Applicable  | Implemented     |
| 5.9.1.3       | Fan Mode                           | A        | Not Applicable  | Implemented     |
| 5.9.1.4       | Heating Mode Temperature           | A        | Not Applicable  | Implemented     |
| 5.9.1.5       | Cooling Mode Temperature           | A        | Not Applicable  | Implemented     |
| 5.9.1.6       | Automatic Mode Heating Temperature | A        | Not Applicable  | Implemented     |
| 5.9.1.7       | Automatic Mode Cooling Temperature | A        | Not Applicable  | Implemented     |
| 5.9.1.8       | Heating Mode Temperature Offset    | A        | Not Applicable  | Implemented     |
| 5.9.1.9       | Cooling Mode Temperature Offset    | A        | Not Applicable  | Implemented     |
| 5.9.1.10      | Boost Duration                     | A        | Not Applicable  | Implemented     |
| 5.9.4.1       | Boost Start                        | C        | Implemented     | Implemented     |
| 5.9.4.2       | Boost Stop                         | C        | Implemented     | Implemented     |

#### 4.10 Simple Button [5.10]

| __Reference__ | __Commands & Attributes__         | __Type__ | __Client__      | __Server__      |
| ------------- | --------------------------------- | :------: | --------------- | --------------- |
| 5.10.1.1      | Short Press Maximum Duration      | A        | Not Applicable  | Implemented     |
| 5.10.1.2      | Extra Long Press Minimum Duration | A        | Not Applicable  | Implemented     |
| 5.10.1.3      | Double Press Gap Duration         | A        | Not Applicable  | Implemented     |
| 5.10.3.1      | Short Press                       | C        | Implemented     | Implemented     |
| 5.10.3.2      | Long Press                        | C        | Implemented     | Implemented     |
| 5.10.3.3      | Extra-Long Press                  | C        | Implemented     | Implemented     |
| 5.10.3.4      | Double Press                      | C        | Implemented     | Implemented     |

#### 4.11 Simple Visual Effects [5.11]

| __Reference__ | __Commands & Attributes__ | __Type__ | __Client__      | __Server__      |
| ------------- | ------------------------- | :------: | --------------- | --------------- |
| 5.11.4.1      | ON                        | C        | Implemented     | Implemented     |
| 5.11.4.2      | OFF                       | C        | Implemented     | Implemented     |
| 5.11.4.3      | Blink                     | C        | Implemented     | Implemented     |
| 5.11.4.4      | Fade                      | C        | Implemented     | Implemented     |
| 5.11.4.5      | Breath                    | C        | Implemented     | Implemented     |

#### 4.12 Air Pressure [5.12]

| __Reference__ | __Commands & Attributes__     | __Type__ | __Client__      | __Server__      |
| ------------- | ----------------------------- | :------: | --------------- | --------------- |
| 5.12.1.1      | Measured Air Pressure         | A        | Not Applicable  | Implemented     |
| 5.12.1.2      | Min. Measurable Air Pressure  | A        | Not Applicable  | Implemented     |
| 5.12.1.3      | Max. Measurable Air Pressure  | A        | Not Applicable  | Implemented     |
| 5.12.1.4      | Tolerance                     | A        | Not Applicable  | Implemented     |

#### 4.13 Simple Light Sensor [5.13]

| __Reference__ | __Commands & Attributes__     | __Type__ | __Client__      | __Server__      |
| ------------- | ----------------------------- | :------: | --------------- | --------------- |
| 5.13.1.1      | Measured Lux                  | A        | Not Applicable  | Implemented     |
| 5.13.1.2      | Min. Measurable Lux           | A        | Not Applicable  | Implemented     |
| 5.13.1.3      | Max. Measurable Lux           | A        | Not Applicable  | Implemented     |
| 5.13.1.4      | Tolerance                     | A        | Not Applicable  | Implemented     |

## Bug Fixes

  * See [Changelog](https://github.com/ULE-Alliance/hanfun/blob/master/Changelog) file.

## Known Issues and Problems

See the Todo List in the doxygen documentation for the software release.

