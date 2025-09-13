#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Encoder.h>

// ----------------------
// CONFIGURAÇÃO ETHERNET
// ----------------------
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
IPAddress gateway(192, 168, 20, 254);
IPAddress dns(192, 168, 20,254);

// MQTT Configuration 
const char* mqttServer = "broker.hivemq.com";
const int mqtt_port = 1883;

EthernetClient ethClient;
PubSubClient client(ethClient);

// ----------------------
// ENCODER
// ----------------------

Encoder myEnc(2, 3);
const long intervalo = 1000;      
unsigned long tempoAnterior = 0;
long oldPosition = -999;

// ----------------------
// FUNÇÕES MQTT
// ----------------------
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT...");
    
    // Gera um client ID único
    String clientId = "arduinoMegaRegua_";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println(" conectado!");
      Serial.print("Client ID: ");
      Serial.println(clientId);
    } else {
      Serial.print(" falhou, rc=");
      Serial.print(client.state());
      Serial.print(" - ");
      
      //Código de erro
      switch(client.state()) {
        case -4: Serial.println("Timeout na conexão"); break;
        case -3: Serial.println("Conexão perdida"); break;
        case -2: Serial.println("Falha na conexão de rede"); break;
        case -1: Serial.println("Cliente desconectado"); break;
        case 1: Serial.println("Protocolo inválido"); break;
        case 2: Serial.println("Client ID rejeitado"); break;
        case 3: Serial.println("Servidor indisponível"); break;
        case 4: Serial.println("Credenciais inválidas"); break;
        case 5: Serial.println("Não autorizado"); break;
        default: Serial.println("Erro desconhecido"); break;
      }
      
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // Aguarda porta serial conectar
  }
  
  Serial.println("=== INICIANDO SISTEMA ===");
  Serial.println("Configurando Ethernet...");
  
  // Tenta DHCP primeiro
  if (Ethernet.begin(mac) == 0) {
    Serial.println("DHCP falhou, usando IP fixo...");
    Ethernet.begin(mac, ip, gateway, dns);
  
  } else {
    Serial.println("DHCP conectado com sucesso!");
  }

  // Aguarda estabilizar a conexão
  delay(2000);
  
  Serial.print("IP obtido: ");
  Serial.println(Ethernet.localIP());
  Serial.print("Gateway: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("DNS: ");
  Serial.println(Ethernet.dnsServerIP());
  
  // Teste de conectividade básica
  Serial.println("Testando conectividade...");
  Serial.print("Conectando ao broker ");
  Serial.print(mqttServer);
  Serial.print(":");
  Serial.println(mqtt_port);
  
  // Configura o servidor MQTT
  client.setServer(mqttServer, mqtt_port);
  
  // Configuração adicional do cliente MQTT
  client.setKeepAlive(60);
  client.setSocketTimeout(30);
  
  Serial.println("Sistema iniciado. Pronto para ler encoder e enviar via MQTT.");
  Serial.println("Envie 'Z' via serial para zerar o encoder.");
  Serial.println("=====================================");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (Serial.available()) {
    char comando = Serial.read();
    if (comando == 'Z' || comando == 'z') {
      myEnc.write(0);
      Serial.println("Encoder zerado!");
    }
  }

  unsigned long tempoAtual = millis();

  if (tempoAtual - tempoAnterior >= intervalo) {
    tempoAnterior = tempoAtual;

    // Lê a posição atual do encoder
    long newPosition = myEnc.read();
    
    // Calibração: Converte pulsos para centímetros
    float medida = newPosition / 1956.0;

    Serial.print("Posição: ");
    Serial.print(newPosition);
    Serial.print(" -> Medida (cm): ");
    Serial.println(medida);
    
    char mensagem[10];
    dtostrf(medida, 1, 2, mensagem);
    
    // Publica a mensagem no broker MQTT se estiver conectado
    if (client.connected()) {
      client.publish("regua/medida", mensagem);
    }
  }
}