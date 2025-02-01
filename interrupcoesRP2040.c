#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "ws2812b.pio.h"

// Identificação das GPIOs conectadas aos LEDs RGB
#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12

// Identificação das GPIOs conectadas aos botões A e B
#define BTN_A_PIN 5
#define BTN_B_PIN 6

// Definição do número de LEDs na Matrix, da intensidade dos LEDs e da GPIO
#define LED_MTX_COUNT 25
#define LED_MTX_LEVEL 10
#define LED_MTX_PIN 7

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
    gpio_init(LED_R_PIN);
    gpio_init(LED_G_PIN);
    gpio_init(LED_B_PIN);
    gpio_init(BTN_A_PIN);
    gpio_init(BTN_B_PIN);

    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_set_dir(BTN_B_PIN, GPIO_IN);

    gpio_put(LED_R_PIN, 0);
    gpio_put(LED_G_PIN, 0);
    gpio_put(LED_B_PIN, 0);
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

int main() {
    stdio_init_all();
    init_pio(LED_MTX_PIN);
    clear_leds();
    init_gpio();
    
    while (true) {
        sleep_ms(100);
        gpio_put(LED_R_PIN, 0);
        sleep_ms(100);
        gpio_put(LED_R_PIN, 1);
    }
}
