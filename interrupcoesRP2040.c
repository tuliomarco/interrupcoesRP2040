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

// Definição do número de LEDs na Matrix a GPIO
#define LED_MTX_COUNT 25
#define LED_MTX_PIN 7

// Definição do pixel GRB
typedef struct pixel_t {
    uint8_t G, R, B;
} led_t;

led_t led_matrix[LED_MTX_COUNT]; // Declaração do buffer de pixels que compõem a matriz

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
 * Limpeza do buffer de LEDs
 */
void clear_leds() {
    for (uint i = 0; i < LED_MTX_COUNT; i++) {
        led_matrix[i].R = 0;
        led_matrix[i].G = 0;
        led_matrix[i].B = 0;
    }
}

/*
 * Transferência dos valores do buffer para a matriz de LEDs
 */
void write_leds() {
    for (uint i = 0; i < LED_MTX_COUNT; ++i) {
        pio_sm_put_blocking(pio, sm, led_matrix[i].G);
        pio_sm_put_blocking(pio, sm, led_matrix[i].R);
        pio_sm_put_blocking(pio, sm, led_matrix[i].B);
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
