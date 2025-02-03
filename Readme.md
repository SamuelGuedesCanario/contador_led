Link do video https://youtu.be/y2Xazk6eoow

Projeto: Controle de LED, Botões e Buzzer com Raspberry Pi Pico W

Descrição

Este projeto utiliza um Raspberry Pi Pico W para controlar:

Um LED vermelho que pisca 5 vezes por segundo;

Dois botões para aumentar e diminuir um contador exibido em uma matriz de LEDs;

Um buzzer que emite um som correspondente ao valor do contador.

Componentes Utilizados

Raspberry Pi Pico W

LED vermelho (indicador visual)

Buzzer piezoelétrico

Matriz de LEDs

Dois botões de pressão

Resistores de pull-down para os botões

Fios de conexão

Protoboard

Funcionamento

LED Vermelho

Controlado em uma thread separada para piscar a uma taxa fixa de 5 Hz (5 piscadas por segundo).

Botões

Botão A: Diminui o contador (circular de 0 a 9).

Botão B: Aumenta o contador (circular de 0 a 9).

Matriz de LEDs

Exibe o valor atual do contador de 0 a 9.

Buzzer

Emite um som cuja frequência corresponde ao valor do contador.

Estrutura do Código

Configuração de periféricos

Inicializa GPIOs para LED, botões e buzzer.

Configura interrupções para os botões.

Thread para piscar o LED vermelho

Criada usando multicore_launch_core1() para rodar no segundo núcleo do RP2040.

Loop principal

Aguarda eventos dos botões.

Atualiza o display de LEDs e aciona o buzzer quando o contador muda.

Como Executar

Compilar e carregar o código no Raspberry Pi Pico W.

Conectar os componentes conforme o esquema de ligação.

Observar o funcionamento:

O LED vermelho pisca constantemente.

Pressionar os botões altera o contador.

A matriz de LEDs reflete o valor do contador.

O buzzer emite um som correspondente ao valor do contador.

Possíveis Melhorias

Adicionar um display LCD ou OLED para exibir informações adicionais.

Implementar um sistema de debouncing para os botões.

Utilizar um encoder rotativo no lugar dos botões para um controle mais intuitivo.

Criar uma interface de comunicação via Wi-Fi para controlar o sistema remotamente.

Autor

Samuel Canário - Engenharia Elétrica, UFOB

