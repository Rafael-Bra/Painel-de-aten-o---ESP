#include <WiFi.h>
#include <PubSubClient.h>

const char* SALA = "A";

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";

const char* MQTT_HOST = "escola-mesquita.cloud.shiftr.io";
const int MQTT_PORT = 1883;
const char* MQTT_USER = "escola-mesquita";
const char* MQTT_PASS = "blcAkgBumM26K61q";

int leds[5] = {2, 4, 5, 18, 19};
const int TOTAL_ALUNOS = 10;
const int SALAS = 5;
const float LIMIAR = 0.5f;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

float atencao [SALAS] [TOTAL_ALUNOS];
bool chegou [SALAS] [TOTAL_ALUNOS];

void limparDados() {

  for (int i = 0; i < SALAS; i++) {
    for (int j = 0; j < TOTAL_ALUNOS; j++) {
      atencao[i][j] = 0.0f;
      chegou[i][j] = false;
    }
  }
}

void conectarWiFi() {
  Serial.print("Conectando WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" OK");
}

void atualizarLed(int lab) {
  float soma = 0.0f;
  int qtd = 0;

  //Passa por todos os alunos e realiza cálculo apenas dos que receberam dados
  for (int i = 0; i < TOTAL_ALUNOS; i++) {
    if (chegou[lab][i]) {
      soma += atencao[lab][i];
      qtd++;
    }
  }

  //Evita divisão por 0
  if (qtd == 0) {
    return;
  }

  //Média da turma
  float media = soma / qtd;

  //Altera estado do Led conforme valor da "media" em comparação com "LIMIAR"(0,5)
  digitalWrite(leds[lab], media < LIMIAR ? HIGH : LOW);

  //Imprime valores e os identifica pelo laboratório
  Serial.print("Lab ");
  Serial.print(lab + 1);
  Serial.print(" | media=");
  Serial.print(media, 4);
  Serial.print(" | amostras=");
  Serial.println(qtd);
}

void onMessage(char* topic, byte* payload, unsigned int length) {
  String t = String(topic);
 //Extração do número do laboratório do tópico e correção do index para o array
  int lab = t.substring(3, 4).toInt();
  lab = lab - 1;
 //Localiza Strinfg "Aluno" no tópico 
  int p = t.indexOf("Aluno");
  if (p < 0) {
    return;
  }
  //Extrai número de identificação e corrige index para o array
  int aluno = t.substring(p + 5).toInt();
  aluno = aluno - 1;

  if (lab < 0 || lab >= SALAS || aluno < 0 || aluno >= TOTAL_ALUNOS){
        return;
  }

  //Recebe payload em Bytes e converte para string
  String msg = "";
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  //Converte valor da atenção de string para Float
  float valor = msg.toFloat();

  //Armazena último valor e valida recebimento de dados
  atencao[lab][aluno] = valor;
  chegou[lab][aluno] = true;

  //Recalcula a média do laboratório a cada dado de aluno individual
  atualizarLed(lab);
}

void conectarMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Conectando MQTT...");
    //Nome do cliente
    String clientId = "esp32-sala-" + String(SALA) + "-" + String(random(1000, 9999));
    if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
      Serial.println(" OK");

      //Montagem das Strings para inscrição nos tópicos
      for (int i = 1; i <= SALAS; i++) {
        String topico = "Lab" + String(i) + "/#";
        mqttClient.subscribe(topico.c_str());
        Serial.print("Inscrito em: ");
        Serial.println(topico);
      }
    } else {
      Serial.println(" falhou");
    }
  }
}

void setup() {
  Serial.begin(115200);
  //Definição dos pinos 
  for (int i = 0; i < SALAS; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }

  //Elimina lixo em memória
  limparDados();
  randomSeed(micros());
  conectarWiFi();

  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCallback(onMessage);
}

void loop() {
  if (!mqttClient.connected()) {
    conectarMQTT();
  }
  mqttClient.loop();
}
