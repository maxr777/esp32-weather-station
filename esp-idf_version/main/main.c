#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_master.h"
#include "i2c_slave.h"
#include "sdkconfig.h"
#include <stdio.h>

#define I2C_MASTER_SCL_IO         CONFIG_I2C_MASTER_SCL       /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO         CONFIG_I2C_MASTER_SDA       /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM            I2C_NUM_0                   /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ        CONFIG_I2C_MASTER_FREQUENCY /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS     1000

#define OLED_ADDR 0x3C

#define BME280_ADDR     0x76
#define BME280_REG_ADDR 0xF7

#define DS3231_ADDR 0x68

#ifdef CONFIG_IDF_TARGET_ESP32
#define CHIP_NAME "ESP32"
#endif

#ifdef CONFIG_IDF_TARGET_ESP32S2BETA
#define CHIP_NAME "ESP32-S2 Beta"
#endif

#define CONFIG_I2C_ENABLE_SLAVE_DRIVER_VERSION_2

typedef struct {
  double temperature;
  double humidity;
  double pressure;
} BME280;

void bme280_fill_out_fields(uint8_t bme280_data[], int size) {
  if (size != 8) {
    fprintf(stderr, "ERROR: The size of bme280_data array should be 8 bytes\n");
    return;
  }
}

i2c_master_bus_config_t i2c_config = {0};
i2c_config.i2c_port = 1;

void app_main(void) {
  i2c_master_bus_config_t bus_config = {
      .i2c_port = I2C_NUM_0,
      .sda_io_num = 32,
      .scl_io_num = 33,
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .glitch_ignore_cnt = 7,
      .flags.enable_internal_pullup = true,
  };

  i2c_master_bus_handle_t bus_handle;
  ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

  i2c_device_config_t bme280_config = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address = BME280_ADDR,
      .scl_speed_hz = 100000,
  };

  i2c_master_devl_handle_t bme280_handle;
  ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &bme280_config, &bme280_handle));

  uint_8t   bme280_data[8];
  esp_err_t ret = i2c_master_transmit_receive(bme280_handle, &BME280_REG_ADDR, 1, bme280_data, 8, 1000);

  if (ret = ESP_OK)
    ;

  uint_32t temperature, humidity, pressure;

  printf("weather station\n");

  vTaskDelay(1000 / portTICK_PERIOD_MS);
  fflush(stdout);
  esp_restart();
}
