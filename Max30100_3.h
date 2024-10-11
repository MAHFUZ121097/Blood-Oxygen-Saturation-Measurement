#ifndef MAX30100_H
#define MAX30100_H

#include <stdint.h>

#define MAX30100_TWI_INSTANCE      0
#define MAX30100_I2C_ADDR           0x57

#define MAX30100_MODE_CONFIG        0x06
#define MAX30100_LED_MODE_CONFIG        0x09
#define MAX30100_MODE_SPO2_HR       0x03

#define MAX30100_SPO2_CONFIG        0x07
#define MAX30100_SPO2_HI_RES_EN     (1 << 6)
#define MAX30100_SPO2_SAMPLE_RATE_100HZ  (1 << 2)
#define MAX30100_SPO2_SAMPLE_RATE_400HZ  (1 << 4)
#define MAX30100_TEMP_EN  (1 << 3)

#define MAX30100_LED_PW_1600U  (3 << 0)
#define MAX30100_LED_PW_400U  0x01

#define MAX30100_LED1_PULSE_AMP     0x0C
#define MAX30100_LED_CURR_50MA      0x24
#define MAX30100_LED_CURR_0MA       0x00

#define MAX30100_LED2_PULSE_AMP     0x0D

#define MAX30100_IR_LED_CURR_50m    0x0F
#define MAX30100_RED_LED_CURR_27_1m    0x80
#define MAX30100_RED_LED_CURR_50_0m    0xF0

#define MAX30100_MULTI_LED_CONFIG1  0x11
#define MAX30100_SLOT1_ENABLE       (1 << 4)
#define MAX30100_SLOT2_DISABLE      0x00
#define MAX30100_SLOT3_DISABLE      0x00
#define MAX30100_SLOT4_DISABLE      0x00

#define MAX30100_FIFO_DATA          0x05
#define MAX30100_TEMP_DATA          0x16

void delay_ms(uint32_t ms);
void twi_init(void);
void max30100_write_reg(uint8_t reg_addr, uint8_t data);
void max30100_read_reg(uint8_t reg_addr, uint8_t *data, uint8_t length);
void max30100_init(void);
//void max30100_read_sensor_data(uint16_t *ir_data, uint16_t *red_data, int8_t *temp_data);
void max30100_read_sensor_data(uint16_t *ir_data, uint16_t *red_data);
void max30100_read_data(uint8_t *sensor_data);
#endif  // MAX30100_H
