
/*
 *  More info      : http://pdacontrolen.com
 *  Mas informacion: http://pdacontroles.com
 *  Channel  Youtube https://www.youtube.com/c/JhonValenciaPDAcontrol/videos   
 *
 *
 *  Modified by Trialcommand
 *   More Tutorials: 
 *   Website http://trialcommand.com
 *	 In English: http://en.trialcommand.com
 *	 En Español: http://en.trialcommand.com  
 *   
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ModbusMaster232.h>
#include <SoftwareSerial.h>   // Modbus RTU pins   D7(13),D8(15)   RX,TX

WiFiClient espClient;
PubSubClient client_MQTT (espClient);

const char* ssid =        "************";
const char* password =    "****************";
const char* mqtt_server = "******************";   /// example 192.168.0.19
int mqtt_port            = 1883;

String MQTT_to_Holding[20];  // Array recepcion valores MQTT // Array reception MQTT values

// Instantiate ModbusMaster object as slave ID 1
ModbusMaster232 node(1);

void setup()
{
  Serial.begin(9600);
  delay(100); 
    
   node.begin(9600);
   delay(100); 
     
   WiFi.begin(ssid, password);
  client_MQTT.setServer(mqtt_server, mqtt_port);
  client_MQTT.setCallback(callback);
  delay(100);  
  Serial.println(".");


 while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("Connected ");
  Serial.print("MQTT Server ");
  Serial.print(mqtt_server);
  Serial.print(":");
  Serial.println(String(mqtt_port));  
  Serial.print("ESP8266 IP ");
  Serial.println(WiFi.localIP());  
  Serial.println("Modbus RTU Master Online");
  

  
}

/*  funtion  callback
 * 
 * Esta funcion realiza la recepcion de los topic suscritos
 * This function performs the reception of subscribed topics
 * 
 * aqui se realiza en almacenamiento de datos en Array       String MQTT_to_Holding
 * Here is done in data storage in Array                     String MQTT_to_Holding
 */

void callback(char* topic, byte* payload, unsigned int length) {
  
for (int i = 0; i < 20; i++) { MQTT_to_Holding[i]="";  }  //limpia cadenas recibidas // Cleans received strings

String string;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  int cantidad=0 ;   // cantidad de registros  //  number of records
  for (int i = 0; i < length; i++) {
  string+=((char)payload[i]);   


      if (topic ="MQTT_Holding_Array")   {      //  Check the topic  ///  Verifica el topico

                 if ( payload[i] == 44 )  // ","= 44 ascii // detecta separador "," //  Detect tab ","
                 {                    
                       cantidad++;        
                 }
                 else
                 {
                   //Serial.println(cantidad);
                    MQTT_to_Holding[cantidad]+=((char)payload[i]);
                   /// Serial.println((char)payload[i]);
                 }         
      }           
    
  }

  Serial.println(string);  // Imprime mensajes MQTT Recibidos // Prints MQTT messages received
}

/*
 * 
 * Funcion que realiza la reconexion de cliente MQTT
 * Function that performs MQTT client reconnection
 * 
 * enable/habilita  client_MQTT.subscribe("MQTT_Holding_Array");
 */
void reconnect() {  
  // Loop until we're reconnected  // Bucle hasta que se vuelva a conectar
  while (!client_MQTT.connected()) {    
  
    if (client_MQTT.connect("ESP8266Client")) {
      // Serial.println("MQTT Online");     
       
      //// Topico para recibir Holding Registers desde MQTT // Topic to receive Holding Registers from MQTT --- return data in callback funtion
      client_MQTT.subscribe("MQTT_Holding_Array");          
    } else {
      Serial.print("failed, rc=");
      Serial.print(client_MQTT.state());
    
    }
  }
}


void loop()
{
 
 /* 
          Direccionamiento MQTT -> Holding Registers [0][9] = 10 registros  
          MQTT Addressing -> Holding Registers [0] [9] = 10 Registers
          Topic = "MQTT_Holding_Array"  1 topico MQTT                                      
          client_MQTT.subscribe("MQTT_Holding_Array");  
          subscribe topic MQTT_Holding_Array

          previamente en la funcion callback se toma la cadena mqtt y se separan los registros 
          se almacenan en array string a int y se envian en los primeros 10 registros modbus

          Previously in the function callback takes the string mqtt and separates the records are
          stored in array string to int and are sent in the first 10 registers modbus

 */     
  
       node.writeSingleRegister(0, MQTT_to_Holding[0].toInt() );     /// Send MQTT Array String to int to Modbus RTU Master
       node.writeSingleRegister(1, MQTT_to_Holding[1].toInt() ); 
       node.writeSingleRegister(2, MQTT_to_Holding[2].toInt() ); 
       node.writeSingleRegister(3, MQTT_to_Holding[3].toInt() ); 
       node.writeSingleRegister(4, MQTT_to_Holding[4].toInt() ); 
       node.writeSingleRegister(5, MQTT_to_Holding[5].toInt() ); 
       node.writeSingleRegister(6, MQTT_to_Holding[6].toInt() ); 
       node.writeSingleRegister(7, MQTT_to_Holding[7].toInt() ); 
       node.writeSingleRegister(8, MQTT_to_Holding[8].toInt() ); 
       node.writeSingleRegister(9, MQTT_to_Holding[9].toInt() ); 





 /* 
          Direccionamiento Holding Registers -> MQTT = 10 registros  
          Addressing Holding Registers  [10] [19] -> MQTT = 10 Registers - 10 topics MQTT 
          Topic INDIVIDUAL = "Holding_to_MQTT_1"....... "Holding_to_MQTT_10"  10 topics MQTT                                      
          publish message  10


          cada uno de los 10 registro modbus se envia en un topico MQTT diferente de manera individual
          Each of the 10 modbus registers is sent in a different MQTT topic individually
          
 */   

int tdelay = 5 ; /// delay
char  buf[10];   
String Holding_to_MQTT[10];

        node.readHoldingRegisters(10, 1);                          // Holding Registers [10]
        Holding_to_MQTT[0]= String(node.getResponseBuffer(0));
        String (node.getResponseBuffer(0)).toCharArray(buf, 10);   /// String a char para mensaje MQTT  /// String to char for MQTT message
        client_MQTT.publish("Holding_to_MQTT_1",buf);
        node.clearResponseBuffer();                               /// Limpiar buffer modbus RTU     /// Clean modbus RTU buffer       
        delay(tdelay);
       
        node.readHoldingRegisters(11, 1);                         // Holding Registers [11]
        Holding_to_MQTT[1]= String(node.getResponseBuffer(0));   
        String (node.getResponseBuffer(0)).toCharArray(buf, 10);
        client_MQTT.publish("Holding_to_MQTT_2",buf);
        node.clearResponseBuffer();
        delay(tdelay);

        node.readHoldingRegisters(12, 1);                         // Holding Registers [12]
        Holding_to_MQTT[2]= String(node.getResponseBuffer(0));
        String (node.getResponseBuffer(0)).toCharArray(buf, 10);
        client_MQTT.publish("Holding_to_MQTT_3",buf);
        node.clearResponseBuffer();
        delay(tdelay);

        node.readHoldingRegisters(13, 1);                         // Holding Registers [13]
        Holding_to_MQTT[3]= String(node.getResponseBuffer(0));
        String (node.getResponseBuffer(0)).toCharArray(buf, 10);
        client_MQTT.publish("Holding_to_MQTT_4",buf);
        node.clearResponseBuffer();
        delay(tdelay);

        node.readHoldingRegisters(14, 1);                         // Holding Registers [14]
        Holding_to_MQTT[4]= String(node.getResponseBuffer(0));
        String (node.getResponseBuffer(0)).toCharArray(buf, 10);
        client_MQTT.publish("Holding_to_MQTT_5",buf);
        node.clearResponseBuffer();
        delay(tdelay);

        node.readHoldingRegisters(15, 1);                         // Holding Registers [15]
        Holding_to_MQTT[5]= String(node.getResponseBuffer(0));
        String (node.getResponseBuffer(0)).toCharArray(buf, 10);
        client_MQTT.publish("Holding_to_MQTT_6",buf);
        node.clearResponseBuffer();
        delay(tdelay);

        node.readHoldingRegisters(16, 1);                          // Holding Registers [16]
        Holding_to_MQTT[6]= String(node.getResponseBuffer(0));
        String (node.getResponseBuffer(0)).toCharArray(buf, 10);
        client_MQTT.publish("Holding_to_MQTT_7",buf);
        node.clearResponseBuffer();
        delay(tdelay);

        node.readHoldingRegisters(17, 1);                           // Holding Registers [17]
        Holding_to_MQTT[7]= String(node.getResponseBuffer(0)); 
        String (node.getResponseBuffer(0)).toCharArray(buf, 10);
        client_MQTT.publish("Holding_to_MQTT_8",buf);
        node.clearResponseBuffer();
        delay(tdelay);

        node.readHoldingRegisters(18, 1);                           // Holding Registers [18]
        Holding_to_MQTT[8]= String(node.getResponseBuffer(0));
        String (node.getResponseBuffer(0)).toCharArray(buf, 10);
        client_MQTT.publish("Holding_to_MQTT_9",buf);
        node.clearResponseBuffer();
        delay(tdelay);

        node.readHoldingRegisters(19, 1);                           // Holding Registers [19]
        Holding_to_MQTT[9]= String(node.getResponseBuffer(0));
        String (node.getResponseBuffer(0)).toCharArray(buf, 10);
        client_MQTT.publish("Holding_to_MQTT_10",buf);
        node.clearResponseBuffer();
        delay(tdelay);
        
 /* 
          Direccionamiento Holding Registers -> MQTT = 10 registros  
          Addressing Holding Registers  [10] [19] -> MQTT = 10 Registers - 1 topic MQTT 
          single 
          Topic  = "Holding_to_MQTT_Array"  1 topics MQTT                                      
          publish message 1

          Se concatena el valor de todos los 10 Registros Modbus y se envian en 1 solo topico 
          The value of all 10 Modbus Registers is concatenated and sent in 1 single topical
 */  

         String Holding_to_MQTT_Array;
         Holding_to_MQTT_Array = Holding_to_MQTT[0];
         Holding_to_MQTT_Array +=",";
         Holding_to_MQTT_Array +=Holding_to_MQTT[1];
         Holding_to_MQTT_Array +=",";
         Holding_to_MQTT_Array +=Holding_to_MQTT[2];
         Holding_to_MQTT_Array +=",";
         Holding_to_MQTT_Array +=Holding_to_MQTT[3];
         Holding_to_MQTT_Array +=",";
         Holding_to_MQTT_Array +=Holding_to_MQTT[4];
         Holding_to_MQTT_Array +=",";
         Holding_to_MQTT_Array +=Holding_to_MQTT[5];
         Holding_to_MQTT_Array +=",";
         Holding_to_MQTT_Array +=Holding_to_MQTT[6];
         Holding_to_MQTT_Array +=",";
         Holding_to_MQTT_Array +=Holding_to_MQTT[7];
         Holding_to_MQTT_Array +=",";
         Holding_to_MQTT_Array +=Holding_to_MQTT[8];
         Holding_to_MQTT_Array +=",";
         Holding_to_MQTT_Array +=Holding_to_MQTT[9];
         
         // Serial.print("tamaño  ");
         // Serial.println( Holding_to_MQTT_Array.length());          
         // Serial.println(Holding_to_MQTT_Array);
         char  buf2[60];        
         Holding_to_MQTT_Array.toCharArray(buf2, 60);
         client_MQTT.publish("Holding_to_MQTT_Array",buf2);    


    if (!client_MQTT.connected()) {
            
         reconnect();         /// reconection MQTT
        }                        
       client_MQTT.loop();  

 
}






