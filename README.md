# Blood-Oxygen-Saturation-Measurement with MAX30101 and nRF52

This repository contains the code for a pulse oximeter using the **MAX30101** sensor and **nRF52** microcontroller, specifically for calculating the oxygen saturation (SpO2). The code processes the infrared (IR) and red light readings from the MAX30100 sensor to derive SpO2 using Butterworth low-pass filtering, DC removal, and RMS calculation. The firmware collects sensor data, filters it using various techniques, and calculates SpO2 using the relationship between the IR and red light RMS values.


The key features of the code include:
- **DC Removal**: Filtering out the DC component from the sensor data.
- **Mean Difference Filter**: Smooths the signal to reduce noise.
- **Butterworth Low-Pass Filter**: Further smooths the signal to focus on the heart rate.
- **RMS Calculation**: Computes the root mean square for SpO2 calculation.
- **SpO2 Calculation**: Computes SpO2 using the RMS values of IR and Red LED data.

## Features

- **Real-time Data Processing**: Process data from the MAX30101 sensor at a sampling rate of 100 Hz.
- **Signal Filtering**: Applies multiple filtering techniques (DC Removal, Mean Difference, Butterworth Low-Pass).
- **SpO2 Level**: Derives the SpO2 levels based on processed data.
- **Logging**: Outputs filtered data and calculated values to the console using the nRF Log module.

## Setup

### Prerequisites

**Hardware**:
- nRF52840 development board
- MAX30101 Pulse Oximeter Sensor
- Breadboard, jumper wires, resistors

**Software**:
- nRF5 SDK
- Segger Embedded Studio or any compatible IDE for Nordic development
  
### Wiring
- **SDA** and **SCL** of MAX30101 connect to corresponding I2C pins on nRF52
- **VCC** connects to 3.3V power
- **GND** connects to ground

### Function Definitions

- `dcRemoval(float x, float prev_w, float alpha)`: Removes the DC component from the signal.
- `meanDiff(float M, meanDiffFilter_t* filterValues)`: Applies a mean difference filter to smooth the data.
- `lowPassButterworthFilter(float x, butterworthFilter_t* filterResult)`: Applies a low-pass Butterworth filter to reduce high-frequency noise.
- `normalize_my_buffer(uint32_t *myBuffer, uint32_t size)`: Normalizes data from 0 to 100.
- `apply_moving_average_filter(uint32_t *data, uint32_t *filtered_data, uint32_t size)`: Applies a moving average filter to smooth the data.

### SpO2 Calculation

The SpO2 is calculated based on the logarithms of the RMS values of the IR and Red LED signals:
float spoo2 = (-45.060 * R^2) + (30.354 * R) + 94.845;
Where `R` is derived from the ratio of the log-transformed RMS values of red and IR light.

### Example Logs
Example outputs for filtered IR and Red LED data, along with calculated SpO2:

```bash
IR RMS: 5, Red RMS: 4
Float 96.120

