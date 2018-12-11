#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/i2c.h"
/*
 * Final Project TCES 430
 * Part 2 implemented by Jake McKenzie
 */
extern bool I2CMasterBusy(uint32_t ui32Base);
extern void I2CMasterControl(uint32_t ui32Base, uint32_t ui32Cmd);
extern void I2CMasterDataPut(uint32_t ui32Base, uint8_t ui8Data);
extern uint32_t I2CMasterErr(uint32_t ui32Base);
extern void I2CMasterSlaveAddrSet(uint32_t ui32Base, uint8_t ui8SlaveAddr, bool bReceive);
unsigned char dec2bcd(unsigned char val);
unsigned char bcd2dec(unsigned char c);
void delay(uint32_t delayValue);
void setupUART(void);

uint8_t I2C_WriteByte(int I2C_Num, uint8_t devAddr, uint8_t redAddr, uint8_t wrtData);
uint8_t I2C_ReadByte(int I2C_Num, uint8_t devAddr, uint8_t regAddr);
uint32_t ui32SysClkFreq;
uint32_t clock;
uint32_t sec = 0;
uint32_t min = 0;
uint32_t time_sec = 0;
uint32_t time_min = 0;


int main(void) {
    // need some way of keeping track of total time
    unsigned char total_time = 1;
    //setting up the clock
    ui32SysClkFreq = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
    //
    clock = SysCtlClockGet();
    // enable I2C0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    // enable GPIOA peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    // pg 744 of the tm4c1294 data sheet (2k pages)
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    // configure pin 3 for I2C
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
    // pg 744 of the tm4c1294 data sheet (2k pages)
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    // configure pin 2 for I2CSCL
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    // initializes I2C master block
    I2CMasterInitExpClk(I2C0_BASE, ui32SysClkFreq, false);
    // sets up UART
//    setupUART();
    // slave address with a direction on the bus
    // slave addr may be 0x68?
    I2CMasterSlaveAddrSet(I2C0_BASE, 0x00 , false);
    // Transmits the char(byte) from the I2C Master.
    I2CMasterDataPut(I2C0_BASE, 0);
    // start the transmission and send data
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    // If I2C master is busy : WAIT
    while(I2CMasterBusy(I2C0_BASE));
    // Transmits the char(byte) from the I2C Master.
    I2CMasterDataPut(I2C0_BASE, dec2bcd(sec));
    // continue the transmission and send data
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
    // If I2C master is busy : WAIT
    while(I2CMasterBusy(I2C0_BASE));
    // Transmits the char(byte) from the I2C Master.
    I2CMasterDataPut(I2C0_BASE, dec2bcd(min));
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    // If I2C master is busy : WAIT
    while(I2CMasterBusy(I2C0_BASE));

    while(1) {
        // slave address with a direction on the bus
        // slave addr may be 0x68?
        I2CMasterSlaveAddrSet(I2C0_BASE, 0x00 , false);
        // Start the transmission and send data
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
        // Transmits the char(byte) from the I2C Master.
        // slave addr may be 0x68?
        I2CMasterDataPut(I2C0_BASE, 0x00);
        // end the transmission and send data
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
        // If I2C master is busy : WAIT
        while(I2CMasterBusy(I2C0_BASE));
        delay(0xF);
        // slave address with a direction on the bus
        // slave addr may be 0x68?
        I2CMasterSlaveAddrSet(I2C0_BASE, 0x00 , true);
        // Start the transmission and send data
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
        // If I2C master is busy : WAIT
        while(I2CMasterBusy(I2C0_BASE));
        delay(0xF);
        // grab seconds
        sec = I2CMasterDataGet(I2C0_BASE);
        // continue the transmission and send data
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
        // If I2C master is busy : WAIT
        while(I2CMasterBusy(I2C0_BASE));
        delay(0xF);
        min = I2CMasterDataGet(I2C0_BASE);
        // end the transmission and send data
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
        // If I2C master is busy : WAIT
        while(I2CMasterBusy(I2C0_BASE));
        delay(0xF);
        // convert seconds from decimal to bcd
        time_sec = bcd2dec(sec) & 0x7f;
        // convert minutes from decimal to bcd
        time_min = bcd2dec(min);

        if(total_time != time_sec) {
            // set total time to current time
            total_time = time_sec;
            // reset time in seconds
            time_sec = 0;
        }
    }
}

uint8_t I2C_WriteByte(int I2C_Num, uint8_t devAddr, uint8_t redAddr, uint8_t wrtData) {

    uint32_t I2C_base;
    uint8_t I2C_error_status;

    // Picks the I2C bus to be used
    switch(I2C_Num) {
        case 0: {
            I2C_base = I2C0_BASE;
            break;
        }
        case 1: {
            I2C_base = I2C1_BASE;
            break;
        }
        case 2: {
            I2C_base = I2C2_BASE;
            break;
        }
        case 3: {
            I2C_base = I2C3_BASE;
            break;
        }
        case 4: {
            I2C_base = I2C4_BASE;
            break;
        }
        case 5: {
            I2C_base = I2C5_BASE;
            break;
        }
        case 6: {
            I2C_base = I2C6_BASE;
            break;
        }
        case 7: {
            I2C_base = I2C7_BASE;
            break;
        }
        case 8: {
            I2C_base = I2C8_BASE;
            break;
        }
        case 9: {
            I2C_base = I2C9_BASE;
            break;
        }
    }
    // If I2C master is busy : WAIT
    while(I2CMasterBusy(I2C_base));
    // Sets the transmission to send (false) and establishes
    // slave address with a direction on the bus
    I2CMasterSlaveAddrSet(I2C_base, devAddr, false);
    // Transmits the char(byte) from the I2C Master.
    I2CMasterDataPut(I2C_base, redAddr);
    // Start the transmission and send data
    I2CMasterControl(I2C_base, I2C_MASTER_CMD_BURST_SEND_START);
    // If I2C0 master is busy: WAIT
    while(I2CMasterBusy(I2C_base));
    // small delay after the master bus is busy
    delay(0xF);
    // returns error status on I2C Master
    I2C_error_status = (uint8_t) I2CMasterErr(I2C_base);
    if(I2C_error_status) {
        // Stops the transmission
        I2CMasterControl(I2C_base, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
        // returns error status
        return I2C_error_status;
    }
    // small delay after the master bus is busy
    delay(0xF);
    // Set the data (the data to the register specified by the above command) to be sent
    I2CMasterDataPut(I2C_base, wrtData);
    // send the second byte and terminate the transmission when finished
    I2CMasterControl(I2C_base, I2C_MASTER_CMD_BURST_SEND_FINISH);
    // Wait if I2C0 master is busy
    while(I2CMasterBusy(I2C_base));
    // small delay after the master bus is busy
    delay(0xF);
    // returns error status on I2C Master
    I2C_error_status = (uint8_t) I2CMasterErr(I2C_base);
    if(I2C_error_status) {
        // Stops the transmission
        I2CMasterControl(I2C_base, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
        // returns error status
        return I2C_error_status;
    }

    return 0;
}

uint8_t I2C_ReadByte(int I2C_Num, uint8_t devAddr, uint8_t regAddr) {
    uint32_t I2C_base;
    uint8_t register_contents;
    uint8_t I2C_error_status;
    //switch I2C
    switch(I2C_Num) {
        case 0: {
            I2C_base = I2C0_BASE;
            break;
        }
        case 1: {
            I2C_base = I2C1_BASE;
            break;
        }
        case 2: {
            I2C_base = I2C2_BASE;
            break;
        }
        case 3: {
            I2C_base = I2C3_BASE;
            break;
        }
        case 4: {
            I2C_base = I2C4_BASE;
            break;
        }
        case 5: {
            I2C_base = I2C5_BASE;
            break;
        }
        case 6: {
            I2C_base = I2C6_BASE;
            break;
        }
        case 7: {
            I2C_base = I2C7_BASE;
            break;
        }
        case 8: {
            I2C_base = I2C8_BASE;
            break;
        }
        case 9: {
            I2C_base = I2C9_BASE;
            break;
        }
    }
    // If I2C0 master is busy: WAIT
    while(I2CMasterBusy(I2C_base));
    // Sets the transmission to send (false) and establishes
    // slave address with a direction on the bus
    I2CMasterSlaveAddrSet(I2C_base, devAddr, false);
    // Transmits the char(byte) from the I2C Master.
    I2CMasterDataPut(I2C_base, regAddr);
    // Start the transmission and send one byte
    I2CMasterControl(I2C_base,I2C_MASTER_CMD_SINGLE_SEND);
    // If I2C0 master is busy: WAIT
    while(I2CMasterBusBusy(I2C_base));
    delay(0x100);
    I2C_error_status = (uint8_t) I2CMasterErr(I2C_base);
    // if there was an error terminate and return error status
    if(I2C_error_status) {
        return I2C_error_status;
    }
    delay(0xF);
    // Sets the transmission to send (false) and establishes
    // slave address with a direction on the bus
    I2CMasterSlaveAddrSet(I2C_base, devAddr, true);
    // Start the transmission and read one byte
    I2CMasterControl(I2C_base,I2C_MASTER_CMD_SINGLE_RECEIVE);
    // wait if bus is busy
    while(I2CMasterBusBusy(I2C_base));
    delay(0x100);
    // capture data from the master's register
    register_contents = I2CMasterDataGet(I2C_base);
    // return registers contents
    return register_contents;
}

// generic delay function
void delay(uint32_t delayValue) {
    uint32_t ui32Loop;
    // delays for "delayValue" clock cycles
    for(ui32Loop = 0; ui32Loop < delayValue; ui32Loop++);
}
// simple dec2bcd conversion
unsigned char dec2bcd(unsigned char val) {
   return ( (val / 10 * 16) + (val % 10) );
}
// simple bcd2dec conversion
unsigned char bcd2dec(unsigned char val) {
   return ( (val / 16 * 10) + (val % 16) );
}


//void setupUART(void) {
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
//
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
//
//    GPIOPinConfigure(GPIO_PA0_U0RX);
//    GPIOPinConfigure(GPIO_PA1_U0TX);
//    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
//
//    UARTStdioConfig(0, 115200, ui32SysClkFreq);
//    IntMasterEnable();
//
//}
