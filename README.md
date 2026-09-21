# Smart Irrigation System with Pump Protection and Mobile Alerts

An embedded automation system designed to monitor soil moisture and environmental conditions, automatically control a water pump, and provide mobile alerts for pump status.

The system uses an **Arduino UNO** as the main controller and integrates a soil moisture sensor, DHT11 temperature and humidity sensor, SIM800L GSM module, relay, and I2C LCD. The objective is to reduce manual intervention, improve water-use efficiency, and provide reliable monitoring of the irrigation process.

## Features

* Automatic irrigation based on soil moisture levels
* Automatic water pump control using a relay
* Pump protection and controlled operation
* Temperature and humidity monitoring using DHT11
* SMS-based pump status notifications using SIM800L
* Real-time system information displayed through an I2C LCD
* Embedded control implemented using C/C++

## Hardware Components

| Component            | Purpose                           |
| -------------------- | --------------------------------- |
| Arduino UNO          | Main control unit                 |
| Soil Moisture Sensor | Measures soil moisture level      |
| DHT11                | Measures temperature and humidity |
| SIM800L GSM Module   | Sends SMS notifications           |
| Relay Module         | Controls the water pump           |
| Water Pump           | Provides irrigation               |
| I2C LCD              | Displays system information       |
| Power Supply         | Provides power to the system      |

## Technologies Used

* **Programming:** Embedded C / C++
* **Microcontroller:** Arduino UNO
* **Development Environment:** Arduino IDE
* **Communication:** GSM, I2C
* **Sensors:** Soil Moisture Sensor, DHT11
* **Actuator Control:** Relay Module

## System Architecture

```text
                    ┌──────────────────────┐
                    │  Soil Moisture       │
                    │      Sensor          │
                    └──────────┬───────────┘
                               │
                               ▼
                    ┌──────────────────────┐
                    │                      │
                    │     Arduino UNO      │
                    │   Main Controller    │
                    │                      │
                    └──────┬───────┬───────┘
                           │       │
              ┌────────────┘       └──────────────┐
              ▼                                   ▼
     ┌─────────────────┐                 ┌─────────────────┐
     │  Relay Module   │                 │      DHT11      │
     └────────┬────────┘                 └─────────────────┘
              │
              ▼
       ┌─────────────┐
       │ Water Pump  │
       └─────────────┘

                    Arduino UNO
                         │
              ┌──────────┴──────────┐
              ▼                     ▼
       ┌─────────────┐       ┌─────────────┐
       │  I2C LCD    │       │   SIM800L   │
       │   Display   │       │ GSM Module  │
       └─────────────┘       └──────┬──────┘
                                    │
                                    ▼
                              SMS Notification
```

## Working Principle

1. The soil moisture sensor continuously monitors the moisture level of the soil.
2. The Arduino UNO reads and processes the sensor data.
3. Based on the configured moisture threshold, the controller determines whether irrigation is required.
4. When irrigation is required, the Arduino activates the relay to operate the water pump.
5. When the required moisture level is reached, the controller switches the pump OFF.
6. The DHT11 sensor provides temperature and humidity measurements.
7. The I2C LCD displays relevant system and environmental information.
8. The SIM800L GSM module sends SMS notifications regarding pump operation and status.

## Project Objectives

* Automate the irrigation process
* Reduce unnecessary water usage
* Minimize manual pump operation
* Monitor environmental conditions
* Provide remote pump-status notifications
* Implement an embedded control system using Arduino

## Project Structure

```text
Smart-Irrigation-System/
│
├── .vscode/
│   ├── c_cpp_properties.json
│   ├── launch.json
│   └── settings.json
│
├── smart_irrigation.ino
│
└── README.md
```

## Future Improvements

The project can be further developed with additional hardware and software capabilities, including:

* Custom PCB design and fabrication
* IoT-based remote monitoring
* Web or mobile dashboard
* Cloud-based sensor data logging
* Remote pump control
* Weather-based irrigation decisions
* Improved power management
* Additional pump safety mechanisms

## Applications

This system can be adapted for:

* Small-scale agricultural irrigation
* Home gardens
* Greenhouses
* Nursery automation
* Water-management systems

## Skills Demonstrated

This project demonstrates practical experience in:

* Embedded C/C++ programming
* Arduino-based system development
* Sensor interfacing
* GSM communication
* I2C communication
* Relay and actuator control
* Embedded system integration
* Hardware-software interfacing

## Author

**Sanketh Deshmukh**

Embedded Systems | Arduino | C/C++ | IoT

---

If you find this project useful or have suggestions for improvement, feel free to explore the repository.
