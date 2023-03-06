#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "dimmer.h"

static const char *TAG = "Dimmer";

volatile bool _init_done;
bool flag;

dimmertyp *ptr_dimmer;

#define INIT_NOT_DONE _init_done == false
#define _50Hz 50

#define ZEROCROSS_GPIO GPIO_NUM_21
#define TRIAC_1_GPIO GPIO_NUM_22
#define TRIAC_2_GPIO GPIO_NUM_26
#define DIAGNOSTIC_LED_GPIO GPIO_NUM_2

void setup();

void setup(){
    ESP_LOGI(TAG, "Starting init sequence");
    // Set diagnostic leds
    gpio_set_direction(DIAGNOSTIC_LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(DIAGNOSTIC_LED_GPIO, 1);
    // Instantiate the new dimmer
    ptr_dimmer = createDimmer(TRIAC_1_GPIO, ZEROCROSS_GPIO);
    // startup
    begin(ptr_dimmer, NORMAL_MODE, ON, _50Hz);
    ESP_LOGI(TAG, "Init sequence completed");
}

void app_main(void)
{
    if (INIT_NOT_DONE)
    {
        setup();
        // Initial point;
        setPower(ptr_dimmer, 1);
    }

    while (1)
    {
        // change the output power
        getPower(ptr_dimmer) < 60 ? setPower(ptr_dimmer, (getPower(ptr_dimmer) + 5)) : setPower(ptr_dimmer, 1);
        // wait
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

