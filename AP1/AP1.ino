#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define OUTPUT_PIN 13
#define TRIG_PIN 5
#define ECHO_PIN 18

long duracao;
float distancia;

const char* ssid = "iPhone Muniz";
const char* password = "muniz2001";

const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long ultimaPublicacao = 0;
const unsigned long intervaloPublicacao = 5000;

void setup_wifi() {
  Serial.print("Conectando ao Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  
  Serial.println("\nWi-Fi conectado. IP local: ");
  Serial.println(WiFi.localIP());
}

void enviarDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duracao = pulseIn(ECHO_PIN, HIGH);
  distancia = duracao * 0.034 / 2.0;

  Serial.print("Distância: ");
  Serial.print(distancia);
  Serial.println(" cm");

  
  char msg[10];
  dtostrf(distancia, 6, 2, msg);

  if (distancia < 10.0) {
    if (client.publish("teste/cansat/check", "Tem gente")) {
      Serial.println("Publicação OK!");
    } else {
      Serial.println("Falha ao publicar!");
    }
  } else {
    if (client.publish("teste/cansat/check", "Comodo vazio")) {
      Serial.println("Publicação OK!");
    } else {
      Serial.println("Falha ao publicar!");
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem em [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  Serial.println("=== reconnect() chamado. Tentando conectar MQTT...");
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT... ");
    if (client.connect("ESP32ClientTestMosquitto")) {
      Serial.println("Conectado!");
      client.subscribe("teste/cansat/check");
    } 
    else {
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println(". Tentando novamente em 5s...");
      delay(5000);
    }
  }
  Serial.println("=== reconnect() finalizado: conexao estabelecida.");
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  unsigned long agora = millis();
  if (agora - ultimaPublicacao >= intervaloPublicacao) {
    ultimaPublicacao = agora;
    enviarDistancia();
  }
}