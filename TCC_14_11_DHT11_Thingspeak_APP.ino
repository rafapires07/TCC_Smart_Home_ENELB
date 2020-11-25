#include <DHT.h>
#include <WiFi.h>
 
#define DHTPIN 23
#define SPOINT 30
int CMD = 4;
 
#define SSID_REDE "2,4g_BRANCO" 
#define SENHA_REDE "ra692515" 
#define INTERVALO_ENVIO_THINGSPEAK 20000
#define DHTTYPE DHT11

char endereco_api_thingspeak[] = "api.thingspeak.com";
String chave_escrita_thingspeak = "FHBC36K0ZC932ECE";  
unsigned long last_connection_time;
WiFiClient  client;
DHT dht(DHTPIN, DHTTYPE);
 
void envia_informacoes_thingspeak(String string_dados);
void init_wifi(void);
void conecta_wifi(void);
void verifica_conexao_wifi(void);

void envia_informacoes_thingspeak(String string_dados)
{
    if (client.connect(endereco_api_thingspeak, 80))
    {

        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+chave_escrita_thingspeak+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(string_dados.length());
        client.print("\n\n");
        client.print(string_dados);
         
        last_connection_time = millis();
        Serial.println("- Informações enviadas ao ThingSpeak!");
    }
}
 

void init_wifi(void)
{
    Serial.println("------WI-FI -----");
    Serial.println("Conectando-se a rede: ");
    Serial.println(SSID_REDE);
    Serial.println("\nAguarde...");
 
    conecta_wifi();
}
 

void conecta_wifi(void)
{

    if (WiFi.status() == WL_CONNECTED)
    {
        return;
    }
     

    WiFi.begin(SSID_REDE, SENHA_REDE);
     
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(100);
    }
 
    Serial.println("Conectado com sucesso a rede wi-fi \n");
    Serial.println(SSID_REDE);
}
 

void verifica_conexao_wifi(void)
{
    conecta_wifi();
}
 
void setup()
{
    Serial.begin(115200);
    pinMode(CMD, OUTPUT);
    last_connection_time = 0;
    dht.begin();
    init_wifi();
}
 
void loop()
{
    char fields_a_serem_enviados[100] = {0};
      float umidade_lida = dht.readHumidity();
      float temperatura_lida = dht.readTemperature();
      float Status_CMD= digitalRead (CMD);

    if (temperatura_lida > SPOINT) 
{
  digitalWrite (CMD, HIGH);
}
else if (temperatura_lida < SPOINT)
{
  digitalWrite (CMD, LOW);
}  
    if (client.connected())
    {
        client.stop();
        Serial.println("- Desconectado do ThingSpeak");
        Serial.println();
        Serial.print(F("Humidade: "));
        Serial.print(umidade_lida);
        Serial.print(F("%  Temperatura: "));
        Serial.print(temperatura_lida);
        Serial.print(F("°C "));
        Serial.print("SetPoint:  ");
        Serial.print(SPOINT);
        Serial.println(F("°C "));
        Serial.print(Status_CMD);
    }
 

    verifica_conexao_wifi();

    if( millis() - last_connection_time > INTERVALO_ENVIO_THINGSPEAK )
    {
        temperatura_lida = dht.readTemperature();
        umidade_lida = dht.readHumidity();
        sprintf(fields_a_serem_enviados,"field1=%.2f&field2=%.2f&field3=%.2f", temperatura_lida, umidade_lida, Status_CMD);
        envia_informacoes_thingspeak(fields_a_serem_enviados);
    }
 
    delay(1000);
}
