#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "ws2812.pio.h" // Biblioteca para controle dos LEDs WS2812
#include <ctype.h>
#include <stdio.h>

// Definições para o display SSD1306 (comunicação I2C)
#define I2C_PORT i2c1
#define PIN_I2C_SDA 14
#define PIN_I2C_SCL 15
#define endereco 0x3C

// Definições dos pinos dos LEDs RGB
#define led_red 13
#define led_blue 12
#define led_green 11

// Definições dos pinos dos botões
#define buttonA 5
#define buttonB 6

// Definições para a matriz de LEDs WS2812
#define MATRIX_LED 7
#define NUM_PIXELS 25 // Matriz 5x5

// Variáveis globais para controle do PIO e estado dos LEDs
PIO pio = pio0;          // Controlador PIO (Programmable I/O)
uint sm = 0;             // Máquina de estado do PIO
volatile int number = 0; // Variável para armazenar números

// Buffer para armazenar o estado dos LEDs da matriz WS2812
uint32_t led_buffer[NUM_PIXELS];

// Variáveis globais para armazenamento de caracteres e strings
char c;                 // Caractere digitado pelo usuário
char palavra[7] = {0};  // Buffer para armazenar uma palavra de até 6 caracteres
ssd1306_t ssd;          // Estrutura para controle do display SSD1306
char *status_led_verde; // Mensagem de status do LED verde
char *status_led_azul;  // Mensagem de status do LED azul

// Protótipos das funções
void atualizar_display();                  // Atualiza o conteúdo do display SSD1306
void led_init();                           // Inicializa os pinos dos LEDs RGB
void button_init();                        // Inicializa os pinos dos botões
void debounce(uint gpio, uint32_t events); // Função de debouncing para os botões
void iniciar_leds();                       // Inicializa a matriz de LEDs WS2812
void atualizar_leds();                     // Atualiza o estado da matriz de LEDs WS2812
void display_number(int num);              // Exibe um número na matriz de LEDs WS2812

// Função para inicializar os LEDs RGB
void led_init()
{
  gpio_init(led_red);
  gpio_init(led_blue);
  gpio_init(led_green);
  gpio_set_dir(led_red, GPIO_OUT);
  gpio_set_dir(led_blue, GPIO_OUT);
  gpio_set_dir(led_green, GPIO_OUT);
}

// Função para inicializar os botões
void button_init()
{
  gpio_init(buttonA);
  gpio_init(buttonB);
  gpio_set_dir(buttonA, GPIO_IN);
  gpio_set_dir(buttonB, GPIO_IN);
  gpio_pull_up(buttonA); // Habilita resistor de pull-up no botão A
  gpio_pull_up(buttonB); // Habilita resistor de pull-up no botão B
}

// Função de debouncing para tratar as interrupções dos botões
void debounce(uint gpio, uint32_t events)
{
  static uint32_t last_time = 0; // Armazena o tempo da última interrupção
  uint32_t current_time = to_us_since_boot(get_absolute_time());

  // Verifica se o tempo desde a última interrupção é maior que 200ms (debouncing)
  if (current_time - last_time > 200000)
  {
    last_time = current_time; // Atualiza o tempo da última interrupção

    // Verifica qual botão foi pressionado
    if (gpio == buttonA)
    {
      // Alterna o estado do LED verde
      if (gpio_get(led_green) == 1)
      {
        gpio_put(led_green, 0);
        status_led_verde = "LED VERDE: OFF";
        printf("LED VERDE: OFF\n");
      }
      else
      {
        gpio_put(led_green, 1);
        status_led_verde = "LED VERDE: ON";
        printf("LED VERDE: ON\n");
      }
    }
    else if (gpio == buttonB)
    {
      // Alterna o estado do LED azul
      if (gpio_get(led_blue) == 1)
      {
        gpio_put(led_blue, 0);
        status_led_azul = "LED AZUL: OFF";
        printf("LED AZUL: OFF\n");
      }
      else
      {
        gpio_put(led_blue, 1);
        status_led_azul = "LED AZUL: ON";
        printf("LED AZUL: ON\n");
      }
    }

    // Atualiza o display com os novos estados dos LEDs
    atualizar_display();
  }
}

// Função para inicializar a matriz de LEDs WS2812
void iniciar_leds()
{
  uint offset = pio_add_program(pio, &ws2812_program);             // Adiciona o programa WS2812 ao PIO
  ws2812_program_init(pio, sm, offset, MATRIX_LED, 800000, false); // Configura o PIO
  pio_sm_set_enabled(pio, sm, true);                               // Habilita a máquina de estado do PIO
  sleep_ms(100);                                                   // Aguarda 100ms para estabilização
}

// Função para atualizar a matriz de LEDs WS2812
void atualizar_leds()
{
  for (int i = 0; i < NUM_PIXELS; i++)
  {
    pio_sm_put_blocking(pio, sm, led_buffer[i]); // Envia o estado de cada LED para o PIO
  }
}
void display_number(int num)
{
  /*
  24,23,22,21,20
  15,16,17,18,19
  14,13,12,11,10
  05,06,07,08,09
  04,03,02,01,00
  */

  // Padrões para cada número (0-9)
  // Cada número é representado por uma matriz 5x5 de 0s e 1s
  // 1 indica que o LED deve estar aceso, 0 indica apagado
  const uint32_t numbers[10][25] = {
      {0, 1, 1, 1, 0,
       0, 1, 0, 1, 0,
       0, 1, 0, 1, 0,
       0, 1, 0, 1, 0,
       0, 1, 1, 1, 0}, // 0

      {0, 1, 1, 1, 0,
       0, 0, 1, 0, 0,
       0, 0, 1, 0, 0,
       0, 1, 1, 0, 0,
       0, 0, 1, 0, 0}, // 1

      {0, 1, 1, 1, 0,
       0, 1, 0, 0, 0,
       0, 1, 1, 1, 0,
       0, 0, 0, 1, 0,
       0, 1, 1, 1, 0}, // 2

      {0, 1, 1, 1, 0,
       0, 0, 0, 1, 0,
       0, 1, 1, 1, 0,
       0, 0, 0, 1, 0,
       0, 1, 1, 1, 0}, // 3

      {0, 1, 0, 0, 0,
       0, 0, 0, 1, 0,
       0, 1, 1, 1, 0,
       0, 1, 0, 1, 0,
       0, 1, 0, 1, 0}, // 4

      {0, 1, 1, 1, 0,
       0, 0, 0, 1, 0,
       0, 1, 1, 1, 0,
       0, 1, 0, 0, 0,
       0, 1, 1, 1, 0}, // 5

      {0, 1, 1, 1, 0,
       0, 1, 0, 1, 0,
       0, 1, 1, 1, 0,
       0, 1, 0, 0, 0,
       0, 1, 1, 1, 0}, // 6

      {0, 1, 0, 0, 0,
       0, 0, 0, 1, 0,
       0, 1, 0, 0, 0,
       0, 0, 0, 1, 0,
       0, 1, 1, 1, 0}, // 7

      {0, 1, 1, 1, 0,
       0, 1, 0, 1, 0,
       0, 1, 1, 1, 0,
       0, 1, 0, 1, 0,
       0, 1, 1, 1, 0}, // 8

      {0, 1, 1, 1, 0,
       0, 0, 0, 1, 0,
       0, 1, 1, 1, 0,
       0, 1, 0, 1, 0,
       0, 1, 1, 1, 0} // 9
  };

  // Atualiza o buffer de LEDs com o padrão correspondente ao número
  for (int i = 0; i < NUM_PIXELS; i++)
  {

    led_buffer[i] = 0x00000000;// Inicializa o LED como apagado
    if (numbers[num][i])
    {
      led_buffer[i] = 0x00550000; // Define cor azul para os leds acesos
    }
  }
}
// Função para atualizar o display SSD1306
void atualizar_display()
{
  ssd1306_fill(&ssd, false); // Limpa o display
  ssd1306_send_data(&ssd);   // Envia os dados para o display

  // Exibe as informações no display
  ssd1306_draw_string(&ssd, "Caractere:", 1, 1);      // Exibe "Caractere:"
  ssd1306_draw_string(&ssd, "Palavra:", 1, 13);       // Exibe "Palavra:"
  ssd1306_draw_string(&ssd, status_led_verde, 1, 26); // Exibe o status do LED verde
  ssd1306_draw_string(&ssd, status_led_azul, 1, 39);  // Exibe o status do LED azul
  ssd1306_draw_char(&ssd, c, 82, 1);                  // Exibe o caractere digitado
  ssd1306_draw_string(&ssd, palavra, 64, 13);         // Exibe a palavra digitada

  ssd1306_send_data(&ssd); // Atualiza o display com os novos dados
}

// Função principal
int main()
{
  stdio_init_all(); // Inicializa a comunicação serial
  button_init();    // Inicializa os botões
  led_init();       // Inicializa os LEDs RGB
  iniciar_leds();   // Inicializa a matriz de LEDs WS2812

  // Configura interrupções para os botões
  gpio_set_irq_enabled_with_callback(buttonA, GPIO_IRQ_EDGE_FALL, true, &debounce);
  gpio_set_irq_enabled_with_callback(buttonB, GPIO_IRQ_EDGE_FALL, true, &debounce);

  // Inicializa a comunicação I2C
  i2c_init(I2C_PORT, 400 * 1000);                // Configura o I2C a 400kHz
  gpio_set_function(PIN_I2C_SDA, GPIO_FUNC_I2C); // Configura o pino SDA
  gpio_set_function(PIN_I2C_SCL, GPIO_FUNC_I2C); // Configura o pino SCL
  gpio_pull_up(PIN_I2C_SDA);                     // Habilita pull-up no pino SDA
  gpio_pull_up(PIN_I2C_SCL);                     // Habilita pull-up no pino SCL

  // Inicializa o display SSD1306
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
  ssd1306_config(&ssd);
  ssd1306_send_data(&ssd);

  // Limpa o display
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  // Inicializa as variáveis de estado
  int cont = 0;                        // Contador para a palavra
  status_led_verde = "LED VERDE: OFF"; // Estado inicial do LED verde
  status_led_azul = "LED AZUL: OFF";   // Estado inicial do LED azul
  atualizar_display();                 // Atualiza o display com os estados iniciais

  // Loop principal
  while (true)
  {
    printf("Digite:\n "); // Solicita entrada do usuário

    if (stdio_usb_connected())
    { // Verifica se o USB está conectado
      if (scanf("%c", &c) == 1)
      { // Lê um caractere da entrada padrão
        if (isdigit(c))
        {                      // Verifica se o caractere é um número
          int num = c - '0';   // Converte o caractere para um número inteiro
          display_number(num); // Exibe o número na matriz de LEDs
          atualizar_leds();    // Atualiza a matriz de LEDs
        }
        printf("Caractere:%c\n", c); // Exibe o caractere no terminal
      }

      // Verifica se a palavra atingiu o limite de 7 caracteres
      if (cont == 7)
      {
        for (int i = 0; i < 7; i++)
        {
          palavra[i] = '\0'; // Limpa o buffer da palavra
        }
        cont = 0; // Reseta o contador
        printf("Resetando Palavra...\n");
      }

      // Armazena o caractere na palavra e exibe no display
      palavra[cont] = c;
      palavra[cont + 1] = '\0';
      printf("Palavra: %s\n", palavra); // Exibe a palavra no terminal
      cont += 1;                        // Incrementa o contador

      atualizar_display(); // Atualiza o display com os novos dados
    }
  }

  return 0;
}