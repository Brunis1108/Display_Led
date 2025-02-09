#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "ws2812.pio.h" // Biblioteca para controle dos LEDs WS2812
#include <ctype.h>
#include <stdio.h>

// Definição dos pinos para comunicação I2C com o display SSD1306
#define I2C_PORT i2c1
#define PIN_I2C_SDA 14
#define PIN_I2C_SCL 15
#define endereco 0x3C

// Definição dos pinos dos LEDs RGB
#define led_red 13
#define led_blue 12
#define led_green 11

// Definição dos pinos dos botões
#define buttonA 5
#define buttonB 6

// Definição do pino da matriz de LEDs WS2812 e número de pixels
#define MATRIX_LED 7
#define NUM_PIXELS 25 // Matriz 5x5

// Configuração da máquina de estado do PIO
PIO pio = pio0;
uint sm = 0;
volatile int number = 0;

// Buffer para armazenar o estado dos LEDs da matriz
uint32_t led_buffer[NUM_PIXELS];

// Variáveis globais para armazenamento de caracteres e strings
char c;
char palavra[7] = {0};
ssd1306_t ssd;
char *status_led_verde;
char *status_led_azul;

// Protótipos de funções
void atualizar_display();
void led_init();
void button_init();
void debounce(uint gpio, uint32_t events);
void iniciar_leds();
void atualizar_leds();
void display_number(int num);

int main()
{
    // Inicialização da comunicação serial
    stdio_init_all();
    
    // Inicialização dos botões e LEDs
    button_init();
    led_init();
    iniciar_leds();

    // Configuração de interrupções para os botões
    gpio_set_irq_enabled_with_callback(buttonA, GPIO_IRQ_EDGE_FALL, true, &debounce);
    gpio_set_irq_enabled_with_callback(buttonB, GPIO_IRQ_EDGE_FALL, true, &debounce);

    // Inicialização da comunicação I2C a 400kHz
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(PIN_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_I2C_SDA);
    gpio_pull_up(PIN_I2C_SCL);

    // Configuração do display SSD1306
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    // Limpa o display ao iniciar
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Variáveis auxiliares para controle do estado dos LEDs
    int cont = 0;
    status_led_verde = "LED VERDE: OFF";
    status_led_azul = "LED AZUL: OFF";
    atualizar_display();

    // Loop principal do programa
    while (true)
    {
        printf("Digite:\n ");

        // Verifica se o USB está conectado para entrada de dados
        if (stdio_usb_connected())
        {
            // Lê um caractere da entrada padrão
            if (scanf("%c", &c) == 1)
            {
                // Se for um número, exibe na matriz de LEDs
                if (isdigit(c))
                {
                    int num = c - '0'; // Converte o caractere para um número inteiro
                    display_number(num);
                    atualizar_leds();
                }
                printf("Caractere:%c\n", c);
            }

            // Controle do armazenamento da palavra digitada
            if (cont == 7)
            {
                // Se a palavra atingiu o limite de 7 caracteres, reseta o buffer
                for (int i = 0; i < 7; i++)
                {
                    palavra[i] = '\0'; // Limpa o vetor
                }
                cont = 0; // Reseta o contador
                printf("Resetando Palavra...\n");
            }

            // Armazena o caractere na palavra e exibe no display
            palavra[cont] = c;
            palavra[cont + 1] = '\0';
            printf("Palavra: %s\n", palavra);
            cont += 1;

            // Atualiza o display com os novos dados
            atualizar_display();
        }
    }

    return 0;
}
