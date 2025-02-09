# Projeto de Comunicação Serial e Controle de LEDs com RP2040
<p style="text-align: justify;">
Este projeto tem como objetivo consolidar os conceitos de comunicação serial e controle de LEDs utilizando a placa de desenvolvimento BitDogLab 
com o microcontrolador RP2040. O projeto envolve a manipulação de LEDs comuns, LEDs endereçáveis WS2812, botões de acionamento, interrupções e comunicação I2C com um display SSD1306.
<p>
  
## Descrição do Projeto
<p style="text-align: justify;">
O projeto consiste em um sistema que permite a interação com o usuário através de um terminal serial e botões físicos. As principais funcionalidades incluem:

1. **Exibição de caracteres no display SSD1306**: O usuário pode digitar caracteres no Serial Monitor do VS Code, que serão exibidos no display.
2. **Controle de LEDs WS2812**: Quando um número entre 0 e 9 é digitado, um símbolo correspondente é exibido na matriz de LEDs WS2812.
3. **Controle de LEDs RGB**: Os botões A e B alternam o estado dos LEDs verde e azul, respectivamente, e o estado é exibido no display e no Serial Monitor.
<p></p>
  
## Componentes Utilizados
<p style="text-align: justify;">
- **Matriz 5x5 de LEDs WS2812**: Conectada à GPIO 7.
- **LED RGB**: Conectado às GPIOs 11 (vermelho), 12 (azul) e 13 (verde).
- **Botão A**: Conectado à GPIO 5.
- **Botão B**: Conectado à GPIO 6.
- **Display SSD1306**: Conectado via I2C (GPIO 14 e GPIO 15).
<p></p>

## Estrutura do Código

O código está organizado em várias seções principais:

### Inicialização e Configuração

- **Inicialização da comunicação serial**: `stdio_init_all()`.
- **Inicialização dos botões e LEDs**: `button_init()` e `led_init()`.
- **Configuração de interrupções para os botões**: `gpio_set_irq_enabled_with_callback()`.
- **Inicialização da comunicação I2C**: `i2c_init()`.
- **Configuração do display SSD1306**: `ssd1306_init()`.

### Funções Principais

- **`atualizar_display()`**: Atualiza o display SSD1306 com o estado atual dos LEDs e a palavra digitada.
- **`led_init()`**: Configura os pinos dos LEDs como saída.
- **`button_init()`**: Configura os pinos dos botões como entrada com pull-up.
- **`debounce()`**: Implementa o debouncing dos botões via software.
- **`iniciar_leds()`**: Inicializa a matriz de LEDs WS2812.
- **`atualizar_leds()`**: Atualiza o estado dos LEDs WS2812.
- **`display_number()`**: Exibe um número na matriz de LEDs WS2812.

### Loop Principal

O loop principal do programa (`while (true)`) realiza as seguintes operações:

1. **Leitura de caracteres do Serial Monitor**: O usuário pode digitar caracteres, que são exibidos no display SSD1306.
2. **Controle da matriz de LEDs WS2812**: Se um número entre 0 e 9 for digitado, um símbolo correspondente é exibido na matriz de LEDs.
3. **Controle dos LEDs RGB**: Os botões A e B alternam o estado dos LEDs verde e azul, respectivamente, e o estado é exibido no display e no Serial Monitor.

## Requisitos do Projeto

- **Uso de interrupções**: Todas as funcionalidades relacionadas aos botões são implementadas utilizando rotinas de interrupção (IRQ).
- **Debouncing**: O tratamento do bouncing dos botões é implementado via software.
- **Controle de LEDs**: O projeto inclui o uso de LEDs comuns e LEDs WS2812.
- **Utilização do Display 128x64**: O display SSD1306 é utilizado para exibir caracteres e mensagens informativas.
- **Envio de informação pela UART**: A comunicação serial via UART é utilizada para enviar informações ao Serial Monitor.
- **Organização do código**: O código está bem estruturado e comentado para facilitar o entendimento.

## Como Executar o Projeto

1. **Configuração do Ambiente**: Certifique-se de que o ambiente de desenvolvimento está configurado para o RP2040 e que todas as bibliotecas necessárias estão instaladas.
2. **Compilação e Upload**: Compile o código e faça o upload para a placa BitDogLab.
3. **Interação com o Sistema**: Utilize o Serial Monitor do VS Code para digitar caracteres e interagir com os botões para controlar os LEDs.

## Conclusão

Este projeto demonstra a aplicação prática de conceitos de comunicação serial, controle de LEDs e interrupções em microcontroladores. A combinação de hardware e 
software permite a criação de um sistema interativo e funcional, que pode ser expandido e adaptado para diversas aplicações.

## Link Video
https://drive.google.com/file/d/1iZsYt-pfxpPSXOELjAsjD14hyODc134w/view?usp=drive_link
