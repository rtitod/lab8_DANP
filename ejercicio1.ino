#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const char* ssid = "TUSSID";
const char* password = "TUPW";
const int AnalogPin = 34;

const char* mqttServer = "a3brjmyw4c304l-ats.iot.us-east-2.amazonaws.com";
const int mqttPort = 8883;
int valor_humedad_raw;
float humedad_relativa;

WiFiUDP ntpUDP;
WiFiClientSecure espClient;
PubSubClient client(espClient);
NTPClient timeClient(ntpUDP);

const char* ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char* client_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUYRSGsxzw94ghaZj/ut+oJhF0jmkwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIzMDcyNDA2MDcw
OVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAPak1bB2aGfF4xvDOTOx
KlVOGobYC0Fm1jNmAIrPfCHL6Bpweekey4kls/Wf/RcjcAWUCtE4JRQCuR4yflpc
dX1E9PZy60KZRcMIJqC2UVODoLIP0zrmdTIjEey1cluwLQzRJ8/oa4wrBcTWawUk
XarI5dYt03nOBg9Prldw9SbIXRnyV/xTGyV6jveYu4YQDNe6wCHTNwehb0Taa5hy
BwBvNzSqCkNegZ5KwB86UYZvfiWy/tKOm5bEFSqfjSYIadSIUak5vCdIW2veuFsg
/JQ0cTET2sKIpwY+RmJDa0C6DP6QMMautr+cKKbvsP0qDiHBaIn+7jOW/YCC+ywh
Ak8CAwEAAaNgMF4wHwYDVR0jBBgwFoAU/uyluUbq+pWD1qEPzJCjyxRnQC4wHQYD
VR0OBBYEFG4GDEwYcDbofPMV/QXE/jjFhi/0MAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAxRV+ssYSFWtcFzXTwrJV3K4QL
D4lJC2FqoeF/lCVwrADoZdmrDaVzR6bUCLgYA4tjS6iyuQKbbmqwGJe9yAaNABfP
EvB6qAPgcSMfuTLOav4CqSCqXig8MKQbY0YAt+U6qerimExNY2y/K0QnE2a6RRwQ
1T/u0AabPup/IzyyHW1ldiOEq9bXtllt/2Qgs0q39OWNo07sWyXmSJafJ8K37jbi
TxH9Vk/nimi8HukDdXrrBRSYI5ZNS+5R7Rso4SuwX77c0Hs0VZvZtASlD7lmnMDA
JiuTjmscLjogptNZIY8bRpmrJ36swxcLoGvzieWH1fOkFd5c6pFXBfQ6UEps
-----END CERTIFICATE-----
)EOF";

const char* client_key = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEA9qTVsHZoZ8XjG8M5M7EqVU4ahtgLQWbWM2YAis98IcvoGnB5
6R7LiSWz9Z/9FyNwBZQK0TglFAK5HjJ+Wlx1fUT09nLrQplFwwgmoLZRU4Ogsg/T
OuZ1MiMR7LVyW7AtDNEnz+hrjCsFxNZrBSRdqsjl1i3Tec4GD0+uV3D1JshdGfJX
/FMbJXqO95i7hhAM17rAIdM3B6FvRNprmHIHAG83NKoKQ16BnkrAHzpRhm9+JbL+
0o6blsQVKp+NJghp1IhRqTm8J0hba964WyD8lDRxMRPawoinBj5GYkNrQLoM/pAw
xq62v5wopu+w/SoOIcFoif7uM5b9gIL7LCECTwIDAQABAoIBABs7b+yf5j8u9jhm
gbZXZVkt2nfw+q0p8ZSTOpQY/Jeur93bdVpr8AUgiJIiLAuV+hRjmB4pmM5wzoer
A5BujPpb1sAvSBJ+Ydl62Oj9uFUEP4XpgWULdP0oKh58mjfNZ+v2W/L0soszZNuq
4P4cWcs1i0runF7ihIDER3eYHTk+xqsE9Ys1fr10Go7t04DXD7FMoElApWq5s4Sx
u53OpR5r2flOO6+oeHv793CTyRabLBKasGXyHLG8i4G9PWDBBSKMs2uISzS5AUtD
dSt2AOwP4ynhL/UIrzZgTwRbcX5gfVsErGY+5Lrex1/KLjlJUtiSvBVUpflBnxmA
CahPGEECgYEA/cSlBvEkWpCzPh5r2xeYLEmfxzLbqHcb0JudFytZ0pE5azhcL112
9y8+v47Q+E+Rl1CTiEGpD8DkYlxMWhdJKtIa1CectP/3VApGT3GbkYKAPzHFckAs
NgZMnqeFbky4MglVaAw7GeMFk9QYT1EKzDZte0sUFzEWxqfKcYuZv+cCgYEA+NAm
YU/Z6oG3Coy+aMHfBcK8zx7EF7bnU7cQUTAFkBkiiW7IOutrALlTfMtzVkymnb+D
1RNIy84wLSKkLVOUcR7y3m+Cd8NmU4GxTRJyuYllimGlhCrh4YmDXbGZIPmZAJ5E
kbeldk6qmNYuxC92tXg4cx3G63ZGLj7Ijv4K/VkCgYB0iyUHJSP++8l1097Qxes7
G6znfPHoUptRmMXyaBReYheuqDmd/xyttOLqcpqCD3jbt0OqJg2QkJPSoHToQx11
LvznRjHv5kYeMIHN5mz9zKM9Yow9Rj+HqCj0DBd5an6FbEfu+v8BioN7OsLXWm7N
DooFP5vAcuj03wZYUzaPLQKBgQDF58qfXIRYoxSUr55EdPCQBHVpyW7v7evSCqpP
6YKDO3rdFeDt/JL/4GknuivjWNSuZMbXlJ8yfK6VOBiedPx9fcBwbQG5gRLDGUWN
qD7YlRDPIRwmJHXGjzqOK9VBBUdN1pVz0j8p+yh2qOZJ5fbmINnFtsmiQFFSN14P
XsIfAQKBgDRTc2UF9ViiMqi6hgqIBsjpcyD9lRVhsNTdqDl6PPOO/rbN7RgN/VjJ
H+XJSZfhIyJmYvcuKv5JW8KnUw9jyVruxZXtMCEEEEocssDsIq8g7vMhh+53TUZ5
eeACYQjbBjLuF2QX2de/mtf5MVe8C87kx3lD/TmKTLozTSgfITFI
-----END RSA PRIVATE KEY-----
)EOF";

void setup() {
  Serial.begin(9600);

  // Configurar conexión Wi-Fi
  WiFi.hostname("SensorLab");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectandose al Access Point WiFi...");
  }
  Serial.println("Conectado al Acces Point WiFi");

  // Cargar certificados
  espClient.setCACert(ca_cert);
  espClient.setCertificate(client_cert);
  espClient.setPrivateKey(client_key);

  // Configurar cliente MQTT
  client.setServer(mqttServer, mqttPort);

  //Configurar NTP
  timeClient.begin();
  timeClient.setTimeOffset(-18000);
}

void loop() {
  // Obtener el timestamp actualizado
  timeClient.update();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  valor_humedad_raw = analogRead (AnalogPin);
  humedad_relativa =  ((4095.0 - valor_humedad_raw) / 4095.0 ) * 100.0;

  // Crear y enviar el JSON
  String jsonString = "{\"Timestamp\":\"" + timeClient.getFormattedTime() + "\",\"Value\":" + String(humedad_relativa) + ",\"Unit\":\"Humedad Relativa\",\"Notes\":\"TEST\"}";

  if (client.publish("mobile/mensajes", jsonString.c_str())) {
    Serial.println("JSON enviado correctamente.");
  } else {
    Serial.println("Error al enviar el JSON.");
  }

  delay(30000);  // Esperar 30 segundos antes de enviar el siguiente JSON
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado al servidor MQTT");
    } else {
      Serial.println("Error de conexión al servidor MQTT, reintentando...");
      delay(5000);
    }
  }
}
