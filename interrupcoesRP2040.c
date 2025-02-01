#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "ws2812b.pio.h"

/* Desenvolvedor: Marco Túlio Macêdo Oliveira dos Santos */ 

// Identificação das GPIOs conectadas aos LEDs RGB
#define LED_R_PIN 13

// Identificação das GPIOs conectadas aos botões A e B
#define BTN_A_PIN 5
#define BTN_B_PIN 6

// Definição do número de LEDs na Matrix, da intensidade dos LEDs e da GPIO
#define LED_MTX_COUNT 25
#define LED_MTX_LEVEL 20 // A intesidade está baixa para não causar incômodo (0-255, caso deseje alterar)
#define LED_MTX_PIN 7

// Definições relacionadas ao controle de tempo e debounce
#define DEBOUNCE_TIME 50
#define TIME to_ms_since_boot(get_absolute_time())

// Definição do pixel GRB
typedef struct pixel_t {
    uint8_t G, R, B;
} led_t;

led_t led_matrix[LED_MTX_COUNT]; // Declaração do buffer de pixels que compõem a matriz

uint32_t led_number_pattern[10] = {
    0b0111001010010100101001110, // Número 0
    0b0010000110101000010000100, // Número 1
    0b0111001000011100001001110, // Número 2
    0b0111001000011100100001110, // Número 3
    0b0101001010011100100000010, // Número 4
    0b0111000010011100100001110, // Número 5
    0b0111001000011100101001110, // Número 6
    0b0111001000000100100000010, // Número 7
    0b0111001010011100101001110, // Número 8
    0b0111001010011100100001110  // Número 9
};

// Variáveis para uso da PIO
PIO pio;
uint sm;

/*
 * Função de inicialização da PIO
 */
void init_pio(uint pin) {
    // Cria programa Pio
    uint offset = pio_add_program(pio0, &ws2812b_program);
    pio = pio0;

    sm = pio_claim_unused_sm(pio, false); // Toma posse de uma máquina PIO

    ws2812b_program_init(pio, sm, offset, pin); // Inicia programa na máquina PIO
}

/*
 * Função de inicialização das GPIOs
 */
void init_gpio() {
    // Inicializa os pinos GPIO do LED e botões
    gpio_init(LED_R_PIN);
    gpio_init(BTN_A_PIN);
    gpio_init(BTN_B_PIN);

    // Define o LED como saída e botões como entrada
    gpio_set_dir(LED_R_PIN, GPIO_OUT);

    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_set_dir(BTN_B_PIN, GPIO_IN);

    // Habilita resistores de pull-up nos botões
    gpio_pull_up(BTN_A_PIN);
    gpio_pull_up(BTN_B_PIN);

    // Inicializa o LED apagado
    gpio_put(LED_R_PIN, 0);
}

/*
 * Atribuição de cor a um LED
 */
void set_led(const uint id, const uint8_t R, const uint8_t G, const uint8_t B) {
    led_matrix[id].R = R;
    led_matrix[id].G = G;
    led_matrix[id].B = B;
}

/*
 * Decodificação do padrão binário para LEDs da matriz
 */
void set_led_by_pattern(uint32_t pattern) {
    uint lvl = LED_MTX_LEVEL;
    for(uint i = 0; i < LED_MTX_COUNT; i++) {
        // Verfica se o bit é 1. Em casos positivos, acende o LED na cor branca com a intensidade setada
        if((pattern >> i) & 1) set_led(i, lvl, lvl, lvl); 
        else set_led(i, lvl, 0, 0); // Caso contrário, deixa o LED na cor vermelha 
    }
}

/*
 * Limpeza do buffer de LEDs
 */
void clear_leds() {
    for (uint i = 0; i < LED_MTX_COUNT; i++) {
        led_matrix[i].R = 0;
        led_matrix[i].G = 0;
        led_matrix[i].B = 0;
    }
}

uint32_t rgb_value(uint8_t B, uint8_t R, uint8_t G){
  return (G << 24) | (R << 16) | (B << 8);
}

/*
 * Transferência dos valores do buffer para a matriz de LEDs
 */
void write_leds() {
    uint32_t value;
    for (uint i = 0; i < LED_MTX_COUNT; ++i) {
        value = rgb_value(led_matrix[i].B, led_matrix[i].R, led_matrix[i].G);
        pio_sm_put_blocking(pio, sm, value);
    }
}

volatile int number = 0; // Variável global para armazenar o número atual
volatile unsigned long debounce_time; // Variável para controle de debounce

void button_callback(uint gpio, uint32_t events) {
    // Verifica se o tempo desde o último acionamento é maior que o tempo de debounce
    if(TIME - debounce_time > DEBOUNCE_TIME) {   
        if (events & GPIO_IRQ_EDGE_FALL) { // Verifica se o evento foi uma borda de descida (pressionamento do botão)
            if (gpio == BTN_A_PIN) {
                number = (number + 1) % 10; // Incrementa o número, retornando a 0 se ultrapassar 9
            } else if (gpio == BTN_B_PIN) {
                number = (number - 1 + 10) % 10; // Decrementa o número, voltando para 9 se for menor que 0
            }
            set_led_by_pattern(led_number_pattern[number]);
            write_leds(); 
        }
    }
}

int main() {
    stdio_init_all();
    init_pio(LED_MTX_PIN);
    clear_leds();
    init_gpio();

    set_led_by_pattern(led_number_pattern[number]); // Atualiza o padrão de LEDs (Inicia como 0)
    write_leds(); // Envia os dados para os LEDs

    // Configura interrupções para os botões com um callback único
    gpio_set_irq_enabled_with_callback(BTN_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    gpio_set_irq_enabled_with_callback(BTN_B_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    while (true) {
        // Pisca o LED vermelho 5 vezes por segundo
        sleep_ms(100);
        gpio_put(LED_R_PIN, 0);
        sleep_ms(100);
        gpio_put(LED_R_PIN, 1);
    }
}