// --- WIFI ---
#include <ESP8266WiFi.h>
const char* ssid = "...";
const char* password = "...";
WiFiClient nodemcuClient;

// --- MQTT ---
#include <PubSubClient.h>
const char* mqtt_broker = "mqtt.eclipseprojects.io";
const char* mqtt_clientId = "alura-exercicio-iot";
PubSubClient client(nodemcuClient);
const char* topicoTemperatura = "labmcoelho/temperatura";
const char* topicoUmidade = "labmcoelho/umidade";

// --- DHT ---
#include <DHT.h>
#define DHTPIN D3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int umidade;
int temperatura;

// --- DISPLAY ---
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define SCREEN_ADDRESS 0x3C

// --- SETUP ---
void setup() {
  dht.begin();

  configurarDisplay();

  conectarWifi();

  client.setServer(mqtt_broker, 1883);
}

// --- LOOP ---
void loop() {
  if (!client.connected()){
    reconectarMQTT();
  }

  medirTemperaturaEUmidade();

  publicarTemperaturaEUmidadeNoTopico();

  mostrarTemperaturaEUmidade();
}

// --- FUNCOES AUXILIARES ---

// --- MEDICAO DE TEMPERATURA E UMIDADE ---
void medirTemperaturaEUmidade() {
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature(false);   // false = graus celsius
  delay(5000);
}

// --- CONFIGURA O DISPLAY ---
void configurarDisplay() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    //Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.clearDisplay();

}

// --- MOSTRA TEMPERATURA e UMIDADE ---
void mostrarTemperaturaEUmidade() {
  mostrarMensagemNoDisplay("Temperat.", (temperatura), " C");
  mostrarMensagemNoDisplay("Umidade", (umidade), " %");
}

// --- EXIBE TEMPERATURA E UMIDADE NO DISPLAY ---
void mostrarMensagemNoDisplay(const char* texto1, int medicao, const char* texto2) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print(texto1);

  display.setTextSize(5);
  display.setCursor(20,20);
  display.print(medicao);

  display.setTextSize(2);
  display.print(texto2);

  display.display();

  delay(2000);
}

// --- CONFIGURACAO E CONEXAO WIFI ---
void conectarWifi() {
  delay(10);
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("Conectando");
  display.display();
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
    display.display();
  }
}

// --- RECONECTAR CLIENTE MQTT ---
void reconectarMQTT() {
  while (!client.connected()) {
    client.connect(mqtt_clientId);
  }
}

// --- PUBLICA (MQTT) TEMPERATURA E UMIDADE ---
void publicarTemperaturaEUmidadeNoTopico() {
  client.publish(topicoTemperatura, String(temperatura).c_str(), true);
  client.publish(topicoUmidade, String(umidade).c_str(), true);
}
