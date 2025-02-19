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

// Definição dos pinos GPIO para os componentes do hardware
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

// Matrizes para outras letras e uma matriz para limpar a tela
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

// Textos para exibição no display OLED
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

// Função para limpar uma linha da matriz de LEDs
void clearMatrizGame2(){
    for(int i = 0; i < 5; i++){
        for(int j = 0 ; j < 5; j++){
            game2[i][j] = (Led_config){0.0, 0.0, 0.0};
        }
    }
    imprimir_desenho(game2, pio, sm);
    
}

// Função para colorir uma célula da matriz de LEDs com base em uma cor específica
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
    case 'C':
        game2[i][j] = (Led_config){0.0, 0.05, 0.05};
        break;
    case 'W':
        game2[i][j] = (Led_config){0.05, 0.05, 0.05};
        break;
    
    default:
        break;
    }
    
}

// Função para gerar um som com uma frequência e duração específicas
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

// Função para tocar um som com base em uma letra específica
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

// Função para inicializar os pinos GPIO, matriz de LEDs, I2C e ADC
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

// Função para controlar os LEDs RGB
void leds(int g, int b, int r){
    gpio_put(ledGrenn, g);
    gpio_put(ledBlue, b);
    gpio_put(ledRed, r);
}

// Função para verificar a entrada do jogador e atualizar a matriz de LEDs e o som
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

// Função para verificar a entrada do jogador e atualizar a matriz de LEDs, o som e o display OLED
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

// Função para escolher aleatoriamente uma letra entre 'A', 'B' e 'J'
char choice(){
        // Array de caracteres para escolher
    char caracteres[] = {'A', 'B', 'J', 'B', 'A', 'A', 'B', 'J'};
    
    // Inicializa o gerador de números aleatórios com base no tempo atual
    srand(time(NULL));
    
    // Escolhe um índice aleatório entre 0 e 7
    int indice = rand() % 8;

    return caracteres[indice];
}

// Função para exibir texto no display OLED
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

//Contagem regressiva
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

//Função para exibir o menu principal de jogos
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

//Função do jogo1: Jogo da memória
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
    char caracteres[] = {'R', 'G', 'B', 'Y', 'C', 'W'}; //Lista de caracteres usados durante o jogo
    char sequencia[5]; //Araay que armazena a sequência sorteada para o jogo
    char playerChoice[5]; // Array que armazena a sequência definida pelo jogador
    int j = 0; // Variável de controle para alternar entre as cores
    bool situation = false; // Variável de controle para inicio do game

    //Texto que será modificado para mostrar informações no display
    char *text[8];
    for (int i = 0; i < 8; i++) {
        text[i] = malloc(32 * sizeof(char)); 
        if (text[i] == NULL) {
            printf("Erro ao alocar memória\n");
        }
    }

    //Loop principal do jogo 
    while(true){
        char status[6] = {'\0'}; //String para armazenar a sequência definida pelo jogador e mostrá-la no display
        bool sequenciaVisited[5] = {false}; // Array para controle da verificação de sequência correta

        //Verifica se está na condição para mostrar a tela de início do game
        if(!situation){
            displayOled(start, count_of(start)); // Mostra as informações iniciais no display
            clearMatrizGame2(); //Limpa a matriz de leds
            //Aguarda que alguma decisão seja tomada
            while(true){
                if(!gpio_get(btnA)) break; // Quebra o loop  e dá inicio ao jogo
                if(!gpio_get(btnB)) return; // Retorna da função jogo2() para a main
                sleep_ms(5); // Deboucing
            }
            situation = true; //Reseta para que que a tela de início seja ignorada no decorer do jogo
            regressiva(); // Inicia a contagem regresiva para o jogo
        }

        //Montagem da sequência das 5 cores
        for(int i = 0;i<5;i++){
            sequencia[i] = caracteres[rand() % 6];
            printf("%c ", sequencia[i]);
        }
        displayOled(sequenciaGame, count_of(sequenciaGame)); //Exibe uma mensagem no display instruindo o jogador a montar sua sequência

        //Jogador tem 5 chances para montar a sequência correta
        for(int k = 0; k < 5; k++){
            int acertos = 0;
            for(int i = 0; i < 5;){
            colorMatriz(caracteres[j], k, i);
            imprimir_desenho(game2, pio, sm);
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
                        colorMatriz(caracteres[j], k, i);
                        imprimir_desenho(game2, pio, sm);
                        sleep_ms(200);
                    }else if(bar_y_pos > 24 && j < 5){
                        j++;
                        colorMatriz(caracteres[j], k, i);
                        imprimir_desenho(game2, pio, sm);
                        sleep_ms(200);
                    }
                }
            }
            putchar('\n');
            //Verificação da sequência do jogador com a montada pelo sistema
            for(int i = 0 ; i<5;i++){
                bool encontrado = false;
                if(playerChoice[i] == sequencia[i]){
                    status[i] = '*';
                    sequenciaVisited[j] = true;
                    printf("* ");
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
                        if(i != j && playerChoice[i] == sequencia[j] && !sequenciaVisited[j] && playerChoice[j] != sequencia[j]){
                            status[i] = '_';
                            printf("# ");
                            encontrado = true;
                            sequenciaVisited[j] = true;
                            break;
                        }
                    }
                    if(!encontrado){
                        status[i] = 'x';
                        printf("x ");
                    }
                }
            }
            snprintf(text[0], 32, "               ");
            snprintf(text[1], 32, "*: certo       ");
            snprintf(text[2], 32, "_: lugar errado");
            snprintf(text[3], 32, "x: ausente     ");
            snprintf(text[4], 32, "               ");
            snprintf(text[5], 32, "     %s    ", status);
            snprintf(text[6], 32, "               ");
            snprintf(text[7], 32, "               ");
            displayOled(text, count_of(text));
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

        //Verifica se o usuário fez a troca entre as opções de jogos
        if(switchGame && bar_y_pos < 16){
            switchGame = !switchGame;
            mainMenu(menu, count_of(menu), bar_y_pos);
        }else if(!switchGame && bar_y_pos > 24){
            switchGame = !switchGame;
            mainMenu(menu, count_of(menu), bar_y_pos);
        }

        //Verifica se o jogador selecinou alguma opçãode jogo
        if(!gpio_get(btnJ) && switchGame){
            jogo1();
            mainMenu(menu, count_of(menu), 25);
        }else if(!gpio_get(btnJ) && !switchGame){
            jogo2();
            mainMenu(menu, count_of(menu), 15);
        }
        sleep_ms(50); // Deboucing
    }
}