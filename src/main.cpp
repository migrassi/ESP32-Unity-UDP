/*
  UDP_Servidor_Multicast
  Created:  2022/09/01
  Last modified: 2022/09/03 23:34:53
  Version: 1.0

  Author: Miguel Grassi
  Contact: migrassi@gmail.com

  Description:
  Requiere un cable con extremo desnudo o soldado a una plaquita PCB conectada al pin
  del Touch0 (GPIO 4 del ESP32 - 7º pin de la derecha, contando desde abajo en NodeMCU)

  Se suscribe a un grupo UDP Multicast (239.1.2.3)
  Ver: https://www.iana.org/assignments/multicast-addresses/multicast-addresses.xhtml

    * Al iniciar imprime por puerto serie el IP y puerto UDP que está escuchando
   
    * Cada vez que recibe un mensaje por UDP imprime en el puerto serie:
      - El tipo de mensaje (Unicast, Multicast o Broadcast)
      - El IP y puerto UDP del Sender
      - El IP y puerto UDP propio
      - El Payload (contenido) del mensaje

    * Retransmite al grupo UDP lo que recibe en el puerto serie
   
    * Si recibe un número entero o float lo guarda en la variable salto
   
    * Cuando recibe contacto en el Touch 0 manda un al valor guardado en la var salto por el UDP.
      
  Ver proyecto completo en https://github.com/migrassi/ESP32-Unity-UDP
  
  (c) 2022 - https://MiguelGrassi.com.ar

*/

#include <Arduino.h> // Esta linea no hace falta si se usa el entorno Arduino
#include "WiFi.h"
#include "AsyncUDP.h"

//const char * ssid = "mitodama";
//const char * password = "ALaFubaMabaraCalacaTacuba";
const char * ssid = "MiguelAP";
const char * password = "ALaFubaM";

const int puerto = 1234;

AsyncUDP udp;
String miBuffer;
float auxi;
String salto="1";

int threshold = 60;// Original 40
bool touch0detected = false;


void gotTouch0(){
 touch0detected = true;
}

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed");
    while (1) {
      delay(1000);
    }
  }
  touchAttachInterrupt(T0, gotTouch0, threshold);
  if (udp.listenMulticast(IPAddress(239, 1, 2, 3), puerto)) {
    Serial.print("Escuchando UDP en IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("  Puerto:  ");
    Serial.println(puerto);
    udp.onPacket([](AsyncUDPPacket packet) {
      Serial.print("Tipo de paquete UDP: ");
      Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast" : "Unicast");
      Serial.print(", De: ");
      Serial.print(packet.remoteIP());
      Serial.print(":");
      Serial.print(packet.remotePort());
      Serial.print(", To: ");
      Serial.print(packet.localIP());
      Serial.print(":");
      Serial.print(packet.localPort());
      Serial.print(", Longitud: ");
      Serial.print(packet.length());
      Serial.print(", Data: ");
      Serial.write(packet.data(), packet.length());
      Serial.println();
      //reply to the client
      packet.printf("Recibí %u bytes de datos", packet.length());
    });
    //Send multicast
    udp.print("Hola Grupo!");
  }
}

void loop()
{
  if (Serial.available()) {      // Si viene algo en el puerto Serial (USB),
    // udp.print(Serial.read());   // Manda por udp al grupo Multicast cada caracter que le llega
    //miBuffer = Serial.readString(); // Espera hasta el TimeOut. Es más lenta pero lee la cadena entera
    miBuffer = Serial.readStringUntil('\n');//Lee la cadena hasta el caracter indicado o toda por timeout si el caracter no está
  }
  if (miBuffer != "") { //Si hay un string en miBuffer
    udp.print(miBuffer);   // Lo manda por udp al grupo Multicast. Unity no recibe esto porque no se suscribe aparentemente
    udp.broadcastTo(miBuffer.c_str(),1234);// Pero si recibe el broadcast
    auxi=miBuffer.toFloat(); // Además, si es convertible a un número, lo carga en una variable auxiliar
    if (auxi==0){// Aunque evito que quede en cero
        salto="1";
      }  
      else{
        salto=String(auxi);
      }
     
    miBuffer = "";
  }

  /*
    delay(1000);
    //Manda un mensaje al grupo multicast
    udp.print("Mensaje Multicast-> Hola Grupo");
    //Manda un mensaje Broadcast
    udp.broadcastTo("Mensaje Broadcast-> Hola a todos", 1234);
  */

    if(touch0detected){
    touch0detected = false;
    Serial.println("Touch 0 detectado");
    udp.broadcastTo(salto.c_str(),1234);
  }
}