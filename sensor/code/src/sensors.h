
#ifndef INC_SENSORS_H_
#define INC_SENSORS_H_

int i2c_initt(void);
int i2c_test(void);
int i2c_tester(void);
int adc_test(void);

int BME680_init(void);
int BME680_read(void);
int BME680_temp_int(void);
int BME680_temp_float(void);
int BME680_humidity_int(void);
int BME680_humidity_float(void);


int LTR329_init(void);
int LTR329_read(void);
#endif