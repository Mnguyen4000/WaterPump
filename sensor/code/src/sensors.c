// Created 06/10/2024

#include <zephyr/drivers/i2c.h>


#define BME680_ADDR 0x76 // Note the LSB is determined by the VDO state
#define LTR329_ADDR 0x29

const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

int i2c_initt(void) {
    
    I2C_SPEED_SET(I2C_SPEED_STANDARD);

    if (!device_is_ready(i2c_dev) || i2c_dev == NULL) {
    //send_str(uart0, "hed:");
    //LOG_ERR("Device not ready");
    printf("huh");
    return -1;
    }
    printf("worked");

    uint8_t buf[2] = {0x72, 0b00000001};
    int ret2 = i2c_write(i2c_dev, buf, 2, BME680_ADDR);

    // Writing something in i2c
    uint8_t buf2[2] = {0x74, 0b00100101};
    int ret = i2c_write(i2c_dev, buf2, 2, BME680_ADDR);

    return 0;
}

int BME680_init(void) {

    return 0;
}

int BME680_read(void) {

    // Writing something in i2c
    //uint8_t buf[2] = { 0x00, 0x37};
    //int ret = i2c_write(i2c_dev, buf, 2, BME680_ADDR);


    // Reading something in i2c

    uint8_t buf2[2] = {0x74, 0b00100101};
    int ret = i2c_write(i2c_dev, buf2, 2, BME680_ADDR);

    uint8_t buffer1[3];
    uint8_t read1[1] = {0x22};

    i2c_write(i2c_dev, read1, 1, BME680_ADDR);
    k_sleep(K_MSEC(30));
    i2c_read(i2c_dev, buffer1, 3, BME680_ADDR);
    for (int i = 0; i < 3; i++) {

    printf("%d-", buffer1[i]);
    }
    printf("\n");
    return 0;
}

int LTR329_init(void) {
    return 0;
}

/* @brief This internal API is used to calculate the temperature value. */
static float calc_temperature(uint32_t temp_adc, struct bme68x_dev *dev)
{
    float var1;
    float var2;
    float calc_temp;

    /* calculate var1 data */
    var1 = ((((float)temp_adc / 16384.0f) - ((float)dev->calib.par_t1 / 1024.0f)) * ((float)dev->calib.par_t2));

    /* calculate var2 data */
    var2 =
        (((((float)temp_adc / 131072.0f) - ((float)dev->calib.par_t1 / 8192.0f)) *
          (((float)temp_adc / 131072.0f) - ((float)dev->calib.par_t1 / 8192.0f))) * ((float)dev->calib.par_t3 * 16.0f));

    /* t_fine value*/
    dev->calib.t_fine = (var1 + var2);

    /* compensated temperature data*/
    calc_temp = ((dev->calib.t_fine) / 5120.0f);

    return calc_temp;
}

/* @brief This internal API is used to calculate the pressure value. */
static float calc_pressure(uint32_t pres_adc, const struct bme68x_dev *dev)
{
    float var1;
    float var2;
    float var3;
    float calc_pres;

    var1 = (((float)dev->calib.t_fine / 2.0f) - 64000.0f);
    var2 = var1 * var1 * (((float)dev->calib.par_p6) / (131072.0f));
    var2 = var2 + (var1 * ((float)dev->calib.par_p5) * 2.0f);
    var2 = (var2 / 4.0f) + (((float)dev->calib.par_p4) * 65536.0f);
    var1 = (((((float)dev->calib.par_p3 * var1 * var1) / 16384.0f) + ((float)dev->calib.par_p2 * var1)) / 524288.0f);
    var1 = ((1.0f + (var1 / 32768.0f)) * ((float)dev->calib.par_p1));
    calc_pres = (1048576.0f - ((float)pres_adc));

    /* Avoid exception caused by division by zero */
    if ((int)var1 != 0)
    {
        calc_pres = (((calc_pres - (var2 / 4096.0f)) * 6250.0f) / var1);
        var1 = (((float)dev->calib.par_p9) * calc_pres * calc_pres) / 2147483648.0f;
        var2 = calc_pres * (((float)dev->calib.par_p8) / 32768.0f);
        var3 = ((calc_pres / 256.0f) * (calc_pres / 256.0f) * (calc_pres / 256.0f) * (dev->calib.par_p10 / 131072.0f));
        calc_pres = (calc_pres + (var1 + var2 + var3 + ((float)dev->calib.par_p7 * 128.0f)) / 16.0f);
    }
    else
    {
        calc_pres = 0;
    }

    return calc_pres;
}

/* This internal API is used to calculate the humidity in integer */
static float calc_humidity(uint16_t hum_adc, const struct bme68x_dev *dev)
{
    float calc_hum;
    float var1;
    float var2;
    float var3;
    float var4;
    float temp_comp;

    /* compensated temperature data*/
    temp_comp = ((dev->calib.t_fine) / 5120.0f);
    var1 = (float)((float)hum_adc) -
           (((float)dev->calib.par_h1 * 16.0f) + (((float)dev->calib.par_h3 / 2.0f) * temp_comp));
    var2 = var1 *
           ((float)(((float)dev->calib.par_h2 / 262144.0f) *
                    (1.0f + (((float)dev->calib.par_h4 / 16384.0f) * temp_comp) +
                     (((float)dev->calib.par_h5 / 1048576.0f) * temp_comp * temp_comp))));
    var3 = (float)dev->calib.par_h6 / 16384.0f;
    var4 = (float)dev->calib.par_h7 / 2097152.0f;
    calc_hum = var2 + ((var3 + (var4 * temp_comp)) * var2 * var2);
    if (calc_hum > 100.0f)
    {
        calc_hum = 100.0f;
    }
    else if (calc_hum < 0.0f)
    {
        calc_hum = 0.0f;
    }

    return calc_hum;
}