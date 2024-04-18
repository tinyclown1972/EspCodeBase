#include <esp_system.h>
#include <stdio.h>
#include "esp_task.h"
#include <freertos/task.h>
#include "ssd1306.h"

void app_main(void)
{
    printf("Hello world\n");
    SSD1306_t dev;
    DisplayInit(&dev);

    vTaskDelay(3000/portTICK_PERIOD_MS);
    ssd1306_clear_screen(&dev, false);
    ssd1306_contrast(&dev, 0xff);
    ssd1306_display_text_x3(&dev, 0, "Hello", 5, false);
    vTaskDelay(3000/portTICK_PERIOD_MS);

    int i, j;
    for (i = 0; i < 128; i++)
    {
        for (j = 0; j < 64; j++)
        {
            _ssd1306_pixel(&dev, i, j,false);
        }
    }
    ssd1306_show_buffer(&dev);
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    for (i = 5; i < 120; i++)
    {
        for (j = 5; j < 50; j++)
        {
            _ssd1306_pixel(&dev, i, j,true);
        }
    }
    ssd1306_show_buffer(&dev);
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    for (i = 50; i < 90; i++)
    {
        for (j = 20; j < 45; j++)
        {
            _ssd1306_pixel(&dev, i, j,false);
        }
    }
    ssd1306_show_buffer(&dev);
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    esp_restart();
}
