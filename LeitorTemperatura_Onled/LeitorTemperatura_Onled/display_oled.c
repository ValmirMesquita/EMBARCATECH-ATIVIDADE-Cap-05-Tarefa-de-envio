#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

float read_internal_temperature() {
    const float conversion_factor = 3.3f / (1 << 12);
    uint16_t raw = adc_read();
    float voltage = raw * conversion_factor;
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;
    return temperature;
}

int main()
{
    stdio_init_all();

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);

    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init();

    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    while (true) {
        float temp = read_internal_temperature();

        char temp_str[32];
        snprintf(temp_str, sizeof(temp_str), "Temp: %.2f C", temp);

        // Limpa o buffer antes de redesenhar
        memset(ssd, 0, ssd1306_buffer_length);

        // Pode adicionar textos estáticos também
        ssd1306_draw_string(ssd, 0, 0, "Temperatura:");
        ssd1306_draw_string(ssd, 0, 16, temp_str); // desenha a string com a temperatura

        render_on_display(ssd, &frame_area);
        sleep_ms(1000);
    }

    return 0;
}
