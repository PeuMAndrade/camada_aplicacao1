# Projeto de Monitoramento e Controle com ESP32 e Arduino IoT Cloud

Este projeto utiliza um microcontrolador ESP32 para monitorar sensores de temperatura, luminosidade e um potenciômetro, enviando os dados para a plataforma Arduino IoT Cloud. Além disso, permite o controle de um LED RGB e um buzzer de acordo com as leituras dos sensores e comandos da nuvem.

## Funcionalidades

- **Monitoramento de Sensores:** Leitura contínua de temperatura (DHT11), nível de luz (LDR) e valor de um potenciômetro.
- **Integração com Arduino IoT Cloud:** Sincronização em tempo real das variáveis dos sensores e atuadores com um dashboard na nuvem.
- **Controle de Atuadores:**
    - **LED RGB:** A cor do LED é controlada pelo potenciômetro, criando um gradiente de cores. O LED também pode ser controlado pela nuvem.
    - **Buzzer:** Dispara um alarme sonoro caso a temperatura saia de uma faixa pré-definida (0°C a 30°C).
- **Sistema de Liga/Desliga:** Um botão físico permite ligar e desligar todo o sistema de monitoramento e controle.
- **Segurança:** O LED e o buzzer são desligados automaticamente em condições específicas (baixa luminosidade ou temperatura fora da faixa) para economia de energia e segurança.

## Hardware Necessário

- ESP32 DevKit
- Sensor de Temperatura e Umidade DHT11
- LDR (Resistor Dependente de Luz)
- Potenciômetro
- LED RGB (Ânodo Comum)
- Botão (Push Button)
- Buzzer
- Protoboard e Jumpers

## Software e Bibliotecas

- Arduino IDE ou editor compatível (VS Code com PlatformIO)
- **Bibliotecas Arduino:**
    - `ArduinoIoTCloud`
    - `Arduino_ConnectionHandler`
    - `DHT`

## Pinagem (ESP32 DevKit)

| Componente | Pino no ESP32 |
| :--- | :--- |
| Botão | GPIO 15 |
| LDR | GPIO 36 |
| Sensor DHT11 | GPIO 14 |
| Potenciômetro | GPIO 39 |
| LED Vermelho (R) | GPIO 26 |
| LED Verde (G) | GPIO 33 |
| LED Azul (B) | GPIO 32 |
| Buzzer | GPIO 25 |

## Instalação e Configuração

1.  **Montagem do Circuito:** Monte o circuito em uma protoboard seguindo a pinagem descrita acima.
2.  **Configuração da Arduino IoT Cloud:**
    - Crie um "Thing" na sua conta da Arduino IoT Cloud.
    - Configure as seguintes variáveis na nuvem:
        - `led` (CloudColoredLight, Read/Write)
        - `temperatura` (CloudTemperatureSensor, Read-Only)
        - `ldr` (CloudIlluminance, Read-Only)
        - `potenciometro` (Integer, Read-Only)
        - `sistema` (Boolean, Read-Only)
3.  **Credenciais de Rede:**
    - No arquivo `arduino_secrets.h`, adicione o nome (SSID) e a senha da sua rede Wi-Fi.
    - O `DEVICE_KEY` será gerado pela Arduino IoT Cloud e deve ser colado em `thingProperties.h` (ou `arduino_secrets.h`).
4.  **Upload do Código:**
    - Abra o arquivo `.ino` na Arduino IDE.
    - Selecione a placa "ESP32 Dev Module" (ou similar).
    - Compile e envie o código para a sua placa ESP32.
5.  **Dashboard:**
    - Crie um dashboard na Arduino IoT Cloud e vincule widgets às variáveis do seu "Thing" para visualizar os dados e controlar o LED.
