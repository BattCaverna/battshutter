#ifndef CFG_H
#define CFG_H

#define MODBUS_ON 1
#define MODBUS_BAUDRATE 9600

//Modbus address
#define ADDR0 3 // D3
#define ADDR1 4 // D3
#define ADDR2 5 // D3
#define BASE_ADDR 0x10 // Mobbus start address when pin address=0

#define LED_COIL_REG 0x00

// Modbus holding registers
#define START_REG 0x00

enum HoldingRegs
{
  ENC_HOLD_REG = START_REG, // Encoder: current position (%)
  ENCMAX_HOLD_REG, // Encoder: max position (steps)
  TARGET_HOLD_REG, // Target position to reach (%)
  HOLD_REG_END
};

#define HOLD_REG_CNT (HOLD_REG_END - START_REG)

// Switches
#define UP_SWITCH   A2
#define DOWN_SWITCH A1
// If we keep pressing the down button when we reach home, a reset
#define RESET_ENC_TIME 5000 //ms

// RS485
#define DE_RS485 A0
#define RE_RS485 A3

// Encoder
#define ENC_PIN 2 //D2
// Position tolerance when reaching target
#define ENC_HYST 0
// Max time between two encoder edges, used to detect when we reach the home position
#define MAX_ENC_PERIOD 1000 //ms


//Motor
#define MOTOR_PWR 9 // D9
#define MOTOR_DIR 8 // D8
// Time before switching on the mains when changing direction
#define GUARD_TIME 500 //ms


#endif
