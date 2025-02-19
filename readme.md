# BrainWave – Central de Jogos Neurocognitivos

## Descrição do Projeto

O **BrainWave** é um sistema interativo desenvolvido para a placa de ensino **BitDogLab**. Ele integra dois jogos de estímulos neurocognitivos, oferecendo uma experiência multissensorial que combina feedback visual, auditivo e tátil. O projeto tem como foco estimular as funções cognitivas do usuário por meio de desafios de memória e percepção, transformando o aprendizado em uma atividade lúdica e interativa.

## Componentes Utilizados

- **BitDogLab:** Placa de ensino para sistemas embarcados.
- **Microcontrolador:** (Modelo utilizado na BitDogLab).
- **Matriz de LEDs:** Para exibição de padrões e feedback visual.
- **Display OLED (SSD1306):** Exibição de mensagens, menus e informações.
- **Buzzer:** Para geração de sons e feedback auditivo.
- **Joystick (com ADC):** Para seleção de opções e navegação.
- **Botões:** Para entrada de comandos e navegação nos menus.   

## Funcionalidades

- **Jogo 1 – Memória:**  
  - **Descrição:** Gera uma sequência aleatória de letras ('A', 'B' e 'J') que o usuário deve reproduzir.  
  - **Objetivo:** Testar e estimular a memória sequencial do jogador.  
  - **Feedback:** Utiliza a matriz de LEDs, o display OLED e sons específicos para cada ação.

- **Jogo 2 – Puzzle de Cores:**  
  - **Descrição:** Sorteia uma sequência de cores que o jogador deve identificar e reproduzir usando o joystick e botões.  
  - **Objetivo:** Desenvolver a percepção visual, a coordenação motora e o raciocínio lógico.  
  - **Feedback:** Fornece orientação visual por meio da matriz de LEDs e do display OLED.

- **Interface Multissensorial:**  
  - Integra estímulos visuais (LEDs e OLED), auditivos (buzzer) e táteis (botões e joystick) para proporcionar uma experiência de jogo imersiva e educativa.

## Como Aplicar o Projeto na Sua Placa

### Pré-requisitos

- Placa **BitDogLab** configurada e conectada ao ambiente de desenvolvimento.
- Ferramentas de compilação e upload (ex.: make, CMake ou IDE compatível com o microcontrolador utilizado).
- Bibliotecas necessárias instaladas (ex.: pico/stdlib, drivers do SSD1306, etc.).

### Passo a Passo

1. Clone este repositório:
   ```sh
   git clone https://github.com/devthiagoribeiro/projetoFinal
2. Compile o código e carregue na placa BitDogLab.
3. Interaja com o joystick e os botões para testar as funcionalidades.

## Demonstração

**🎥[Clique para assistir o vídeo](https://youtu.be)**