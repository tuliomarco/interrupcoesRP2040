# Tarefa_Aula_27: Controle de Matriz de LEDs WS2812B com Botões

## Descrição
Este projeto utiliza o microcontrolador **RP2040** para controlar uma matriz de LEDs endereçáveis **WS2812B** e realizar a interação com o usuário por meio de dois botões (A e B). O objetivo é exibir números de 0 a 9 na matriz de LEDs de maneira visualmente clara, com a possibilidade de incrementar ou decrementar o número exibido a partir dos botões.

## Funcionalidades Principais
- Controle de uma matriz de LEDs **WS2812B** (25 LEDs endereçáveis).
- Exibição de números de 0 a 9, com padrões binários representando os números na matriz.
- Piscar do LED vermelho (parte do LED RGB) 5 vezes por segundo.
- Aumento e diminuição do número exibido na matriz usando os botões:
  - **Botão A**: Incrementa o número exibido.
  - **Botão B**: Decrementa o número exibido.
- Os números são representados na matriz em estilo digital, utilizando segmentos iluminados para formar os números.

## Tecnologias de Software Utilizadas
- Linguagem: **C**.
- Ferramentas: Kit de desenvolvimento de software SDK do Raspberry Pi Pico

## Como Executar
1. Certifique-se de que o hardware está montado conforme o projeto:
   - Raspberry Pi Pico conectado à matriz de LEDs WS2812B.
   - LED e botões A e B conectados às GPIOs apropriadas.
2. Compile o código no Raspberry Pi Pico utilizando o arquivo **CMakeLists**.
3. Alimente o Raspberry Pi Pico e observe o LED vermelho piscando.
4. Pressione o botão A ou B para alterar o número exibido na matriz de LEDs.

## Demonstração
Assista ao funcionamento do projeto neste [vídeo demonstrativo](https://drive.google.com/file/d/1UBNjed4hO4BNVe3I1Lz-pr53m3Wg95Gi/view?usp=sharing).
