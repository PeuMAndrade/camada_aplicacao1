#include "arduino_secrets.h"
// ✅ INCLUDES DO ARDUINO IOT CLOUD
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include <DHT.h>
#include "thingProperties.h"

// ✅ PINAGEM ESP32 DEVKIT
#define BOTAO 15
#define LDR_PIN 36
#define DHTPIN 14
#define DHTTYPE DHT11
#define POT_PIN 39
#define RED 26
#define GREEN 33
#define BLUE 32
#define BUZZER 25

DHT sensorDHT11(DHTPIN, DHTTYPE);

volatile bool sistemaLigado = false;
volatile unsigned long ultimoTempo = 0;
const unsigned long debounceDelay = 200;

#define CANAL_R 0
#define CANAL_G 1
#define CANAL_B 2

#define LUZ_MINIMA 800
#define TEMP_MIN 0
#define TEMP_MAX 30

void IRAM_ATTR botaoISR() {
  unsigned long tempoAtual = millis();
  if (tempoAtual - ultimoTempo > debounceDelay) {
    sistemaLigado = !sistemaLigado;
    ultimoTempo = tempoAtual;
  }
}

void setColor(int r, int g, int b) {
  // ✅ ÂNODO COMUM - LÓGICA DIRETA
  // 255 = aceso, 0 = apagado
  ledcWrite(CANAL_R, r);
  ledcWrite(CANAL_G, g);
  ledcWrite(CANAL_B, b);
}

void apagarLED() {
  // ✅ APAGA LED ÂNODO COMUM (todos em LOW = 0)
  ledcWrite(CANAL_R, 0);
  ledcWrite(CANAL_G, 0);
  ledcWrite(CANAL_B, 0);
}

void onLedChange() {
  if (sistemaLigado && led.getSwitch()) {
    float h = led.getHue();
    float s = 1.0;
    float v = 1.0;
    
    float c = v * s;
    float x = c * (1 - abs(fmod(h / 60.0, 2) - 1));
    float m = v - c;
    
    float r_temp, g_temp, b_temp;
    
    if (h >= 0 && h < 60) {
      r_temp = c; g_temp = x; b_temp = 0;
    } else if (h >= 60 && h < 120) {
      r_temp = x; g_temp = c; b_temp = 0;
    } else if (h >= 120 && h < 180) {
      r_temp = 0; g_temp = c; b_temp = x;
    } else if (h >= 180 && h < 240) {
      r_temp = 0; g_temp = x; b_temp = c;
    } else if (h >= 240 && h < 300) {
      r_temp = x; g_temp = 0; b_temp = c;
    } else {
      r_temp = c; g_temp = 0; b_temp = x;
    }
    
    uint8_t r = (r_temp + m) * 255;
    uint8_t g = (g_temp + m) * 255;
    uint8_t b = (b_temp + m) * 255;
    
    setColor(r, g, b);
  } else {
    apagarLED();  // ✅ Apaga corretamente
  }
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  
  pinMode(BOTAO, INPUT_PULLUP);
  pinMode(LDR_PIN, INPUT);
  pinMode(POT_PIN, INPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  
  analogSetAttenuation(ADC_11db);
  
  sensorDHT11.begin();
  
  attachInterrupt(digitalPinToInterrupt(BOTAO), botaoISR, FALLING);
  
  ledcSetup(CANAL_R, 1000, 8);
  ledcSetup(CANAL_G, 1000, 8);
  ledcSetup(CANAL_B, 1000, 8);
  
  ledcAttachPin(RED, CANAL_R);
  ledcAttachPin(GREEN, CANAL_G);
  ledcAttachPin(BLUE, CANAL_B);
  
  // ✅ ARDUINO IOT CLOUD
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
  
  Serial.println("Sistema iniciado!");
}

void loop() {
  ArduinoCloud.update();
  
  static bool ultimoEstado = false;
  
  sistema = sistemaLigado;
  
  if (sistemaLigado != ultimoEstado) {
    ultimoEstado = sistemaLigado;
    if (sistemaLigado) {
      Serial.println("SISTEMA LIGADO");
    } else {
      Serial.println("SISTEMA DESLIGADO");
      apagarLED();  // ✅ Apaga LED corretamente
      noTone(BUZZER);
      led.setSwitch(false);
    }
  }
  
  if (!sistemaLigado) {
    // ✅ GARANTE QUE O LED FIQUE APAGADO
    apagarLED();
    return;
  }
  
  if (sistemaLigado) {
    // Lê sensores
    int ldrValor = analogRead(LDR_PIN);
    int potValor = analogRead(POT_PIN);
    float temperaturaLida = sensorDHT11.readTemperature();
    
    // Atualiza variáveis da nuvem
    ldr = ldrValor;
    potenciometro = potValor;
    if (!isnan(temperaturaLida)) {
      temperatura = temperaturaLida;
    }
    
    bool ledDesligado = false;
    
    // Verifica luminosidade
    if (ldrValor < LUZ_MINIMA) {
      apagarLED();
      led.setSwitch(false);
      ledDesligado = true;
    }
    
    // Verifica temperatura
    if (!isnan(temperaturaLida)) {
      if (temperaturaLida < TEMP_MIN || temperaturaLida > TEMP_MAX) {
        apagarLED();
        tone(BUZZER, 1000);
        led.setSwitch(false);
        ledDesligado = true;
      } else {
        noTone(BUZZER);
      }
    }
    
    // Controla cor do LED pelo potenciômetro
    if (!ledDesligado) {
      // Mapeia potenciômetro para cores RGB
      int r, g, b;
      
      if (potValor < 819) {
        // VERMELHO
        r = 255;
        g = 0;
        b = 0;
      } 
      else if (potValor < 1638) {
        // VERMELHO -> AMARELO
        r = 255;
        g = map(potValor, 819, 1637, 0, 255);
        b = 0;
      } 
      else if (potValor < 2457) {
        // AMARELO -> VERDE
        r = map(potValor, 1638, 2456, 255, 0);
        g = 255;
        b = 0;
      } 
      else if (potValor < 3276) {
        // VERDE -> CIANO
        r = 0;
        g = 255;
        b = map(potValor, 2457, 3275, 0, 255);
      } 
      else {
        // CIANO -> AZUL
        r = 0;
        g = map(potValor, 3276, 4095, 255, 0);
        b = 255;
      }
      
      setColor(r, g, b);
      
      // Atualiza LED na nuvem
      led.setSwitch(true);
      float hue = map(potValor, 0, 4095, 0, 360);
      led.setHue(hue);
      
      Serial.print("POT: ");
      Serial.print(potValor);
      Serial.print(" | RGB(");
      Serial.print(r);
      Serial.print(",");
      Serial.print(g);
      Serial.print(",");
      Serial.print(b);
      Serial.println(")");
    }
    
    delay(100);
  }
}