// Created 06/10/2024

#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/adc.h>

#define BME680_ADDR 0x76 // Note the LSB is determined by the VDO state
#define LTR329_ADDR 0x29


/* ADC node from the devicetree. */
//#define ADC_NODE DT_ALIAS(adc2)

/* Data of ADC device specified in devicetree. */
//const struct device *adc = DEVICE_DT_GET(ADC_NODE);

const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

int i2c_initt(void) {
    
    I2C_SPEED_SET(I2C_SPEED_STANDARD);

    if (!device_is_ready(i2c_dev) || i2c_dev == NULL) {
    //send_str(uart0, "hed:");
    //LOG_ERR("Device not ready");
    printf("huh");
    return -1;
    }
    printf("worked\n");


    return 0;
}

/* Auxiliary macro to obtain channel vref, if available. */
//#define CHANNEL_VREF(node_id) DT_PROP_OR(node_id, zephyr_vref_mv, 0)

/* Data array of ADC channels for the specified ADC. */
//static const struct adc_channel_cfg channel_cfgs[] = {
//	DT_FOREACH_CHILD_SEP(ADC_NODE, ADC_CHANNEL_CFG_DT, (,))};

/* Data array of ADC channel voltage references. */
//static uint32_t vrefs_mv[] = {DT_FOREACH_CHILD_SEP(ADC_NODE, CHANNEL_VREF, (,))};

/* Get the number of channels defined on the DTS. */
//#define CHANNEL_COUNT ARRAY_SIZE(channel_cfgs)

//#define CONFIG_SEQUENCE_SAMPLES 5
//#define CONFIG_SEQUENCE_RESOLUTION 12
/*
int adc_test(void) {
	int err;
	uint32_t count = 0;
	uint16_t channel_reading[CONFIG_SEQUENCE_SAMPLES][CHANNEL_COUNT];

	const struct adc_sequence_options options = {
		.extra_samplings = CONFIG_SEQUENCE_SAMPLES - 1,
		.interval_us = 0,
	};

	struct adc_sequence sequence = {
		.buffer = channel_reading,
		.buffer_size = sizeof(channel_reading),
		.resolution = CONFIG_SEQUENCE_RESOLUTION,
		.options = &options,
	};

	if (!device_is_ready(adc)) {
		printf("ADC controller device %s not ready\n", adc->name);
		return 0;
	}

	for (size_t i = 0U; i < CHANNEL_COUNT; i++) {
		sequence.channels |= BIT(channel_cfgs[i].channel_id);
		err = adc_channel_setup(adc, &channel_cfgs[i]);
		if (err < 0) {
			printf("Could not setup channel #%d (%d)\n", i, err);
			return 0;
		}
		if (channel_cfgs[i].reference == ADC_REF_INTERNAL) {
			vrefs_mv[i] = adc_ref_internal(adc);
		}
	}

	while (1) {

		printf("ADC sequence reading [%u]:\n", count++);
		k_msleep(1000);

		err = adc_read(adc, &sequence);
		if (err < 0) {
			printf("Could not read (%d)\n", err);
			continue;
		}

		for (size_t channel_index = 0U; channel_index < CHANNEL_COUNT; channel_index++) {
			int32_t val_mv;

			printf("- %s, channel %" PRId32 ", %" PRId32 " sequence samples:\n",
			       adc->name, channel_cfgs[channel_index].channel_id,
			       CONFIG_SEQUENCE_SAMPLES);
			for (size_t sample_index = 0U; sample_index < CONFIG_SEQUENCE_SAMPLES;
			     sample_index++) {

				val_mv = channel_reading[sample_index][channel_index];

				printf("- - %" PRId32, val_mv);
				err = adc_raw_to_millivolts(vrefs_mv[channel_index],
							    channel_cfgs[channel_index].gain,
							    CONFIG_SEQUENCE_RESOLUTION, &val_mv);

				if ((err < 0) || vrefs_mv[channel_index] == 0) {
					printf(" (value in mV not available)\n");
				} else {
					printf(" = %" PRId32 "mV\n", val_mv);
				}
			}
		}
	}

	return 0;
}
*/

int i2c_test(void) {

}

int i2c_tester(void) {

    uint8_t buf[2] = {0x72, 0b00000001};
    i2c_write(i2c_dev, buf, 2, BME680_ADDR);

    // Writing something in i2c
    uint8_t buf2[2] = {0x74, 0b00100101};
    i2c_write(i2c_dev, buf2, 2, BME680_ADDR);

    return 0;
}

int LTR329_init(void) {
    uint8_t buffer[2] = {0x80, 0x01};
    i2c_write(i2c_dev, buffer, 2, LTR329_ADDR);

    return 0;
}

int LTR329_read(void) {
    uint8_t buffer[2];
    // Reading from ALS_DATA_CH0_0
    uint8_t read[1] = {0x8A};

    i2c_write(i2c_dev, read, 1, LTR329_ADDR);
    k_sleep(K_MSEC(30));
    i2c_read(i2c_dev, buffer, 2, LTR329_ADDR);

    // buffer[0] is LSB, buffer[1] is MSB
    uint16_t result = buffer[0] | (buffer[1] << 8);
    printf("Light Sensor: %d out of 65536\n", result);
    return 0;
}

int BME680_init(void) {
	// CTRL Measure, Turning on temperature, pressure
    uint8_t buf1[2] = {0x74, 0b00100101};
    int ret = i2c_write(i2c_dev, buf1, 2, BME680_ADDR);

	// Turn on Humidity
	uint8_t buf2[2] = {0x72, 0b00000001};
    ret = i2c_write(i2c_dev, buf2, 2, BME680_ADDR);

	// Turn on Gas

	uint8_t buf3[2] = {0x71, 0b00010000};
    ret = i2c_write(i2c_dev, buf3, 2, BME680_ADDR);

    return 0;
}

int BME680_temp_raw(void) {
	// Read Temperature
    uint8_t buffer[3];
    uint8_t read[1] = {0x22};
    i2c_write(i2c_dev, read, 1, BME680_ADDR);
    k_sleep(K_MSEC(30));
    i2c_read(i2c_dev, buffer, 3, BME680_ADDR);

	int result = buffer[0] << 16 | buffer[1] << 8 | buffer[2];

	return result >> 4;
}

int BME680_pressure_raw(void) {
	// Read Pressure
	uint8_t buffer[3];
    uint8_t read1[1] = {0x1F};
    i2c_write(i2c_dev, read1, 1, BME680_ADDR);
    k_sleep(K_MSEC(30));
    i2c_read(i2c_dev, buffer, 3, BME680_ADDR);

	int result = buffer[0] << 16 | buffer[1] << 8 | buffer[2];

	return result >> 4;
}

int BME680_gas_raw(void) {
	// Read Gas
    uint8_t read1[0] = {0x2A};
	uint8_t buffer[2];
    i2c_write(i2c_dev, read1, 1, BME680_ADDR);
    k_sleep(K_MSEC(30));
    i2c_read(i2c_dev, buffer, 2, BME680_ADDR);

	int result = buffer[0] << 8 | buffer[1];
	
	printf("Gas: %d", result);

	return result >> 6;

}

int BME680_humidity_raw(void) {
	// Initialise Variables
	uint8_t buffer1[2];
    uint8_t read1[1];

	// Read Humidity
	read1[0] = 0x25;
    i2c_write(i2c_dev, read1, 1, BME680_ADDR);
    k_sleep(K_MSEC(30));
    i2c_read(i2c_dev, buffer1, 2, BME680_ADDR);

	int hum_adc =  buffer1[0] << 8 | buffer1[1];

	return hum_adc;
}

int BME680_t_fine(void) {
		// Initialise variables
	int var1, var2, var3, t_fine;
	
	// Get calibration parameters
	uint8_t buffer1[2], buffer2[3];
    uint8_t read1[1] = {0xE9};
    i2c_write(i2c_dev, read1, 1, BME680_ADDR);
    k_sleep(K_MSEC(30));
    i2c_read(i2c_dev, buffer1, 2, BME680_ADDR);

	int16_t par_t1 = buffer1[0] | buffer1[1] << 8;


    read1[0] = 0x8A;
    i2c_write(i2c_dev, read1, 1, BME680_ADDR);
    k_sleep(K_MSEC(30));
    i2c_read(i2c_dev, buffer2, 3, BME680_ADDR);

	int16_t par_t2 = buffer2[0] | buffer2[1] << 8;
	int8_t par_t3 = buffer2[2];

	int temp_adc = BME680_temp_raw();

	// Calculate the compensated temperature from raw data.
	var1 = ((int32_t) temp_adc >> 3) - ((int32_t)par_t1 << 1);
	var2 = (var1 * (int32_t)par_t2) >> 11;
	var3 = ((((var1 >> 1) * (var1 >> 1)) >> 12) * ((int32_t)par_t3 << 4)) >> 14;
	t_fine = var2 + var3;
	return t_fine;
}

int BME680_temp_int(void) {
	// Initialise variables
	int var1, var2, var3, t_fine, temp_comp;
	
	// Get calibration parameters
	uint8_t buffer1[2], buffer2[3];
    uint8_t read1[1] = {0xE9};
    i2c_write(i2c_dev, read1, 1, BME680_ADDR);
    k_sleep(K_MSEC(30));
    i2c_read(i2c_dev, buffer1, 2, BME680_ADDR);

	int16_t par_t1 = buffer1[0] | buffer1[1] << 8;


    read1[0] = 0x8A;
    i2c_write(i2c_dev, read1, 1, BME680_ADDR);
    k_sleep(K_MSEC(30));
    i2c_read(i2c_dev, buffer2, 3, BME680_ADDR);

	int16_t par_t2 = buffer2[0] | buffer2[1] << 8;
	int8_t par_t3 = buffer2[2];

	int temp_adc = BME680_temp_raw();

	// Calculate the compensated temperature from raw data.
	var1 = ((int32_t) temp_adc >> 3) - ((int32_t)par_t1 << 1);
	var2 = (var1 * (int32_t)par_t2) >> 11;
	var3 = ((((var1 >> 1) * (var1 >> 1)) >> 12) * ((int32_t)par_t3 << 4)) >> 14;
	t_fine = var2 + var3;
	temp_comp = ((t_fine * 5) + 128) >> 8;
	printf("Temp: %d\n", temp_comp);
	return temp_comp;
}


int BME680_humidity_int(void) {
	// Initialise variables
	int temp_scaled, var1, var2, var3, var4, var5, var6, hum_comp;
	int temp_comp = BME680_temp_int();

	uint8_t buffer2[8];

	int hum_adc = BME680_humidity_raw();


	// Get calibration parameters
	uint8_t read2[1] = {0xE1};
    i2c_write(i2c_dev, read2, 1, BME680_ADDR);
    k_sleep(K_MSEC(30));
    i2c_read(i2c_dev, buffer2, 8, BME680_ADDR);
	
	int par_h1 = (buffer2[1] & 0x0F) | buffer2[2] << 8; 
	int par_h2 = (buffer2[1] & 0xF0) | buffer2[0] << 8;
	par_h1 = par_h1 >> 4;
	par_h2 = par_h2 >> 4;
	int par_h3 = buffer2[3];
	int par_h4 = buffer2[4];
	int par_h5 = buffer2[5];
	int par_h6 = buffer2[6];
	int par_h7 = buffer2[7];

	// Calcalculate the compensated humidity with raw data.
	temp_scaled = (int32_t)temp_comp;
	var1 = (int32_t)hum_adc - (int32_t)((int32_t)par_h1 << 4) - (((temp_scaled * (int32_t)par_h3) / ((int32_t)100)) >> 1);
	var2 = ((int32_t)par_h2 * (((temp_scaled * (int32_t)par_h4) / ((int32_t)100)) + (((temp_scaled * ((temp_scaled * (int32_t)par_h5) / ((int32_t)100))) >> 6) / ((int32_t)100)) + ((int32_t)(1 << 14)))) >> 10;
	var3 = var1 * var2;
	var4 = (((int32_t)par_h6 << 7) + ((temp_scaled * (int32_t)par_h7) / ((int32_t)100))) >> 4;
	var5 = ((var3 >> 14) * (var3 >> 14)) >> 10;
	var6 = (var4 * var5) >> 1;
	hum_comp = (((var3 + var6) >> 10) * ((int32_t) 1000)) >> 12;
	
	printf("Humidity: %d\n", hum_comp);
	return hum_comp;
}

int BME680_pressure_int(void) {
	int var1, var2, var3, press_comp;
	int t_fine = BME680_t_fine();
	int press_adc = BME680_pressure_raw();

	uint8_t buffer[20];
	
	// Get calibration parameters
	uint8_t read2[1] = {0x8E};
    i2c_write(i2c_dev, read2, 1, BME680_ADDR);
    k_sleep(K_MSEC(30));
    i2c_read(i2c_dev, buffer, 18, BME680_ADDR);

	int par_p1 = buffer[0] | buffer[1] << 8; // 0x8E/0x8F
	int par_p2 = buffer[2] | buffer[3] << 8; // 0x90/0x91
	int par_p3 = buffer[4]; // 0x92
	int par_p4 = buffer[6] | buffer[7] << 8; // 0x94/0x95
	int par_p5 = buffer[8] | buffer[9] << 8; // 0x96/0x97
	int par_p6 = buffer[11]; // 0x99
	int par_p7 = buffer[10]; // 0x98
	int par_p8 = buffer[14] | buffer[15] << 8; // 0x9C/0x9D
	int par_p9 = buffer[16] | buffer[17] << 8;// 0x9E/0x9F
	int par_p10 = buffer[18]; // 0xA0

	//printf("testing: %x, %x, %x, %x, %x, %x, %x, %x, %x, %x\n", par_p1, par_p2, par_p3, par_p4, par_p5, par_p6, par_p7, par_p8, par_p9, par_p10);


	var1 = ((int32_t)t_fine >> 1) - 64000;
	var2 = ((((var1 >> 2) * (var1 >> 2)) >> 11) * (int32_t)par_p6) >> 2;
	var2 = var2 + ((var1 * (int32_t)par_p5) << 1);
	var2 = (var2 >> 2) + ((int32_t)par_p4 << 16);
	var1 = (((((var1 >> 2) * (var1 >> 2)) >> 13) * ((int32_t)par_p3 << 5)) >> 3) + (((int32_t)par_p2 * var1) >> 1);
	var1 = var1 >> 18;
	var1 = ((32768 + var1) * (int32_t)par_p1) >> 15;
	press_comp = 1048576 - press_adc;
	press_comp = (uint32_t)((press_comp - (var2 >> 12)) * ((uint32_t)3125));

	if (press_comp >= (1 << 30)) {
		press_comp = ((press_comp / (uint32_t)var1) << 1);
	} else {
		press_comp = ((press_comp << 1) / (uint32_t)var1);
	}

	var1 = ((int32_t)par_p9 * (int32_t)(((press_comp >> 3) * (press_comp >> 3)) >> 13)) >> 12;
	var2 = ((int32_t)(press_comp >> 2) * (int32_t)par_p8) >> 13;
	var3 = ((int32_t)(press_comp >> 8) * (int32_t)(press_comp >> 8) * (int32_t)(press_comp >> 8) * (int32_t)par_p10) >> 17;
	press_comp = (int32_t)(press_comp) + ((var1 + var2 + var3 + ((int32_t)par_p7 << 7)) >> 4);
	printf("Pressure: %d\n", press_comp);
	// Note to marcus, may need to -120 to the number to calibrate it.
	return press_comp;
}

int BME680_read(void) {

    // Writing something in i2c
    //uint8_t buf[2] = { 0x00, 0x37};
    //int ret = i2c_write(i2c_dev, buf, 2, BME680_ADDR);

	//BME680_temp_raw();
	BME680_temp_int();
	BME680_pressure_int();
	BME680_humidity_int();
	BME680_gas_raw();
	//BME680_temp_float();
    uint8_t buffer1[3];
    uint8_t read1[1] = {0x22};





	/*
	uint8_t buffer1[3];
    uint8_t read1[1] = {0x8C};
    i2c_write(i2c_dev, read1, 1, BME680_ADDR);
    k_sleep(K_MSEC(30));
    i2c_read(i2c_dev, buffer1, 2, BME680_ADDR);
    for (int i = 0; i < 2; i++) {

    printf("%x-", buffer1[2]);
    }
	*/
    printf("\n");
    return 0;
}


