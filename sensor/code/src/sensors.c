// Created 06/10/2024

#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/adc.h>

#define BME680_ADDR 0x76 // Note the LSB is determined by the VDO state
#define LTR329_ADDR 0x29

#define SOIL adc_channels[0]
#define UV adc_channels[1]

const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)
};

uint16_t buf;

struct adc_sequence sequence = {
	.buffer = &buf,
	// buffer size in bytes, not number of samples
	.buffer_size = sizeof(buf),
};




int soil_init(void) {
	int err;

	/* Configure channels individually prior to sampling. */
	if (!adc_is_ready_dt(&SOIL)) {
		printk("ADC controller device %s not ready\n", SOIL.dev->name);
		return 1;
	}

	err = adc_channel_setup_dt(&SOIL);
	if (err < 0) {
		printk("Could not setup channel #%d (%d)\n", 0, err);
		return 1;
	}
	printk("SOIL Initialised!\n");
	return 0;
}

int uv_init(void) {
	int err;
	/* Configure channels individually prior to sampling. */
	if (!adc_is_ready_dt(&UV)) {
		printk("ADC controller device %s not ready\n", UV.dev->name);
		return 1;
	}

	err = adc_channel_setup_dt(&UV);
	if (err < 0) {
		printk("Could not setup channel #%d (%d)\n", 1, err);
		return 1;
	}

	printk("UV Initialised!\n");
	return 0;
}

int soil_read(void) {
	int err;
	int32_t val_mv;

	(void)adc_sequence_init_dt(&SOIL, &sequence);
	adc_read_dt(&SOIL, &sequence);

	/*
		* If using differential mode, the 16 bit value
		* in the ADC sample buffer should be a signed 2's
		* complement value.
		*/
	if (SOIL.channel_cfg.differential) {
		val_mv = (int32_t)((int16_t)buf);
	} else {
		val_mv = (int32_t)buf;
	}
	
	adc_raw_to_millivolts_dt(&SOIL, &val_mv);

	return val_mv;
}

int uv_read(void) {
	int err;
	int32_t val_mv;

	(void)adc_sequence_init_dt(&UV, &sequence);

	err = adc_read_dt(&UV, &sequence);
	if (err < 0) {
		printk("Could not read (%d)\n", err);
	}

	/*
		* If using differential mode, the 16 bit value
		* in the ADC sample buffer should be a signed 2's
		* complement value.
		*/
	if (UV.channel_cfg.differential) {
		val_mv = (int32_t)((int16_t)buf);
	} else {
		val_mv = (int32_t)buf;
	}
	adc_raw_to_millivolts_dt(&UV, &val_mv);

	return val_mv;
}

int soil_read_percent(void) {
	int value = soil_read();
	int percent = ((float) value - 62.0) / 9.6;
	printk("SOIL: %i%\n", percent);
	return percent;
} 

int uv_read_percent(void) {
	int value = uv_read();
	int percent = ((float) value - 62.0) / 9.6;
	printk("UV: %i%\n", percent);
	return percent;
}

int adc_test(void) {
	soil_init();
	uv_init();


	while (1) {
		k_sleep(K_MSEC(1000));
	}

}

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
	
	printf("Gas: %d\n", result);

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
	//printf("Temp: %d\n", temp_comp);
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
	
	//printf("Humidity: %d\n", hum_comp);
	//printf("Humidity: %d.%02d\n", hum_comp / 1000, hum_comp % 1000);
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
	//printf("Pressure: %d\n", press_comp);
	//printf("Pressure: %d.%02d\n", press_comp/100, press_comp % 100);
	// Note to marcus, may need to -120 to the number to calibrate it.
	return press_comp;
}

int BME680_read(void) {

	int temp, press, hum, gas;
	temp = BME680_temp_int();
	press = BME680_pressure_int();
	hum = BME680_humidity_int();
	gas = BME680_gas_raw();


	printf("Temp: %d.%02d\n", temp / 100, temp % 100);
	printf("Pressure: %d.%02d\n", press / 100, press % 100);
	printf("Humidity: %d.%02d\n", hum / 1000, hum % 1000);

    printf("\n");
    return 0;
}


