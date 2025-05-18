#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>  // Biblioteca para data e hora
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

/*A função read_internal_temperature() tem como objetivo ler a temperatura 
interna do microcontrolador (no caso, provavelmente do Raspberry Pi Pico) 
usando o ADC (Conversor Analógico-Digital) e calcular o valor da temperatura 
a partir dessa leitura.
*/
float read_internal_temperature() {
    const float conversion_factor = 3.3f / (1 << 12);
    uint16_t raw = adc_read();
    float voltage = raw * conversion_factor;
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;
    return temperature;
}

/*A função get_time_str() tem como objetivo obter a hora atual e 
formatá-la como uma string no formato "HH:MM:SS"*/
void get_time_str(char* buffer, size_t buffer_size) {
    // Obter hora atual
    time_t t = time(NULL);
    struct tm tm_info;
    localtime_r(&t, &tm_info);
    
    // Formatar como "hh:mm:ss"
    strftime(buffer, buffer_size, "%H:%M:%S", &tm_info);
}

/*A função get_date_str() tem como objetivo obter a data atual do sistema e 
formatá-la como uma string no formato "DD/MM/YYYY". Assim como a função get_time_str() 
(que você viu anteriormente), ela usa a estrutura time_t para acessar o tempo e, em 
seguida, formata a data conforme o formato especificado.*/
void get_date_str(char* buffer, size_t buffer_size) {
    // Obter data atual
    time_t t = time(NULL);
    struct tm tm_info;
    localtime_r(&t, &tm_info);
    
    // Formatar como "dd/mm/yyyy"
    strftime(buffer, buffer_size, "%d/%m/%Y", &tm_info);
}


int main()
{
    stdio_init_all();

    /*Essas linhas de código inicializam e configuram o ADC (Conversor Analógico-Digital) do 
    RP2040 (chip do Raspberry Pi Pico) para ler a temperatura interna do microcontrolador. */
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);

    // inicializar a comunicação I2C no Raspberry Pi Pico (RP2040), 
    // utilizando o SDK do Pico (pico-sdk). A seguir, explico o que cada linha faz:
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init();

    /*Essa linha define e inicializa uma estrutura render_area chamada frame_area, 
    que representa uma área da tela do display OLED SSD1306 onde você deseja desenhar ou atualizar conteúdo.*/
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    //A função calculate_render_area_buffer_length(&frame_area) provavelmente 
    //retorna o tamanho, em bytes, do buffer necessário para renderizar a área 
    //da tela especificada por frame_area.
    calculate_render_area_buffer_length(&frame_area);

    /*Esse trecho de código em C cria, zera e envia um framebuffer vazio (tela preta ou desligada) 
    para um display OLED SSD1306, utilizando um buffer de memória e uma área de renderização. 
    Vamos analisar linha por linha:*/
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    while (true) {
        //Está chamando uma função chamada read_internal_temperature(), que provavelmente:
        float temp = read_internal_temperature();


        //Esse trecho de código converte o valor da temperatura (temp, tipo float) para uma 
        //string formatada, armazenada no buffer temp_str, de forma que você possa exibir ou imprimir o valor de forma legível.
        char temp_str[32];
        snprintf(temp_str, sizeof(temp_str), "Temp: %.2f C", temp);

        // Obter hora e data
        char time_str[16];
        char date_str[16];
        get_time_str(time_str, sizeof(time_str));
        get_date_str(date_str, sizeof(date_str));

        // Limpa o buffer antes de redesenhar
        memset(ssd, 0, ssd1306_buffer_length);

        // Pode adicionar textos estáticos também
        ssd1306_draw_string(ssd, 0, 0, "Temperatura:");
        ssd1306_draw_string(ssd, 0, 16, temp_str); // desenha a string com a temperatura

        // ssd1306_draw_string(ssd, 0, 32, "Hora:");
        // ssd1306_draw_string(ssd, 0, 48, time_str); // Desenha a hora formatada

        ssd1306_draw_string(ssd, 0, 32, "Data:");
        ssd1306_draw_string(ssd, 0, 48, date_str); // Desenha a data formatada

        render_on_display(ssd, &frame_area);
        sleep_ms(1000);
    }

    return 0;
}
