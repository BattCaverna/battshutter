#ifndef CFG_H
#define CFG_H

#define MODBUS_ON 1
#define MODBUS_BAUDRATE 9600

//Modbus address
#define ADDR0 3 // D3
#define ADDR1 4 // D3
#define ADDR2 5 // D3
#define BASE_ADDR 0x10 // Start address when pin address=0

#define LED_COIL_REG 0x00
#define ENC_HOLD_REG 0x00
#define TARGET_HOLD_REG 0x00

// Switches
#define UP_SWITCH   A2
#define DOWN_SWITCH A1

// RS485
#define DE_RS485 A0
#define RE_RS485 A3

// Encoder
#define ENC_PIN 2 //D2

//Motor
#define MOTOR_PWR 9 // D9
#define MOTOR_DIR 8 // D8
#define GUARD_TIME 500 //ms


#endif
