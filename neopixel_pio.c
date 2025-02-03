#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "ws2818b.pio.h"
#include "pico/multicore.h"


#define LED_PIN_RED 13    // Pino do LED
#define BLINK_DELAY_MS 6000  // 5 piscadas por minuto (6000 ms de atraso entre as piscadas)

// Protótipos das funções
/* ========== CONFIGURAÇÕES ========== */
#define LED_COUNT 25       // Número total de LEDs (5x5)
#define LED_PIN 7          // GPIO para matriz LED
#define BUZZER_PIN 21      // GPIO para buzzer
#define BOTAO_A 5          // Botão - (decremento)
#define BOTAO_B 6          // Botão + (incremento)

// Frequências das notas musicais (Hz)
#define C4 261
#define D4 294
#define E4 329
#define F4 349
#define G4 392
#define A4 440
#define B4 494
#define C5 523
#define D5 587
#define E5 659

/* ========== PROTÓTIPOS DE FUNÇÕES ========== */
void npInit(uint pin);
void npSetLED(uint index, uint8_t r, uint8_t g, uint8_t b);
void npClear(void);
void npWrite(void);
void beep(uint pin, uint frequency, uint duration_ms);
void pwm_init_buzzer(uint pin);
int getIndex(int x, int y);
void atualizarDisplay(int numero);
void configurarBotoes(void);
void gpio_irq_handler(uint gpio, uint32_t events);

/* ========== VARIÁVEIS GLOBAIS ========== */
int contador = 0;  // Armazena o valor atual
static volatile uint32_t last_time = 0;  // Tempo do último evento de interrupção
static volatile bool botao_a_pressionado = false;
static volatile bool botao_b_pressionado = false;


// Sprites dos números (5x5 pixels em vermelho)
int numeros[10][5][5][3] = {
    { // 0
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}},
        {{255,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}},
        {{255,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}},
        {{255,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}},
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}}
    },
    { // 1
        {{0,0,0}, {0,0,0}, {255,0,0}, {0,0,0}, {0,0,0}},
        {{0,0,0}, {0,0,0}, {255,0,0}, {0,0,0}, {0,0,0}},
        {{0,0,0}, {0,0,0}, {255,0,0}, {0,0,0}, {0,0,0}},
        {{0,0,0}, {0,0,0}, {255,0,0}, {0,0,0}, {0,0,0}},
        {{0,0,0}, {0,0,0}, {255,0,0}, {0,0,0}, {0,0,0}}
    },
    { // 2
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}},
        {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}},
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}},
        {{255,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}}
    },
    { // 3
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}},
        {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}},
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}},
        {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}},
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}}
    },
    { // 4
        {{255,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}},
        {{255,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}},
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}},
        {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}},
        {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}}
    },
    { // 5
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}},
        {{255,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}},
        {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}},
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}}
    },
    { // 6
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}},
        {{255,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}},
        {{255,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}},
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}}
    },
    { // 7
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}},
        {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}},
        {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}},
        {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}},
        {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}}
    },
    { // 8
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}},
        {{255,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}},
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}},
        {{255,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}},
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}}
    },
    { // 9
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}},
        {{255,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}},
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}},
        {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {255,0,0}},
        {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}}
    }
};



// Frequências correspondentes a cada número
int frequencias[10] = {C4, D4, E4, F4, G4, A4, B4, C5, D5, E5};

// Estrutura para controle dos LEDs
typedef struct { uint8_t G, R, B; } pixel_t;
pixel_t leds[LED_COUNT];
PIO np_pio;
uint sm;

/* ========== FUNÇÕES DE CONTROLE ========== */

// Configura botões com pull-up interno e interrupções
void configurarBotoes(void) {
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_B);
    gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
}

// Atualiza a matriz com o número especificado
void atualizarDisplay(int numero) {
    npClear();
    for(int y = 0; y < 5; y++) {
        for(int x = 0; x < 5; x++) {
            int pos = getIndex(x, y);
            npSetLED(pos, 
                    numeros[numero][y][x][0],
                    numeros[numero][y][x][1],
                    numeros[numero][y][x][2]);
        }
    }
}

// Mapeamento para matriz serpentina
int getIndex(int x, int y) {
    return (y % 2 == 0) ? (24 - (y * 5 + x)) : (24 - (y * 5 + (4 - x)));
}

/* ========== FUNÇÕES NEO-PIXEL ========== */

void npInit(uint pin) {
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;
    sm = pio_claim_unused_sm(np_pio, true);
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);
    npClear();
    npWrite();
}

void npSetLED(uint index, uint8_t r, uint8_t g, uint8_t b) {
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}

void npClear(void) {
    for(uint i = 0; i < LED_COUNT; i++) 
        npSetLED(i, 0, 0, 0);
}

void npWrite(void) {
    for(uint i = 0; i < LED_COUNT; i++) {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100);
}

/* ========== FUNÇÕES DO BUZZER ========== */

void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();
    pwm_init(slice, &config, true);
    pwm_set_gpio_level(pin, 0);
}

void beep(uint pin, uint freq, uint duration_ms) {
    uint slice = pwm_gpio_to_slice_num(pin);
    float divider = (float)clock_get_hz(clk_sys) / (freq * 4096);
    pwm_set_clkdiv(slice, divider);
    
    // Calcula o valor de wrap manualmente
    uint32_t wrap = (uint32_t)(clock_get_hz(clk_sys) / (freq * divider)) - 1;
    pwm_set_wrap(slice, wrap);
    
    // Define o nível do PWM para 50% (wrap / 2)
    pwm_set_gpio_level(pin, wrap / 2);
    
    // Mantém o tom pelo tempo especificado
    sleep_ms(duration_ms);
    
    // Desliga o PWM
    pwm_set_gpio_level(pin, 0);
    sleep_ms(50);  // Pequena pausa entre bipes
}

/* ========== FUNÇÃO DE INTERRUPÇÃO ========== */

void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    
    // Debouncing: verifica se passou tempo suficiente desde o último evento
    if (current_time - last_time > 200000) {  // 200 ms de debouncing
        last_time = current_time;

        if (gpio == BOTAO_A) {
            botao_a_pressionado = true;
        } else if (gpio == BOTAO_B) {
            botao_b_pressionado = true;
        }
    }
}


void led_blink_task() {
    while (1) {
        gpio_put(LED_PIN_RED, 1);  // Liga o LED
        sleep_ms(100);             // Aguarda 100ms
        gpio_put(LED_PIN_RED, 0);  // Desliga o LED
        sleep_ms(100);             // Aguarda 100ms
    }
}




/* ========== FUNÇÃO PRINCIPAL ========== */
int main() {
    // Inicializações
    stdio_init_all();
    npInit(LED_PIN);
    pwm_init_buzzer(BUZZER_PIN);
    configurarBotoes();
    atualizarDisplay(0);

    // Configura o LED vermelho
    gpio_init(LED_PIN_RED);
    gpio_set_dir(LED_PIN_RED, GPIO_OUT);

    // Inicia a tarefa de piscar o LED em outro núcleo
    multicore_launch_core1(led_blink_task);

    while(1) {
        // Verifica se houve pressionamento dos botões
        if (botao_a_pressionado) {
            contador = (contador - 1 + 10) % 10;
            atualizarDisplay(contador);
            beep(BUZZER_PIN, frequencias[contador], 200);
            botao_a_pressionado = false;
        } else if (botao_b_pressionado) {
            contador = (contador + 1) % 10;
            atualizarDisplay(contador);
            beep(BUZZER_PIN, frequencias[contador], 200);
            botao_b_pressionado = false;
        }

        npWrite();
        sleep_ms(10);
    }
}
