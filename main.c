//working

#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"
#include "MAX30100_3.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "math.h"


// Constants
//#define BUFFER_SIZE 1000
//#define ALPHA 0.95 // Smoothing factor
//#define MIN_PEAK_HEIGHT 1e8
#define MEAN_FILTER_SIZE 15
#define SAMPLING_RATE 100   // 100 Hz
#define WINDOW_SIZE 500     // 5 seconds of data (100 samples per second * 5 seconds)
uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data
int32_t bufferLength; //data length
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid

// Define the dcFilter_t structure
typedef struct {
    float w;
    float result;
} dcFilter_t;

// Define the meanDiffFilter_t structure
typedef struct {
    float values[MEAN_FILTER_SIZE];
    uint8_t index;
    float sum;
    uint8_t count;
} meanDiffFilter_t;

// Define the butterworthFilter_t structure
typedef struct {
    float v[2];
    float result;
} butterworthFilter_t;

// Function for DC removal
dcFilter_t dcRemoval(float x, float prev_w, float alpha) {
    dcFilter_t filtered;
    filtered.w = x + alpha * prev_w;
    filtered.result = filtered.w - prev_w;

    return filtered;
}

// Function for Mean Difference Filter
float meanDiff(float M, meanDiffFilter_t* filterValues) {
    float avg = 0;

    // Update sum and values
    filterValues->sum -= filterValues->values[filterValues->index];
    filterValues->values[filterValues->index] = M;
    filterValues->sum += filterValues->values[filterValues->index];

    // Increment and wrap the index
    filterValues->index++;
    filterValues->index = filterValues->index % MEAN_FILTER_SIZE;

    // Update count until it reaches the size of the filter
    if(filterValues->count < MEAN_FILTER_SIZE)
        filterValues->count++;

    // Calculate average
    avg = filterValues->sum / filterValues->count;

    // Return the difference between the mean and the current value
    return avg - M;
}

// Function for Butterworth Low-Pass Filter
void lowPassButterworthFilter(float x, butterworthFilter_t* filterResult) {
    filterResult->v[0] = filterResult->v[1];

    // Apply the Butterworth filter for Fs = 100Hz and Fc = 10Hz
    filterResult->v[1] = (2.452372752527856026e-1 * x) + (0.50952544949442879485 * filterResult->v[0]);

    // Combine the results
    filterResult->result = filterResult->v[0] + filterResult->v[1];
}

// Variables
//uint32_t ir_data_buffer[BUFFER_SIZE];
//uint32_t red_data_buffer[BUFFER_SIZE];

// Function prototypes
//void calculate_spo2(uint32_t *ir_data_buffer, uint32_t *red_data_buffer, int32_t buffer_size, float *spo2, bool *spo2_valid);

int main(void)
{

// initialize the logger
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();
	ret_code_t err_code;
        dcFilter_t filtered_ir, filtered_red;
    float prev_w_ir = 0, prev_w_red = 0;
    float alpha = 0.95f;  // Alpha value for the DC removal filter
    uint16_t ir_data, red_data;
    // Variables for RMS calculation
    float ir_rms_sum = 0, red_rms_sum = 0;
    int rms_counter = 0;
    meanDiffFilter_t ir_filter = {0};
    meanDiffFilter_t red_filter = {0};
    butterworthFilter_t butter_ir = {0};
    butterworthFilter_t butter_red = {0};
    
twi_init(); 
max30100_init();

    while (1) {
     
    max30100_read_sensor_data(&ir_data, &red_data);
    // Apply DC removal filter
        filtered_ir = dcRemoval((float)ir_data, prev_w_ir, alpha);
        filtered_red = dcRemoval((float)red_data, prev_w_red, alpha);
        // Update previous w values
        prev_w_ir = filtered_ir.w;
        prev_w_red = filtered_red.w;
        // Apply Mean Difference Filter
        float meanDiff_ir = meanDiff(filtered_ir.result, &ir_filter);
        float meanDiff_red = meanDiff(filtered_red.result, &red_filter);
        // Apply Butterworth Low-Pass Filter
        lowPassButterworthFilter(meanDiff_ir, &butter_ir);
        lowPassButterworthFilter(meanDiff_red, &butter_red);
        // Accumulate squares of filtered results for RMS calculation
        ir_rms_sum += butter_ir.result * butter_ir.result;
        red_rms_sum += butter_red.result * butter_red.result;
        rms_counter++;

        // Calculate RMS every 5 seconds (500 samples)
        if (rms_counter == WINDOW_SIZE) {
            float ir_rms = sqrtf(ir_rms_sum / WINDOW_SIZE);
            float red_rms = sqrtf(red_rms_sum / WINDOW_SIZE);
            float log_ir = log(ir_rms);
            float log_red = log(red_rms);
            float R = (log_red*650) / (log_ir*950);
            //float spoo2 = 110.0 - 25.0 * R;
            float spoo2 = (-45.060 * R * R)  + (30.354*R) + 94.845;

            // Log the RMS values
            NRF_LOG_RAW_INFO("IR RMS: %d, Red RMS: %d\n", (int32_t)log_ir, (int32_t)log_red);
            // Log the RMS values
            NRF_LOG_ERROR( "Float " NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(spoo2));
            //NRF_LOG_RAW_INFO("SPO2: %d", spoo2);

            // Reset the sums and counter for the next window
            ir_rms_sum = 0;
            red_rms_sum = 0;
            rms_counter = 0;
}
 // Log the filtered data
        //NRF_LOG_RAW_INFO("%d, %d\n", (int32_t)butter_ir.result, (int32_t)butter_red.result);
    nrf_delay_ms(10);
  } 
  
}


void normalize_my_buffer(uint32_t *myBuffer, uint32_t size) {
    uint32_t i;
    uint32_t min_value = myBuffer[0];
    uint32_t max_value = myBuffer[0];

    // Step 1: Find the minimum and maximum values
    for (i = 1; i < size; i++) {
        if (myBuffer[i] < min_value) {
            min_value = myBuffer[i];
        }
        if (myBuffer[i] > max_value) {
            max_value = myBuffer[i];
        }
    }

    // Step 2: Normalize the values in the array from 0 to 100
    for (i = 0; i < size; i++) {
        myBuffer[i] = (uint32_t)(((myBuffer[i] - min_value) * 100) / (max_value - min_value));
    }
}

void apply_moving_average_filter(uint32_t *data, uint32_t *filtered_data, uint32_t size) {
filtered_data[0] = data[0]; // Initialize the first point
float alpha = 10;
    for (uint32_t i = 1; i < size; i++) {
        filtered_data[i] = (uint32_t)(alpha * data[i] + (1 - alpha) * filtered_data[i - 1]);
    }
}
