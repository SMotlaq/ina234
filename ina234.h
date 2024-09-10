/*!
 * @file ina234.h
 *
 * @mainpage INA234 current/voltage/power monitor library
 *
 * @section intro_sec Introduction
 *
 * The INA234 device is a 12-bit digital current monitor with an I2C/SMBus-compatible interface that is
 * compliant with a wide range of digital bus voltages such as 1.2 V, 1.8 V, 3.3 V, and 5.0 V. The device
 * monitors the voltage across an external sense resistor and reports values for current, bus voltage, and
 * power.
 * 
 * The INA234 features programmable ADC conversion times and averaging. The device also has a
 * programmable calibration value with an internal multiplier that enables direct readouts of current in
 * amperes and power in watts. The device monitors the bus voltage present on the IN- pin and can
 * alert on overcurent and undercurrent conditions as well as overvoltage and undervoltage conditions. High
 * input impedance while in current measurement mode allows use of larger current sense resistors needed to
 * measure small value system currents.
 * 
 * The INA234 senses current on common-mode bus
 * voltages that can vary from -0.3 V to 28 V,
 * independent of the supply voltage. The device
 * operates from a single 1.7-V to 5.5-V supply, drawing
 * a typical supply current of 300 uA in normal operation.
 * The device can be placed in a low-power standby
 * mode where the typical operating current is 2.2 uA.
 *
 * @section author Author
 *
 * Written by Salman Motlaq (<a href="https://github.com/SMotlaq">@SMotlaq</a> on Github)
 *
 * @section license License
 *
 * MIT License
 *
 * @section start Getting Started
 * Go to [my Github page](https://github.com/SMotlaq) to get started. You can also see the [functions list](./ina234_8c.html) to learn more.
 *
 */

#ifndef __INA234_H_
#define __INA234_H_

#include "main.h"
#include "i2c.h"

#define MAXIMUM_EXPECTED_CURRENT	5.0
#define CURRENT_LSB_MINIMUM				(MAXIMUM_EXPECTED_CURRENT / 2048.0)
#define CURRENT_LSB								(CURRENT_LSB_MINIMUM * 1.0) // in A
#define BUS_VOLTAGE_LSB						0.025 // in V
#define SHUNT_VOLTAGE_81_92mv_LSB	0.04  // in mV		
#define SHUNT_VOLTAGE_20_48mv_LSB	0.01  // in mV
#define POWER_LSB									(CURRENT_LSB*0.032) // in W

#define CONFIGURATION_REGISTER	0x00
#define SHUNT_VOLTAGE_REGISTER	0x01
#define BUS_VOLTAGE_REGISTER		0x02
#define POWER_REGISTER					0x03
#define CURRENT_REGISTER				0x04
#define CALIBRATION_REGISTER		0x05
#define MASK_ENABLE_REGISTER		0x06
#define ALERT_LIMIT_REGISTER		0x07
#define MANUFACTURERID_REGISTER	0x3E
#define DEVICEID_REGISTER				0x3F

typedef enum ADCRange				{RANGE_81_92mV, RANGE_20_48mV} ADCRange;
typedef enum NumSamples			{NADC_1, NADC_4, NADC_16, NADC_64, NADC_128, NADC_256, NADC_512, NADC_1024} NumSamples;
typedef enum ConvTime				{CTIME_140us, CTIME_204us, CTIME_332us, CTIME_588us, CTIME_1100us, CTIME_2116us, CTIME_4156us, CTIME_8244us} ConvTime;
typedef enum Mode						{MODE_SHUTDOWN, MODE_SINGLESHOT_SUNT, MODE_SINGLESHOT_BUS, MODE_SINGLESHOT_BOTH_SHUNT_BUS, MODE_SHUTDOWN2, MODE_CONTINUOUS_SHUNT, MODE_CONTINUOUS_BUS, MODE_CONTINUOUS_BOTH_SHUNT_BUS} Mode;
typedef enum Status					{STATUS_OK, STATUS_TimeOut} Status;
typedef enum AlertOn				{ALERT_NONE, ALERT_SHUNT_OVER_LIMIT, ALERT_SHUNT_UNDER_LIMIT, ALERT_BUS_OVER_LIMIT, ALERT_BUS_UNDER_LIMIT, ALERT_POWER_OVER_LIMIT} AlertOn;
typedef enum AlertPolarity	{ALERT_ACTIVE_LOW, ALERT_ACTIVE_HIGH} AlertPolarity;
typedef enum AlertLatch			{ALERT_TRANSPARENT, ALERT_LATCHED} AlertLatch;
typedef enum AlertConvReady	{ALERT_CONV_DISABLE, ALERT_CONV_ENABLE} AlertConvReady;
typedef enum AlertSource		{ALERT_DATA_READY, ALERT_LIMIT_REACHED} AlertSource;
typedef enum ErrorType			{ERROR_NONE, ERROR_MEMORY, ERROR_OVF, ERROR_BOTH_MEMORY_OVF} ErrorType;

/*! 
    @brief  Class (struct) that stores variables for interacting with INA234
*/
typedef struct ina234{
	
	I2C_HandleTypeDef*	hi2c;										/*!< Specifies the I2C handler. */
	uint8_t 						I2C_ADDR;
	
	// Main configs
	ADCRange		adc_range;
	NumSamples	number_of_adc_samples;
	ConvTime		vbus_conversion_time;
	ConvTime		vshunt_conversion_time;
	Mode				mode;
	float				ShuntResistor;
	
	// Alert Configs
	AlertOn					alert_on;
	AlertPolarity		alert_polarity;
	AlertLatch			alert_latch;
	AlertConvReady	alert_conv_ready;
	float						alert_limit;
	int32_t 				alert_limit_int;

	float				ShuntVoltage;
	float				BusVoltage;
	float				Power;
	float				Current;
	
	union _reg {
		uint8_t raw_data[2];
		
		struct _config_register{
			uint16_t MODE:3, VSHCT:3, VBUSCT:3, AVG:3, ACDRANGE:1, RESERVED:2, RST:1;
		} config_register;
		
		struct _shunt_voltage_register{
			int16_t RESERVED:4, VSHUNT:12;
		} shunt_voltage_register;
		
		struct _bus_voltage_register{
			uint16_t RESERVED:4, VBUS:11, RESERVED2:1;
		} bus_voltage_register;
		
		struct _power_register{
			uint16_t POWER;
		} power_register;
		
		struct _current_register{
			int16_t RESERVED:4, CURRENT:12;
		} current_register;
		
		struct _calibration_register{
			uint16_t SHUNT_CAL:15, RESERVED:1;
		} calibration_register;
		
		struct _mask_enable_register{
			uint16_t LEN:1, APOL:1, OVF:1, CVRF:1, AFF:1, MemError:1, RESERVED:4, CNVR:1, POL:1, BUL:1, BOL:1, SUL:1, SOL:1;
		} mask_enable_register;
		
		struct _alert_limit_register{
			int16_t LIMIT;
		} alert_limit_register;
			
		struct _manufacture_id_register{
			uint16_t MANUFACTURE_ID;
		} manufacture_id_register;
		
		struct _devide_id_register{
			uint16_t RESERVED:4, DIEID:12;
		} devide_id_register;
		
	} reg;

} INA234;

Status INA234_init(INA234* self, uint8_t I2C_ADDR, I2C_HandleTypeDef* hi2c, float ShuntResistor, ADCRange adc_range, NumSamples numer_of_adc_samples, ConvTime vbus_conversion_time, ConvTime vshunt_conversion_time, Mode mode);
Status INA234_alert_init(INA234* self, AlertOn alert_on, AlertPolarity alert_polarity, AlertLatch alert_latch, AlertConvReady alert_conv_ready, float alert_limit);

// Privates ----------------------------------

Status __INA234_readTwoBytes(INA234* self, uint8_t MemAddress);
Status __INA234_writeTwoBytes(INA234* self, uint8_t MemAddress);

// Configurations ----------------------------

Status INA234_setADCRange(INA234* self, ADCRange adc_range);
Status INA234_setNumberOfADCSamples(INA234* self, NumSamples numer_of_adc_samples);
Status INA234_setVBusConversionTime(INA234* self, ConvTime vbus_conversion_time);
Status INA234_setVShuntConversionTime(INA234* self, ConvTime vshunt_conversion_time);
Status INA234_setMode(INA234* self, Mode mode);

ADCRange		INA234_getADCRange(INA234* self);
NumSamples	INA234_getNumberOfADCSamples(INA234* self);
ConvTime		INA234_getVBusConversionTime(INA234* self);
ConvTime		INA234_getVShuntConversionTime(INA234* self);
Mode 				INA234_getMode(INA234* self);

void INA234_SoftResetAll(INA234* self);

// Getting Data ------------------------------

uint16_t	INA234_getManID(INA234* self);
uint16_t	INA234_getDevID(INA234* self);
void			INA234_readAll(INA234* self);
float			INA234_getCurrent(INA234* self);
float			INA234_getBusVoltage(INA234* self);
float			INA234_getShuntVoltage(INA234* self);
float			INA234_getPower(INA234* self);

uint8_t			INA234_isDataReady(INA234* self);
AlertSource	INA234_getAlertSource(INA234* self);
ErrorType		INA234_getErrors(INA234* self);
Status			INA234_resetAlert(INA234* self);

#endif
