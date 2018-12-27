/*
 * main.c
 */

#include <stdint.h>
#include <stdbool.h>

#include "driverlib/sysctl.h"
#include "driverlib/i2c.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"

#include "inc/hw_memmap.h"


#define COM I2C0_BASE

#define I2C_READ true
#define I2C_WRITE false

/* Setup to communicate with DS1307
 * "The DS1307 operates in the standard mode (100kHz) only"
 * "7-bit DS1307 address, which is 1101000"
 *
 * I2C0 is on Port B
 * PB2: I2C0SCL
 * PB3: I2c0SDA
 */

#define DS1307_ADDR 0x68 // the device i2c address

// registers in memory
#define DS1307_SEC  0x00
#define DS1307_MIN  0x01
#define DS1307_HOUR 0x02
#define DS1307_DAY  0x03
#define DS1307_DATE 0x04
#define DS1307_MON  0x05
#define DS1307_YEAR 0x06
#define DS1307_CTRL 0x07
#define DS1307_RAM  0x08
#define DS1307_MAX  0x3f

#define DS1307_DATA_CNT 7


uint8_t time[DS1307_DATA_CNT];


// simple dec2bcd conversion
static inline uint8_t bin2bcd(uint8_t val) {
   return ( (val / 10 * 16) + (val % 10) );
}
// simple bcd2bin conversion
static inline uint8_t bcd2bin(uint8_t val) {
   return ( (val / 16 * 10) + (val % 16) );
}

static inline void delay(volatile int time){
	while(time--);
}


 static void DS1307write(uint32_t i2c, uint8_t addr, const uint8_t *data, uint8_t cnt){
	int i;

	// address
	I2CMasterSlaveAddrSet(i2c, DS1307_ADDR, I2C_WRITE);
	//delay(100);
	I2CMasterDataPut(i2c, addr);
	//delay(100);
	while(I2CMasterBusBusy(i2c)); // wait for the bus
	//delay(100);
	I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_START);
	delay(100);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(100);

	// data
	for(i=0; i<cnt-1; i++){ // all but the last one
		I2CMasterDataPut(i2c, data[i]);
		//delay(100);
		I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_CONT);
		delay(100);
		while(I2CMasterBusy(i2c)); // wait for the bus
		delay(100);
	}

	// finish data
	I2CMasterDataPut(i2c, data[i]);
	//delay(100);
	I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_FINISH);
	delay(100);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(100);
}


static void DS1307read(uint32_t i2c, uint8_t addr, uint8_t *data, uint8_t cnt){
	int i;

	// address
	I2CMasterSlaveAddrSet(i2c, DS1307_ADDR, I2C_WRITE);
	//delay(100);
	I2CMasterDataPut(i2c, addr);
	//delay(100);
	while(I2CMasterBusBusy(i2c)); // wait for the bus
	//delay(100);
	I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_START); // leave open for repeated start
	delay(100);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(100);

	// data
	I2CMasterSlaveAddrSet(i2c, DS1307_ADDR, I2C_READ);
	//delay(100);

	if(cnt==1){
		I2CMasterControl(i2c, I2C_MASTER_CMD_SINGLE_RECEIVE);
		delay(100);
		while(I2CMasterBusy(i2c));
		delay(100);
		data[0] = I2CMasterDataGet(i2c);
		//delay(100);
		return;
	}

	// first byte
	I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_RECEIVE_START);
	delay(100);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(100);
	data[0] = I2CMasterDataGet(i2c);
	//delay(100);

	for(i=1; i<cnt-1; i++){ // all but the last one
		I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
		delay(100);
		while(I2CMasterBusy(i2c));
		delay(100);
		data[i] = I2CMasterDataGet(i2c);
		//delay(100);
	}

	// last byte
	I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
	delay(100);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(100);
	data[i] = I2CMasterDataGet(i2c);
	delay(100);
}


static inline stopClk(uint32_t i2c){
	const uint8_t n = 0xff; // overwrite the seconds

	DS1307write(i2c, DS1307_SEC, &n, 1);
}

static inline void startClk(uint32_t i2c){
	uint8_t sec;

	DS1307read(i2c, DS1307_SEC, &sec, 1);
	sec &= 0x7f; // clear the CH bit
	DS1307write(i2c, DS1307_SEC, &sec, 1);
}

static bool clkRunning(uint32_t i2c){
	uint8_t sec;

	DS1307read(i2c, DS1307_SEC, &sec, 1);
	return !(sec&0x80);
}


static void setTime(
		uint32_t i2c, uint8_t sec , uint8_t min, uint8_t hour,
		uint8_t day, uint8_t date, uint8_t mon, uint8_t year
){
	uint8_t data[7];


	data[DS1307_SEC ] = bin2bcd(sec) &0x7f; // mask the enable bit
	data[DS1307_MIN ] = bin2bcd(min) &0x7f; // bit-7 is 0
	data[DS1307_HOUR] = bin2bcd(hour)&0x7f; // mask the 0 bit
	data[DS1307_DAY ] = bin2bcd(day) &0x07; // mask the 0 bits
	data[DS1307_DATE] = bin2bcd(date)&0x3f; // mask the 0 bits
	data[DS1307_MON ] = bin2bcd(mon) &0x1f; // mask the 0 bits
	data[DS1307_YEAR] = bin2bcd(year)     ;

	DS1307write(COM, DS1307_SEC, data, 7);
}

static void getTime(uint32_t i2c, uint8_t * data){
	int i;

	DS1307read(i2c, DS1307_SEC, data, DS1307_DATA_CNT);
	for(i=0; i<DS1307_DATA_CNT; i++)
		data[i] = bcd2bin(data[i]);

}

int main(void) {
	uint32_t sysClkFreq, i;

	sysClkFreq = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
			SYSCTL_USE_OSC), 16000000);

	if(!sysClkFreq) while(1); // check the clock setting

	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0));
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

	I2CMasterInitExpClk(COM, sysClkFreq, false);

	GPIOPinConfigure(GPIO_PB3_I2C0SDA);
	GPIOPinConfigure(GPIO_PB2_I2C0SCL);

	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

	// stop the clock
	while(clkRunning(COM)){
		stopClk(COM);
	}

	getTime(COM, time);
	setTime(COM, 0, 27, 1, 3, 12, 12, 18);
	getTime(COM, time);
	startClk(COM);

	while(1){
		getTime(COM, time);
		for(i=0; i<100000; i++); // delay
	}
}
