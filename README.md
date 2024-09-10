[![Doxygen Action](https://github.com/SMotlaq/ina234/actions/workflows/main2.yml/badge.svg)](https://github.com/SMotlaq/ina234/actions/workflows/main2.yml)
[![pages-build-deployment](https://github.com/SMotlaq/ina234/actions/workflows/pages/pages-build-deployment/badge.svg)](https://github.com/SMotlaq/ina234/actions/workflows/pages/pages-build-deployment)


# INA234 HAL Based Library

The INA234 device is a 12-bit digital current monitor with an I2C/SMBus-compatible interface that is compliant with a wide range of digital bus voltages such as 1.2 V, 1.8 V, 3.3 V, and 5.0 V. The device monitors the voltage across an external sense resistor and reports values for current, bus voltage, and power. ([Click for more info](https://www.ti.com/product/INA234))

This library is a software library that works with the INA234 current, voltage, and power monitor chip. This library provides a convenient and efficient way to access the I2C interfaces of the chip, allowing developers to easily integrate this power meter into their systems.

The library is designed to be easy to use and provides a simple, intuitive API for accessing the I2C interfaces of the INA234. It includes a range of functions for performing common I2C operations, such as sending and receiving data, querying the status of the chip, reading the measured parameters, and configuring the INA234 settings.

With this library, developers can quickly and easily integrate the INA234 into their systems, enabling them to take full advantage of the chip's capabilities.

## Key Features

* Easy-to-use API for accessing the I2C interfaces of the INA234
* Support for common I2C operations, such as sending and receiving data, querying the status of the chip, reading the measured parameters, and configuring the INA234 settings
* Full feature library

## Documentations

The full documents are available [here](https://smotlaq.github.io/ina234/)

## Donate
Is it helpfull?

<p align="left">
  <a href="http://smotlaq.ir/LQgQF">
  <img src="https://raw.githubusercontent.com/SMotlaq/LoRa/master/bmc.png" width="200" alt="Buy me a Coffee"/>
  </a>
</p>

# Getting Started

## Quick Start

1. Downoad the library source from the [latest release](http://github.com/smotlaq/ina234/releases/latest)
2. Copy `ina234.c` and `ina234.h` file to your project directory and add them to your IDE if necessary.
3. Inclued the library into your project:
   ```C
   #include "ina234.h"
   ```
4. Create an object (instanse) from INA234 struct with desired name:
   ```C
   INA234 ina234;
   ```
5. Initialize the chip:
   ```C
   INA234_init(&ina234, 0x48, &hi2c1, 1, RANGE_20_48mV, NADC_16, CTIME_1100us, CTIME_140us, MODE_CONTINUOUS_BOTH_SHUNT_BUS);
   ```
   Each argument is described on the [doc page](https://smotlaq.github.io/ina234/ina234_8c.html#a21acc30b187445a98f711a54b8678c7c).
6. Now you can call `INA234_readAll` function to read the meassured data:
   ```C
   INA234_readAll(&ina234);
   shunt_voltage = ina234.ShuntVoltage;
   bus_voltage = ina234.BusVoltage;
   current = ina234.Current;
   power = ina234.Power;
   ```

Here is the whole code:
```C
#include "ina234.h"

INA234 ina234;
float shunt_voltage, bus_voltage, current, power;

if(STATUS_OK == INA234_init(&ina234, 0x48, &hi2c1, 1, RANGE_20_48mV, NADC_16, CTIME_1100us, CTIME_140us, MODE_CONTINUOUS_BOTH_SHUNT_BUS)){

  INA234_readAll(&ina234);
  shunt_voltage = ina234.ShuntVoltage;
  bus_voltage = ina234.BusVoltage;
  current = ina234.Current;
  power = ina234.Power;
}
```
