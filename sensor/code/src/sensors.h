
#ifndef INC_SENSORS_H_
#define INC_SENSORS_H_

int i2c_init(void);
int i2c_test(void);
int i2c_tester(void);
int adc_test(void);

// ADC Functionality
int soil_init(void);
int soil_read(void);
int soil_read_percent(void);
int uv_init(void);
int uv_read(void);
int uv_read_percent(void);


int BME680_init(void);
int BME680_read(void);
int BME680_measure(void);
int BME680_temp_int(void);
int BME680_humidity_int(void);
int BME680_pressure_int(void);
int BME680_gas_int(void);
int BME680_IAQ(void);


int LTR329_init(void);
int LTR329_read(void);
#endif