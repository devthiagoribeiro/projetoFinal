#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/adc.h"
#include "matriz_leds.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"

#define buzzerA 21
#define buzzerB 10
#define ledGrenn 11
#define ledRed 13
#define ledBlue 12
#define btnA 5
#define btnB 6
#define btnJ 22
#define VRY 26
#define VRX 26
#define FREQ_A 440  // Nota A (440 Hz)
#define FREQ_B 494  // Nota B (494 Hz)
#define FREQ_C 523  // Nota C (523 Hz)
uint dificuldade = 500;
PIO pio;
uint sm;
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;
struct render_area *frame_area_ptr; //Variável global para conseguir manipular o display oled em qualquer função

Matriz_leds_config letraA = {
    //       Coluna 0          Coluna 1          Coluna 2          Coluna 3          Coluna 4
    //R    G    B       R    G    B       R    G    B       R    G    B       R    G    B
    {{0.0, 0.0, 0.0}, {0.05, 0.05, 0.05}, {0.05, 0.05, 0.05}, {0.05, 0.05, 0.05}, {0.0, 0.0, 0.0}}, // Linha 0
    {{0.0, 0.0, 0.0}, {0.05, 0.05, 0.05}, {0.0, 0.0, 0.0}, {0.05, 0.05, 0.05}, {0.0, 0.0, 0.0}}, // Linha 1
    {{0.0, 0.0, 0.0}, {0.05, 0.05, 0.05}, {0.05, 0.05, 0.05}, {0.05, 0.05, 0.05}, {0.0, 0.0, 0.0}}, // Linha 2
    {{0.0, 0.0, 0.0}, {0.05, 0.05, 0.05}, {0.0, 0.0, 0.0}, {0.05, 0.05, 0.05}, {0.0, 0.0, 0.0}}, // Linha 3
    {{0.0, 0.0, 0.0}, {0.05, 0.05, 0.05}, {0.0, 0.0, 0.0}, {0.05, 0.05, 0.05}, {0.0, 0.0, 0.0}}  // Linha 4
    };
Matriz_leds_config letraB = {
    //       Coluna 0          Coluna 1          Coluna 2          Coluna 3          Coluna 4
    //R    G    B       R    G    B       R    G    B       R    G    B       R    G    B
    {{0.0, 0.0, 0.0}, {0.05, 0.05, 0.05}, {0.05, 0.05, 0.05}, {0.05, 0.05, 0.05}, {0.0, 0.0, 0.0}}, // Linha 0
    {{0.0, 0.0, 0.0}, {0.05, 0.05, 0.05}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.05, 0.05, 0.05}}, // Linha 1
    {{0.0, 0.0, 0.0}, {0.05, 0.05, 0.05}, {0.05, 0.05, 0.05}, {0.05, 0.05, 0.05}, {0.0, 0.0, 0.0}}, // Linha 2
    {{0.0, 0.0, 0.0}, {0.05, 0.05, 0.05}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.05, 0.05, 0.05}}, // Linha 3
    {{0.0, 0.0, 0.0}, {0.05, 0.05, 0.05}, {0.05, 0.05, 0.05}, {0.05, 0.05, 0.05}, {0.0, 0.0, 0.0}}  // Linha 4
    };
Matriz_leds_config letraJ = {
    //       Coluna 0          Coluna 1          Coluna 2          Coluna 3          Coluna 4
    //R    G    B       R    G    B       R    G    B       R    G    B       R    G    B
    {{0.05, 0.05, 0.05}, {0.05, 0.05, 0.05}, {0.05, 0.05, 0.05}, {0.05, 0.05, 0.05}, {0.05, 0.05, 0.05}}, // Linha 0
    {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.05, 0.05, 0.05}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 1
    {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.05, 0.05, 0.05}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 2
    {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.05, 0.05, 0.05}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 3
    {{0.05, 0.05, 0.05}, {0.05, 0.05, 0.05}, {0.05, 0.05, 0.05}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}  // Linha 4
    };
Matriz_leds_config clear = {
    //       Coluna 0          Coluna 1          Coluna 2          Coluna 3          Coluna 4
    //R    G    B       R    G    B       R    G    B       R    G    B       R    G    B
    {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 0
    {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 1
    {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 2
    {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 3
    {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}  // Linha 4
    };
Matriz_leds_config game2 = {
    //       Coluna 0          Coluna 1          Coluna 2          Coluna 3          Coluna 4
    //R    G    B       R    G    B       R    G    B       R    G    B       R    G    B
    {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 0
    {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 1
    {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 2
    {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 3
    {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}  // Linha 4
    };

char *start[8] = {
    //"             "
    "               ",
    "               ",
    "   Aperte A    ",
    " para comecar  ",
    "   ou B para   ",
    "    voltar     ",
    "               ",
    "               "
    };
char *correto[8] = {
    //"             "
    "               ",
    "               ",
    "   Sequencia   ",
    "               ",
    "    correta!   ",
    "               ",
    "               ",
    "               "
    };
char *menu[8] = {
        //"             "
        "               ",
        "               ",
        "     Memoria   ",
        "               ",
        "               ",
        "     Puzzle    ",
        "               ",
        "               "
        };
char *sequenciaGame[8] = {
        //"             "
        "               ",
        "               ",
        "    Monte a    ",
        "               ",
        "   sequencia   ",
        "               ",
        "               ",
        "               "
        };
char *youLost[8] = {
        //"             "
        "               ",
        "  Voce perdeu  ",
        "               ",
        "     Tente     ",
        "               ",
        "   novamente   ",
        "               ",
        "               "
        };


void clearLine(int i){
    for(int j = 0 ; j < 5; j++){
        game2[i][j] = (Led_config){0.0, 0.0, 0.0};
    }
}

void colorMatriz(char color, int i, int j){
    switch (color)
    {
    case 'R':
        game2[i][j] = (Led_config){0.05, 0.0, 0.0};
        break;
    case 'G':
        game2[i][j] = (Led_config){0.0, 0.05, 0.0};
        break;
    case 'B':
        game2[i][j] = (Led_config){0.0, 0.0, 0.05};
        break;
    case 'Y':
        game2[i][j] = (Led_config){0.05, 0.05, 0.0};
        break;
    case 'P':
        game2[i][j] = (Led_config){0.0, 0.05, 0.05};
        break;
    case 'W':
        game2[i][j] = (Led_config){0.05, 0.05, 0.05};
        break;
    
    default:
        break;
    }
    
}

void beep_tone(int frequency, int duration_ms) {
    int delay = 1000000 / (2 * frequency);  // Tempo para meio ciclo da onda (microsegundos)
    int cycles = (duration_ms * 1000) / (2 * delay);  // Número de alternâncias necessárias

    for(int i = 0; i < cycles; i++) {
        gpio_put(buzzerA, 1);
        gpio_put(buzzerB, 1);
        sleep_us(delay);
        gpio_put(buzzerA, 0);
        gpio_put(buzzerB, 0);
        sleep_us(delay);
    }
}

void play_sound(char letter) {
    switch(letter) {
        case 'A':
            beep_tone(FREQ_A, 300); // 300ms de duração
            break;
        case 'B':
            beep_tone(FREQ_B, 300);
            break;
        case 'J':
            beep_tone(FREQ_C, 300);
            break;
        default:
            printf("Letra inválida!\n");
    }
}

void inicializar(){
    //Inicializa os pinos GPIO
    gpio_init(buzzerA);
    gpio_init(buzzerB);
    gpio_set_dir(buzzerA, GPIO_OUT);
    gpio_set_dir(buzzerB, GPIO_OUT);
    gpio_init(ledGrenn);
    gpio_set_dir(ledGrenn, GPIO_OUT);
    gpio_init(ledBlue);
    gpio_set_dir(ledBlue, GPIO_OUT);
    gpio_init(ledRed);
    gpio_set_dir(ledRed, GPIO_OUT);
    gpio_init(btnA);
    gpio_set_dir(btnA, GPIO_IN);
    gpio_pull_up(btnA);
    gpio_init(btnB);
    gpio_set_dir(btnB, GPIO_IN);
    gpio_pull_up(btnB);
    gpio_init(btnJ);
    gpio_set_dir(btnJ, GPIO_IN);
    gpio_pull_up(btnJ);
    // inicialização da matriz de leds;
    pio = pio0;
    sm = configurar_matriz(pio);

    // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000); 
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();

    // Inicializa o conversor analógico-digital (ADC)
    adc_init();
    // Configura os pinos GPIO 26 e 27 como entradas de ADC (alta impedância, sem resistores pull-up)
    adc_gpio_init(VRY);
    adc_gpio_init(VRX);
}

void leds(int g, int b, int r){
    gpio_put(ledGrenn, g);
    gpio_put(ledBlue, b);
    gpio_put(ledRed, r);
}

void beep(int duration){
    for(int i = 0; i < duration; i++){
        gpio_put(buzzerA, 1);
        gpio_put(buzzerB, 1);
        sleep_ms(1);
        gpio_put(buzzerA, 0);
        gpio_put(buzzerB, 0);
        sleep_ms(1);
    }
}

void tutorial(){
    printf("Tutorial:\n");
    printf("Botão A - Led Verde\n");
    leds(1, 0, 0);
    sleep_ms(1000);
    leds(0, 0, 0);
    sleep_ms(1000);
    printf("Botão B - Led Azul\n");
    leds(0, 1, 0);
    sleep_ms(1000);
    leds(0, 0, 0);
    sleep_ms(1000);
    printf("Botão Joystick - Led Vermelho\n");
    leds(0, 0, 1);
    sleep_ms(1000);
    leds(0, 0, 0);
    sleep_ms(1000);
    printf("Palma - Led Branco\n");
    leds(1, 1, 1);
    sleep_ms(1000);
    leds(0, 0, 0);
    sleep_ms(1000);
}

void verify(char key){
    if(key == 'A'){
        leds(1, 0, 0);
        imprimir_desenho(letraA, pio, sm);
        play_sound(key);
        sleep_ms(dificuldade);
        leds(0, 0, 0);
        imprimir_desenho(clear, pio, sm);
        sleep_ms(dificuldade);
    }else if(key == 'B'){
        leds(0, 1, 0);
        imprimir_desenho(letraB, pio, sm);
        play_sound(key);
        sleep_ms(dificuldade);
        leds(0, 0, 0);
        imprimir_desenho(clear, pio, sm);
        sleep_ms(dificuldade);
    }else if(key == 'J'){
        leds(0, 0, 1);
        imprimir_desenho(letraJ, pio, sm);
        play_sound(key);
        sleep_ms(dificuldade);
        leds(0, 0, 0);
        imprimir_desenho(clear, pio, sm);
        sleep_ms(dificuldade);
    }
    
}

void verifyPlayer(char key, int j, int i){
    char *text[8];
    for (int i = 0; i < 8; i++) {
        text[i] = malloc(32 * sizeof(char)); // Aloca espaço suficiente para cada string
        if (text[i] == NULL) {
            printf("Erro ao alocar memória\n");
        }
    }
    if(key == 'A'){
        leds(1, 0, 0);
        imprimir_desenho(letraA, pio, sm);
        snprintf(text[0], 32, "               ");
        snprintf(text[1], 32, "               ");
        snprintf(text[2], 32, "               ");
        snprintf(text[3], 32, "  Rodada:  %d  ", i+1);
        snprintf(text[4], 32, "               ");
        snprintf(text[5], 32, "     %d/%d     ",j+1, i+1);
        snprintf(text[6], 32, "               ");
        snprintf(text[7], 32, "               ");
        displayOled(text, count_of(text));
        play_sound(key);
        sleep_ms(dificuldade);
        leds(0, 0, 0);
        imprimir_desenho(clear, pio, sm);
        sleep_ms(dificuldade);
    }else if(key == 'B'){
        leds(0, 1, 0);
        imprimir_desenho(letraB, pio, sm);
        snprintf(text[0], 32, "               ");
        snprintf(text[1], 32, "               ");
        snprintf(text[2], 32, "               ");
        snprintf(text[3], 32, "  Rodada:  %d  ", i+1);
        snprintf(text[4], 32, "               ");
        snprintf(text[5], 32, "     %d/%d     ",j+1, i+1);
        snprintf(text[6], 32, "               ");
        snprintf(text[7], 32, "               ");
        displayOled(text, count_of(text));
        play_sound(key);
        sleep_ms(dificuldade);
        leds(0, 0, 0);
        imprimir_desenho(clear, pio, sm);
        sleep_ms(dificuldade);
    }else if(key == 'J'){
        leds(0, 0, 1);
        imprimir_desenho(letraJ, pio, sm);
        snprintf(text[0], 32, "               ");
        snprintf(text[1], 32, "               ");
        snprintf(text[2], 32, "               ");
        snprintf(text[3], 32, "  Rodada:  %d  ", i+1);
        snprintf(text[4], 32, "               ");
        snprintf(text[5], 32, "     %d/%d     ",j+1, i+1);
        snprintf(text[6], 32, "               ");
        snprintf(text[7], 32, "               ");
        displayOled(text, count_of(text));
        play_sound(key);
        sleep_ms(dificuldade);
        leds(0, 0, 0);
        imprimir_desenho(clear, pio, sm);
        sleep_ms(dificuldade);
    }
    
}

char choice(){
        // Array de caracteres para escolher
    char caracteres[] = {'A', 'B', 'J', 'B', 'A', 'A', 'B', 'J'};
    
    // Inicializa o gerador de números aleatórios com base no tempo atual
    srand(time(NULL));
    
    // Escolhe um índice aleatório entre 0 e 7
    int indice = rand() % 8;

    return caracteres[indice];
}

void displayOled(char *text[8],size_t line_count){
    // zera o display inteiro
        uint8_t ssd[ssd1306_buffer_length];
        memset(ssd, 0, ssd1306_buffer_length);
        render_on_display(ssd, frame_area_ptr);

        srand(time(NULL));

        restart:

        // Criação do Texto de Exibição

        int y = 0;
        for (uint i = 0; i < line_count; i++)
        {
            ssd1306_draw_string(ssd, 5, y, text[i]);
            y += 8;
        }
        border0(ssd);
        render_on_display(ssd, frame_area_ptr);
}

void regressiva(){
    char *tres[8] = {
        //"             "
        "               ",
        "               ",
        "   Comecando   ",
        "               ",
        "      em: 3    ",
        "               ",
        "               ",
        "               "
        };
    char *dois[8] = {
        //"             "
        "               ",
        "               ",
        "   Comecando   ",
        "               ",
        "      em: 2    ",
        "               ",
        "               ",
        "               "
        };
    char *um[8] = {
        //"             "
        "               ",
        "               ",
        "   Comecando   ",
        "               ",
        "      em: 1    ",
        "               ",
        "               ",
        "               "
        };
        displayOled(tres, count_of(tres));
        sleep_ms(1000);
        displayOled(dois, count_of(dois));
        sleep_ms(1000);
        displayOled(um, count_of(um));
        sleep_ms(1000);
}

void mainMenu(char *text[8], size_t line_count, uint y_pos){
    // zera o display inteiro
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, frame_area_ptr);

    srand(time(NULL));

    restart:

    // Criação do Texto de Exibição

    int y = 0;
    for (uint i = 0; i < line_count; i++)
    {
        ssd1306_draw_string(ssd, 5, y, text[i]);
        y += 8;
    }
    border0(ssd);
    if(y_pos < 16){
        draw_arrow(ssd, 20, 43);
    }else if(y_pos > 24){
        draw_arrow(ssd, 20, 20);
    }
    render_on_display(ssd, frame_area_ptr);
}

void jogo1(){
    char sequencia[100]; // 'A', 'B', 'J'
    int key = 0;
    char alt;
    bool situation = false;
    int record = 0;
    char *text[8];
    for (int i = 0; i < 8; i++) {
        text[i] = malloc(32 * sizeof(char)); // Aloca espaço suficiente para cada string
        if (text[i] == NULL) {
            printf("Erro ao alocar memória\n");
        }
    }
    while (true) {

        if(!situation){
            displayOled(start, count_of(start));
            while(true){
                if(!gpio_get(btnA)) break;
                if(!gpio_get(btnB)) return;
                sleep_ms(5);
            }
            situation = true;
            regressiva();
        }

        snprintf(text[0], 32, "               ");
        snprintf(text[1], 32, "               ");
        snprintf(text[2], 32, "               ");
        snprintf(text[3], 32, "  Rodada:  %d  ", key+1);
        snprintf(text[4], 32, "               ");
        snprintf(text[5], 32, "      0/%d     ", key+1);
        snprintf(text[6], 32, "               ");
        snprintf(text[7], 32, "               ");
        displayOled(text, count_of(text));

        sequencia[key] = choice();

        //Mostra a nova sequencia
        for(int j = 0; j <= key; j++){
            verify(sequencia[j]);
            printf("%c ", sequencia[j]);
        }
        putchar('\n');


        //Usuário insere a sequência
        for(int j = 0; j <= key; j++){
            while(true){
                if(!gpio_get(btnA)){
                    alt = 'A';
                    verifyPlayer(alt, j, key);
                    break;
                }else if(!gpio_get(btnB)){
                    alt = 'B';
                    verifyPlayer(alt, j, key);
                    break;
                }else if(!gpio_get(btnJ)){
                    alt = 'J';
                    verifyPlayer(alt, j, key);
                    break;
                }
                sleep_ms(5);
            }
            //Verifica se errou
            if(sequencia[j] != alt){
                situation = false;
                break;
            }
        }
        if(situation){
            key++;
            if(dificuldade > 100) dificuldade -= 50;
            displayOled(correto, count_of(correto));
            sleep_ms(800);
        }else{
            if(key > record) record = key;
            snprintf(text[0], 32, "               ");
            snprintf(text[1], 32, "  Voce perdeu  ");
            snprintf(text[2], 32, "               ");
            snprintf(text[3], 32, "    Pts: %d    ", key);
            snprintf(text[4], 32, "               ");
            snprintf(text[5], 32, "   Record: %d  ", record);
            snprintf(text[6], 32, "               ");
            snprintf(text[7], 32, "               ");
            displayOled(text, count_of(text));
            key = 0;
            sleep_ms(5000);
        }
        
    }
}

void jogo2(){
    char caracteres[] = {'R', 'G', 'B', 'Y', 'P', 'W'};
    char sequencia[5];
    char playerChoice[5];
    int j = 0;
    bool situation = false;
    char *text[8];
    for (int i = 0; i < 8; i++) {
        text[i] = malloc(32 * sizeof(char)); // Aloca espaço suficiente para cada string
        if (text[i] == NULL) {
            printf("Erro ao alocar memória\n");
        }
    }

    while(true){
        char lugarErrado[6] = {'\0'};
        char ausente[6] = {'\0'};

        if(!situation){
            displayOled(start, count_of(start));
            imprimir_desenho(clear, pio, sm);
            while(true){
                if(!gpio_get(btnA)) break;
                if(!gpio_get(btnB)) return;
                sleep_ms(5);
            }
            situation = true;
            regressiva();
        }

        //Montagem da sequência das 5 cores
        for(int i = 0;i<5;i++){
            sequencia[i] = caracteres[rand() % 6];
            printf("%c ", sequencia[i]);
        }
        displayOled(sequenciaGame, count_of(sequenciaGame));
        //Jogador tem 5 chances para montar a sequência correta
        for(int k = 0; k < 5; k++){
            int acertos = 0;
            for(int i = 0; i < 5;){
                while(true){
                    if(!gpio_get(btnA)){
                        sleep_ms(200);
                        playerChoice[i] = caracteres[j];
                        if(i>0) i--;
                        break;
                    }else if(!gpio_get(btnB)){
                        sleep_ms(200);
                        playerChoice[i] = caracteres[j];
                        i++;
                        break;
                    }
                    // Seleciona a entrada ADC 1 (conectada ao eixo Y do joystick)
                    adc_select_input(0);
                    // Lê o valor do ADC para o eixo Y
                    uint adc_y_raw = adc_read();
                    // Configuração para estabilizar os valores dos eixos entre 0 e 39
                    const uint bar_width = 40;
                    const uint adc_max = (1 << 12) - 1; 
                    uint bar_y_pos = adc_y_raw * bar_width / adc_max;
                    if(bar_y_pos < 16 && j > 0 ){
                        j--;
                        printf("playerChoice[%d]: %c\n", i, caracteres[j]);
                        colorMatriz(caracteres[j], k, i);
                        imprimir_desenho(game2, pio, sm);
                        sleep_ms(200);
                    }else if(bar_y_pos > 24 && j < 5){
                        j++;
                        printf("playerChoice[%d]: %c\n", i, caracteres[j]);
                        colorMatriz(caracteres[j], k, i);
                        imprimir_desenho(game2, pio, sm);
                        sleep_ms(200);
                    }
                }
            }
            
            //Verificação da sequência do jogador com a montada pelo sistema
            clearLine(k);
            for(int i = 0 ; i<5;i++){
                bool encontrado = false;
                if(playerChoice[i] == sequencia[i]){
                    colorMatriz(playerChoice[i], k, i);
                    printf("%c ", playerChoice[i]);
                    if(++acertos == 5){
                        snprintf(text[0], 32, "               ");
                        snprintf(text[1], 32, "               ");
                        snprintf(text[2], 32, "  Voce ganhou  ");
                        snprintf(text[3], 32, "               ");
                        snprintf(text[4], 32, "   na sua %d   ", k+1);
                        snprintf(text[5], 32, "               ");
                        snprintf(text[6], 32, "   tentativa   ");
                        snprintf(text[7], 32, "               ");
                        displayOled(text, count_of(text));
                        sleep_ms(5000);
                        situation = false;
                        k = 5;
                        break;
                    } 

                }else {
                    for(int j = 0; j < 5; j++){
                        if(playerChoice[i] == sequencia[j] && playerChoice[j] != sequencia[j]){
                            for(int i = 0; i < 6; i++){
                                if(lugarErrado[i] == '\0'){
                                    lugarErrado[i] = playerChoice[i];
                                    lugarErrado[i+1] = '\0';
                                    break;
                                }
                            }
                            encontrado = true;
                            break;
                        }
                    }
                    if(!encontrado){
                        for(int i = 0; i < 6; i++){
                            if(ausente[i] == '\0'){
                                ausente[i] = playerChoice[i];
                                ausente[i+1] = '\0';
                                break;
                            }
                        }
                    }
                }
            }
            snprintf(text[0], 32, "               ");
            snprintf(text[1], 32, "               ");
            snprintf(text[2], 32, " lugar errado: ");
            snprintf(text[3], 32, " %s ", lugarErrado);
            snprintf(text[4], 32, "               ");
            snprintf(text[5], 32, "   ausente:    ");
            snprintf(text[6], 32, " %s ", ausente);
            snprintf(text[7], 32, "               ");
            displayOled(text, count_of(text));
            imprimir_desenho(game2, pio, sm);
        }
        if(situation){
            situation = false;
            displayOled(youLost, count_of(youLost));
            sleep_ms(5000);
        }
    }
}

int main()
{
    bool switchGame = true;
    inicializar();
    stdio_init_all();

    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);
    frame_area_ptr = &frame_area;
    mainMenu(menu, count_of(menu), 25);


    while(true){
        // Seleciona a entrada ADC 1 (conectada ao eixo Y do joystick)
        adc_select_input(0);
        // Lê o valor do ADC para o eixo Y
        uint adc_y_raw = adc_read();

        // Configuração para estabilizar os valores dos eixos entre 0 e 39
        const uint bar_width = 40;
        const uint adc_max = (1 << 12) - 1; 
        uint bar_y_pos = adc_y_raw * bar_width / adc_max;
        if(switchGame && bar_y_pos < 16){
            switchGame = !switchGame;
            mainMenu(menu, count_of(menu), bar_y_pos);
        }else if(!switchGame && bar_y_pos > 24){
            switchGame = !switchGame;
            mainMenu(menu, count_of(menu), bar_y_pos);
        }
        if(!gpio_get(btnJ) && switchGame){
            jogo1();
            mainMenu(menu, count_of(menu), 25);
        }else if(!gpio_get(btnJ) && !switchGame){
            jogo2();
            mainMenu(menu, count_of(menu), 15);
        }
        sleep_ms(50);
    }
}