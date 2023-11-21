/*
  Basic MQTT example

  This sketch demonstrates the basic capabilities of the library.
  It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary

  It will reconnect to the server if the connection is lost using a blocking
  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
  achieve the same result without blocking the main loop.

*/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>                
#include <DallasTemperature.h>

#define DISPOSITIVO "fab1" //Dispositivo que identifica al publicar en MQTT
#define RAIZ "cefi/ejercicio1"  //raiz de la ruta donde va a publicar

OneWire puertosensor1(9);                //Se establece el pin 2  como bus OneWire
DallasTemperature sensor1(&puertosensor1); //Se declara una variable u objeto para nuestro sensor

//Crear el objeto lcd  dirección  0x3F y 16 columnas x 2 filas
LiquidCrystal_I2C lcd(0x3F, 16, 2);
// Update these with values suitable for your network.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//byte mac[]    = {  0xD8, 0xA7, 0x56, 0x31, 0x06, 0xD0 };
IPAddress ip(192, 168, 8, 40);
IPAddress server(192, 168, 8, 13); //IP RaspBerry

long ahora, ultimamesura, segundero; //variables de tiempo

char charTemperature3[]= "00.00";
char charSecons[]= "00.0";
float temp1; //variables temperatura

//Topics
String topic_root =  String(RAIZ) + "/" + String(DISPOSITIVO);
String publish_temperatura1_string = topic_root + "/temperaturaEnric";
const char* publish_temperatura1 = publish_temperatura1_string.c_str();


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

EthernetClient ethClient;
PubSubClient client(ethClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("TopicEnric", "hello Enric");
      // ... and resubscribe
      client.subscribe("TopicEnric");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
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
  lcd.print("HOLA Enric");



  client.setServer(server, 1883);
  client.setCallback(callback);

  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);

}

void loop()
{
 ahora = millis();
  if ((ahora - ultimamesura) > 3000) {
    ultimamesura = ahora;
    segundero = ultimamesura / 1000;
    sensor1.requestTemperatures();   //Se envía el comando para leer la temperatura
    temp1= sensor1.getTempCByIndex(0); //Se obtiene la temperatura en ºC  
    // Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1)
    lcd.setCursor(0, 1);
    lcd.print(segundero);
    lcd.print(" ");
    lcd.print("s");
    Serial.println(segundero);

    
    lcd.print(" ");
    lcd.print(temp1);
    lcd.print(" ");
    lcd.print("Cels");
    Serial.println(temp1);    

    dtostrf(temp1,4,2, charTemperature3);
    client.publish("EnricTemperatura", charTemperature3);
    
    client.publish(publish_temperatura1, charTemperature3);
    
    /*dtostrf(segundero,4,1, charSecons);
   client.publish("EnricSegundero", charSecons);*/
  }

   if (!client.connected()) {
     reconnect();
    }
    client.loop();
}
