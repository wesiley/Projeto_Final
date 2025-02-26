#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "lib/ssd1306.h"
#include "lib/font.h"

// Definições de pinos
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define OLED_ADDRESS 0x3C
#define MIC_PIN 28
#define BUTTON_A 5
#define BUTTON_B 6
#define LED_RED_PIN 13
#define LED_GREEN_PIN 11
#define LED_BLUE_PIN 12

// Definições do display
#define WIDTH 128
#define HEIGHT 64

// Configurações do ADC para detecção de som
const float SOUND_OFFSET = 1.65;
const float SOUND_THRESHOLD_LOW = 0.1;  // Limite para som baixo
const float SOUND_THRESHOLD_MEDIUM = 0.3;  // Limite para som médio
const float SOUND_THRESHOLD_HIGH = 0.5;  // Limite para som alto
const float ADC_REF = 3.3;         // Tensão de referência do ADC
const int ADC_RES = 4095;          // Resolução do ADC (12 bits)

// Estrutura do display
ssd1306_t ssd;

// Variáveis de estado
bool system_active = false;
bool displayUpdated = false;

// Inicialização do I2C
void initI2C() {
    i2c_init(I2C_PORT, 400 * 1000); // Inicializa o I2C com clock de 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

// Inicialização do display
void initDisplay() {
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, OLED_ADDRESS, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false); // Limpa o display
    ssd1306_send_data(&ssd);   // Envia os dados para o display
}

// Função para atualizar o display com uma mensagem
void updateDisplay(const char* line1, const char* line2, const char* line3, const char* line4) {
    ssd1306_fill(&ssd, false); // Limpa o display
    ssd1306_draw_string(&ssd, line1, 0, 0);  // Linha 1
    ssd1306_draw_string(&ssd, line2, 0, 16); // Linha 2
    ssd1306_draw_string(&ssd, line3, 0, 32); // Linha 3
    ssd1306_draw_string(&ssd, line4, 0, 48); // Linha 4
    ssd1306_send_data(&ssd); // Envia os dados para o display
    displayUpdated = true; // Marca que o display foi atualizado
}

// Função para tratar interrupções dos botões
void gpio_irq_handler(uint gpio, uint32_t events) {
    static absolute_time_t lastInterruptTime = 0;
    absolute_time_t now = get_absolute_time();

    // Debounce
    if (absolute_time_diff_us(lastInterruptTime, now) > 200000) {
        if (gpio == BUTTON_A) {
            system_active = true;
            updateDisplay("Sistema Ativo", "Aguardando Som", "Aperte Botao: B", "para desativar");
            printf("Sistema ativado!\n");
        } else if (gpio == BUTTON_B) {
            system_active = false;
            updateDisplay("Sistema Desativado", "Aperte Botao: A", "para ativar!", "");
            printf("Sistema desativado!\n");
        }
    }
    lastInterruptTime = now;
}

// Inicialização dos botões com interrupções
void initButtons() {
    gpio_init(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_pull_up(BUTTON_B);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
}
// Inicialização dos LEDs
void initLEDs() {
    gpio_init(LED_RED_PIN);
    gpio_init(LED_GREEN_PIN);
    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    gpio_put(LED_RED_PIN, 0);   // Desliga o LED vermelho
    gpio_put(LED_GREEN_PIN, 0); // Desliga o LED verde
    gpio_put(LED_BLUE_PIN, 0);  // Desliga o LED azul
}

// Função para controlar os LEDs com base no nível de ruído
void updateLEDs(const char* sound_class) {
    if (strcmp(sound_class, "Baixo") == 0) {
        gpio_put(LED_RED_PIN, 0);    // Desliga vermelho
        gpio_put(LED_GREEN_PIN, 1);  // Liga verde
        gpio_put(LED_BLUE_PIN, 0);   // Desliga azul
    } else if (strcmp(sound_class, "Medio") == 0) {
        gpio_put(LED_RED_PIN, 0);    // Desliga vermelho
        gpio_put(LED_GREEN_PIN, 0);  // Desliga verde
        gpio_put(LED_BLUE_PIN, 1);   // Liga azul
    } else if (strcmp(sound_class, "Alto") == 0) {
        gpio_put(LED_RED_PIN, 1);    // Liga vermelho
        gpio_put(LED_GREEN_PIN, 0);  // Desliga verde
        gpio_put(LED_BLUE_PIN, 0);   // Desliga azul
    }
}


// Função principal
int main() {
    stdio_init_all(); // Inicializa a comunicação serial
    initI2C(); // Inicializa o I2C
    initDisplay(); // Inicializa o display
    initButtons(); // Inicializa os botões
    initLEDs(); // Inicializa os LEDs

    // Inicialização do ADC para o microfone
    adc_init();
    adc_gpio_init(MIC_PIN);
    adc_select_input(2); // Seleciona o canal ADC 2 (GPIO 28)

    // Mensagem inicial no display
    updateDisplay("Detector de Ruido", "Aguardando...", "Aperte Botao: A", "para ativar!");
    printf("Detector de Ruido\n");
    printf("Aguardando...\n");
    printf("Aperte Botao: A para ativar!\n");

    while (true) {
        if (system_active) {
            // Leitura do microfone
            uint16_t raw_adc = adc_read();
            float voltage = (raw_adc * ADC_REF) / ADC_RES;
            float sound_level = fabs(voltage - SOUND_OFFSET);

            // Classifica o som
            const char* sound_class;
            if (sound_level < SOUND_THRESHOLD_LOW) {
                sound_class = "Baixo";
            } else if (sound_level < SOUND_THRESHOLD_MEDIUM) {
                sound_class = "Medio";
            } else {
                sound_class = "Alto";
            }

            // Atualiza o display com o nível de som detectado
            updateDisplay("Sistema Ativo", "Som Detectado:", sound_class, "Aperte Botao: B");
            printf("Som detectado: %s (ADC: %d)\n", sound_class, raw_adc);

            // Atualiza os LEDs com base no nível de ruído
            updateLEDs(sound_class);

            sleep_ms(500); // Aguarda 500 ms antes da próxima leitura
        } else {
            // Desliga todos os LEDs quando o sistema está inativo
            gpio_put(LED_RED_PIN, 0);
            gpio_put(LED_GREEN_PIN, 0);
            gpio_put(LED_BLUE_PIN, 0);
            sleep_ms(100); // Pequeno delay para evitar sobrecarga
        }
    }

    return 0;
}