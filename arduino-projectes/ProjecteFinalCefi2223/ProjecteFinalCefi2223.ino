#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>                
#include <DallasTemperature.h>

#define DISPOSITIVO "fab1" //Dispositivo que identifica al publicar en MQTT
#define RAIZ "cefi/ejercicio1"  //raiz de la ruta donde va a publicar
 
OneWire ourWire3(9);                //Se establece el pin 2  como bus OneWire
DallasTemperature sensors3(&ourWire3); //Se declara una variable u objeto para nuestro sensor

//Crear el objeto lcd  dirección  0x3F y 16 columnas x 2 filas
LiquidCrystal_I2C lcd(0x3F,16,2);  //
// Update these with values suitable for your network.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,8,30);
IPAddress server(192,168,8,5);

//Sensor
long now;
long lastMsg =0;

float temp3;
char charTemperature3[] = "00.0";

//Topics
String topic_root =  String(RAIZ) + "/" + String(DISPOSITIVO);
String publish_temperatura3_string = topic_root + "/temperatura3";
const char* publish_temperatura3 = publish_temperatura3_string.c_str();
  
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[0]);
  }
}

EthernetClient ethClient;
PubSubClient clientmqtt(ethClient);

void reconnect() {
  // Loop until we're reconnected
  while (!clientmqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (clientmqtt.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      clientmqtt.publish(publish_temperatura3,"Proba arduino esta publicant");
    } else {
      Serial.print("failed, rc=");
      Serial.print(clientmqtt.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(57600);

  // Inicializar el LCD
  lcd.init();
  
  //Encender la luz de fondo.
  lcd.backlight();
  
  // Escribimos el Mensaje en el LCD.
  lcd.print("HOLA MUNDO");

  sensors3.begin();   //Se inicia el sensor 2

  clientmqtt.setServer(server, 1883);
  clientmqtt.setCallback(callback);

  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);
}

void loop()
{
  if (!clientmqtt.connected()) {
    reconnect();
  }
  clientmqtt.loop();

  now=millis();
  if ((now-lastMsg)>3000) {
    lastMsg = now;
    sensors3.requestTemperatures();   //Se envía el comando para leer la temperatura
    temp3= sensors3.getTempCByIndex(0); //Se obtiene la temperatura en ºC

    if (clientmqtt.connect("arduinoClient")) {
      Serial.println("connected");
      dtostrf(temp3, 4, 1, charTemperature3);

      clientmqtt.publish(publish_temperatura3, charTemperature3);   
    } 
    // Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1)
    lcd.setCursor(0, 1);
    lcd.print(temp3);
    lcd.print(" ");
  }

  lcd.setCursor(0, 0);
  lcd.print("CEFI:");
  lcd.print(" t:");
  // Escribimos el número de segundos trascurridos
  lcd.print(millis()/1000);
  lcd.print(" s");  
}
