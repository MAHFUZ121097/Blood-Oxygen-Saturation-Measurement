#include "nrf_drv_twi.h"
#include "app_util_platform.h"
#include "nrf_delay.h"
#include "max30100_3.h"

static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(MAX30100_TWI_INSTANCE);

// Dummy implementation for the delay function
void delay_ms(uint32_t ms) {
    nrf_delay_ms(ms);
}

// Function to initialize the TWI (I2C) peripheral
void twi_init(void) {
    const nrf_drv_twi_config_t twi_config = {
       .scl                = 28,  // SCL pin
       .sda                = 29,  // SDA pin
       .frequency          = NRF_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_LOW,
       .clear_bus_init     = false
    };

    APP_ERROR_CHECK(nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL));
    nrf_drv_twi_enable(&m_twi);
}

// Function to write data to the MAX30100 register
void max30100_write_reg(uint8_t reg_addr, uint8_t data) {
    uint8_t tx_data[2] = {reg_addr, data};
    nrf_drv_twi_tx(&m_twi, MAX30100_I2C_ADDR, tx_data, sizeof(tx_data), false);
    //delay_ms(5);  // Delay for stability, adjust as needed
}

// Function to read data from the MAX30100 register
void max30100_read_reg(uint8_t reg_addr, uint8_t *data, uint8_t length) {
    nrf_drv_twi_tx(&m_twi, MAX30100_I2C_ADDR, &reg_addr, sizeof(reg_addr), true);
   // delay_ms(5);  // Delay for stability, adjust as needed
    nrf_drv_twi_rx(&m_twi, MAX30100_I2C_ADDR, data, length);
   // delay_ms(5);  // Delay for stability, adjust as needed
}

// Function to initialize the MAX30100 sensor
void max30100_init(void) {
    // Configuration register settings
    max30100_write_reg(MAX30100_MODE_CONFIG, MAX30100_MODE_SPO2_HR);
    max30100_write_reg(MAX30100_SPO2_CONFIG, MAX30100_SPO2_HI_RES_EN | MAX30100_SPO2_SAMPLE_RATE_100HZ | MAX30100_LED_PW_1600U);

    // LED Pulse amplitude register settings
    max30100_write_reg(MAX30100_LED_MODE_CONFIG, MAX30100_RED_LED_CURR_27_1m | MAX30100_IR_LED_CURR_50m);
   // max30100_write_reg(MAX30100_LED2_PULSE_AMP, MAX30100_LED_CURR_0MA);

    // Multi-LED mode configuration register settings
   // max30100_write_reg(MAX30100_MULTI_LED_CONFIG1, MAX30100_SLOT1_ENABLE | MAX30100_SLOT2_DISABLE | MAX30100_SLOT3_DISABLE | MAX30100_SLOT4_DISABLE);

    // Set other configurations as needed
}

// Read sensor data
//void max30100_read_sensor_data(uint16_t *ir_data, uint16_t *red_data, int8_t *temp_data) {
//uint8_t data[4];
////int8_t temp_data;
//    max30100_read_reg(MAX30100_TEMP_DATA, temp_data, sizeof(data));
//     *temp_data = *temp_data<<0;
//    max30100_read_reg(MAX30100_FIFO_DATA, data, sizeof(data));
//    //max30100_read_reg(MAX30100_FIFO_DATA, red_data, 1);
//    *ir_data = (data[0]<<8) | data[1];
//    *red_data = (data[2]<<8) | data[3];
//     
//}

void max30100_read_sensor_data(uint16_t *ir_data, uint16_t *red_data) {
uint8_t data[4];
//int8_t temp_data;
//    max30100_read_reg(MAX30100_TEMP_DATA, temp_data, sizeof(data));
//     *temp_data = *temp_data<<0;
    max30100_read_reg(MAX30100_FIFO_DATA, data, sizeof(data));
    //max30100_read_reg(MAX30100_FIFO_DATA, red_data, 1);
    *ir_data = (data[0]<<8) | (data[1]);
    *red_data = (data[2]<<8) | (data[3]);
     
}

void max30100_read_data(uint8_t *sensor_data){
uint8_t data;
    max30100_read_reg(MAX30100_FIFO_DATA, &data, sizeof(data));
    *sensor_data = data;
}
// Add other functions as needed

//// Example usage
//int main(void) {
//    // Initialize TWI (I2C) peripheral
//    twi_init();
//
//    // Initialize MAX30100 sensor
//    max30100_init();
//
//    while (1) {
//        // Implement your main functionality here
//
//        // Example: Read sensor data
//        uint8_t ir_data, red_data;
//        max30100_read_sensor_data(&ir_data, &red_data);
//
//        // Process the data as needed
//
//        nrf_delay_ms(1000);  // Example delay
//    }
//}
