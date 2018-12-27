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

#define DS1307_ADDR 0x68
#define DS1307_SEC  0x00
#define DS1307_MIN  0x01
#define DS1307_HR   0x02
#define DS1307_DAY  0x03
#define DS1307_DATE 0x04
#define DS1307_MON  0x05
#define DS1307_YR   0x06
#define DS1307_CTRL 0x07
#define DS1307_RAM  0x08
#define DS1307_MAX  0x3f
#define DS1307_DATA_CNT 7


volatile uint8_t time[3];


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
	I2CMasterDataPut(i2c, addr);
	while(I2CMasterBusBusy(i2c)); // wait for the bus
	I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_START);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(10);

	// data
	for(i=0; i<cnt-1; i++){ // all but the last one
		I2CMasterDataPut(i2c, data[i]);
		I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_CONT);
		while(I2CMasterBusy(i2c)); // wait for the bus
		delay(10);
	}

	// finish data
	I2CMasterDataPut(i2c, data[i]);
	I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_FINISH);
	while(I2CMasterBusy(i2c)); // wait for the bus
}

static void DS1307read(uint32_t i2c, uint8_t addr, uint8_t *data, uint8_t cnt){
	int i;

	// address
	I2CMasterSlaveAddrSet(i2c, DS1307_ADDR, I2C_WRITE);
	delay(100);
	I2CMasterDataPut(i2c, addr);
	delay(100);
	while(I2CMasterBusBusy(i2c)); // wait for the bus
	delay(100);
	I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_START); // leave open for repeated start
	delay(100);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(100);

	// data
	I2CMasterSlaveAddrSet(i2c, DS1307_ADDR, I2C_READ);
	delay(100);

	if(cnt==1){
		I2CMasterControl(i2c, I2C_MASTER_CMD_SINGLE_RECEIVE);
		delay(100);
		while(I2CMasterBusy(i2c));
		delay(100);
		data[0] = I2CMasterDataGet(i2c);
		delay(100);
		return;
	}

	// first byte
	I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_RECEIVE_START);
	delay(100);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(100);
	data[0] = I2CMasterDataGet(i2c);
	delay(100);

	for(i=1; i<cnt-1; i++){ // all but the last one
		I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
		delay(100);
		while(I2CMasterBusy(i2c));
		delay(100);
		data[i] = I2CMasterDataGet(i2c);
		delay(100);
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
	const uint8_t n = 0xff;

	DS1307write(i2c, DS1307_SEC, &n, 1);
}


static inline void startClk(uint32_t i2c){
	int err;
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

/*
static inline int
stopClk(uint32_t i2c){
	int err;

	if((err=updatePointer(i2c, DS1307_ADDR, DS1307_SEC))){ // set to seconds register
		I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return err;
	}

	I2CMasterDataPut(i2c, 0xff); // set all bits
	I2CMasterControl(i2c, I2C_MASTER_CMD_SINGLE_SEND);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(10);
	err=I2CMasterErr(i2c);

	return err;
}

static inline int
startClk(uint32_t i2c){
	int err;
	uint8_t sec;

	if((err=updatePointer(i2c, DS1307_ADDR, DS1307_SEC))){ // set to seconds register
		I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return err;
	}

	// get the current seconds
	I2CMasterControl(i2c, I2C_MASTER_CMD_SINGLE_RECEIVE);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(10);
	if((err=I2CMasterErr(i2c)))
		return err;
	sec = I2CMasterDataGet(i2c);

	sec &= 0x7f; // clear the CH bit

	if((err=updatePointer(i2c, DS1307_ADDR, DS1307_SEC))){ // set to seconds register
		I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return err;
	}

	I2CMasterDataPut(i2c, sec);
	I2CMasterControl(i2c, I2C_MASTER_CMD_SINGLE_SEND);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(10);
	err=I2CMasterErr(i2c);

	return err;
}


static bool clkRunning(uint32_t i2c){
	uint8_t sec;

	updatePointer(i2c, DS1307_ADDR, DS1307_SEC);
	delay(10);

	I2CMasterSlaveAddrSet(i2c, DS1307_ADDR, I2C_READ);
	delay(10);
	I2CMasterControl(i2c, I2C_MASTER_CMD_SINGLE_RECEIVE);
	delay(10);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(10);

	sec = I2CMasterDataGet(i2c);

	if(sec&0x80) return false;
	else return true;
}
*/


// write to the pointer and don't STOP
static inline int
updatePointer(uint32_t i2c, uint8_t slvAddr, uint8_t addr){
	uint32_t err;

	while(I2CMasterBusBusy(i2c));
	I2CMasterSlaveAddrSet(i2c, slvAddr, I2C_WRITE);

	// device address
	I2CMasterDataPut(i2c, addr);
	I2CMasterControl(i2c, I2C_MASTER_CMD_SINGLE_SEND);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(10);
	if( (err=I2CMasterErr(i2c)) != I2C_MASTER_ERR_NONE)
		I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);

	return err;
}


// read all the data registers
static int readDS1307(uint32_t i2c, uint8_t *data_pt){
	uint32_t temp;


	// set the word pointer
	if( (temp=updatePointer(i2c, DS1307_ADDR, DS1307_SEC)) != 0)
		return temp;

	delay(100);

	I2CMasterSlaveAddrSet(i2c, DS1307_ADDR, I2C_READ);
	I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_RECEIVE_START);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(10);
	if( (temp=I2CMasterErr(i2c)) != I2C_MASTER_ERR_NONE){
		I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return temp;
	}


	// get data
	int i;
	for(i=0; i<DS1307_DATA_CNT; i++){
		I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
		while(I2CMasterBusy(i2c)); // wait for the bus
		delay(10);
		if( (temp=I2CMasterErr(i2c)) != I2C_MASTER_ERR_NONE){
			I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
			return temp;
		}
		data_pt[i] = I2CMasterDataGet(i2c);
	}

	// finish the read
	I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
	while(I2CMasterBusBusy(i2c)); // wait for the bus
	delay(10);
	if( (temp=I2CMasterErr(i2c)) != I2C_MASTER_ERR_NONE){
		I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
	}

	return temp;
}

static int
writeDS1307(
		uint32_t i2c, uint8_t sec , uint8_t min, uint8_t hour,
		uint8_t day, uint8_t date, uint8_t mon, uint8_t year
){
	int err;

	// set the pointer
	if((err=updatePointer(i2c, DS1307_ADDR, DS1307_SEC))){
		I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return err;
	}

	// Seconds
	I2CMasterDataPut(i2c, sec&0x7f); // mask the enable bit
	I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_CONT);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(10);
	if( (err=I2CMasterErr(i2c)) ){
		I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return err;
	}

	// Minutes
	I2CMasterDataPut(i2c, min&0x7f); // bit-7 is 0
	I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_CONT);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(10);
	if( (err=I2CMasterErr(i2c)) ){
		I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return err;
	}

	// Hours
	I2CMasterDataPut(i2c, hour&0x7f); // mask the 0 bit
	I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_CONT);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(10);
	if( (err=I2CMasterErr(i2c)) ){
		I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return err;
	}

	// Day of week
	I2CMasterDataPut(i2c, day&0x07); // mask the 0 bits
	I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_CONT);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(10);
	if( (err=I2CMasterErr(i2c)) ){
		I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return err;
	}

	// Date
	I2CMasterDataPut(i2c, date&0x3f); // mask the 0 bits
	I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_CONT);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(10);
	if( (err=I2CMasterErr(i2c)) ){
		I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return err;
	}

	// Month
	I2CMasterDataPut(i2c, mon&0x1f); // mask the 0 bits
	I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_CONT);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(10);
	if( (err=I2CMasterErr(i2c)) ){
		I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return err;
	}

	// Year
	I2CMasterDataPut(i2c, year); // mask the 0 bits
	I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_FINISH);
	while(I2CMasterBusy(i2c)); // wait for the bus
	delay(10);
	if( (err=I2CMasterErr(i2c)) ){
		I2CMasterControl(i2c, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return err;
	}

	return 0;
}

int main(void) {
	uint32_t sysClkFreq, i;
	uint8_t rtcData[DS1307_DATA_CNT];
	int temp;

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

	for(i=0; i<DS1307_DATA_CNT; i++) rtcData[i] =0;

	// stop the clock
	while(clkRunning(COM)){
		stopClk(COM);
		//delay(1000);
	}


	temp=readDS1307(COM, rtcData);

	writeDS1307(COM,
			bin2bcd(0), bin2bcd(0), bin2bcd(0), bin2bcd(2),
			bin2bcd(11), bin2bcd(12), bin2bcd(18)
	);

	temp=readDS1307(COM, rtcData);

	startClk(COM);

	while(1){
		DS1307read(COM, DS1307_SEC, time,3);

		for(i=0; i<100000; i++) // delay
			;
	}




}
