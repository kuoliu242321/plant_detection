#include <ArduinoMqttClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ESP8266WiFiMulti.h>   // 本程序使用ESP8266WiFiMulti库

ESP8266WiFiMulti wifiMulti;     // 建立ESP8266WiFiMulti对象,对象名称是'wifiMulti'


WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[]    = "iot-xxxxxxxx.mqtt.iothub.aliyuncs.com";
int        port        = 1883;

const char inTopic[]   = "/sys/xxxxxxx/esp8266/thing/event/property/post_reply";
const char outTopic[]  = "/sys/xxxxxxx/esp8266/thing/event/property/post";

const long interval = 2000;
unsigned long previousMillis = 0;

int count = 0;

String inputString ="";

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // attempt to connect to WiFi network:
  wifiMulti.addAP("12345678", "22222222");  // 这三条语句通过调用函数addAP来记录3个不同的WiFi网络信息。
  wifiMulti.addAP("ZhouQ3", "87654321"); // 这3个WiFi网络名称分别是taichi-maker, taichi-maker2, taichi-maker3。
  wifiMulti.addAP("666", "12345678"); // 这3个网络的密码分别是123456789，87654321，13572468。
                                                // 此处WiFi信息只是示例，请在使用时将需要连接的WiFi信息填入相应位置。
                                                
  //Serial.println("Connecting ...");         // 通过串口监视器输出信息告知用户NodeMCU正在尝试连接WiFi
  int i = 0;                                 
  while (wifiMulti.run() != WL_CONNECTED) {  // 此处的wifiMulti.run()是重点。通过wifiMulti.run()，NodeMCU将会在当前
    delay(1000);                             // 环境中搜索addAP函数所存储的WiFi。如果搜到多个存储的WiFi那么NodeMCU
    Serial.print('.');                       // 将会连接信号最强的那一个WiFi信号。
  }  

  //Serial.println("You're connected to the network");
  //Serial.println();

  mqttClient.setId("xxxxxxx.esp8266|securemode=2,signmethod=hmacsha256,timestamp=1706962307270|");                    //mqtt 连接客户端id
  mqttClient.setUsernamePassword("esp8266&xxxxxxx", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");        //mqtt 连接用户名、密码

  //Serial.print("Attempting to connect to the MQTT broker: ");
  //Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  //Serial.println("You're connected to the MQTT broker!");
  //Serial.println();

  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);

  //Serial.print("Subscribing to topic: ");
  //Serial.println(inTopic);
  //Serial.println();

  int subscribeQos = 1;

  mqttClient.subscribe(inTopic, subscribeQos);

  // topics can be unsubscribed using:
  // mqttClient.unsubscribe(inTopic);

  //Serial.print("Waiting for messages on topic: ");
  //Serial.println(inTopic);
  //Serial.println();
}

void loop() {

  mqttClient.poll();


  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    String payload;
    //25.2 12345 52.8 12.5  串口接收的数据消息
    if (Serial.available() > 0) 
    {
      // 读取串口接收到的数据
      String receivedMessage = Serial.readStringUntil('\n');
     // 将 String 对象转换为 C 风格字符串
      char receivedMessageBuffer[receivedMessage.length() + 1];
      // 加上 null 结尾字符所需的空间
      receivedMessage.toCharArray(receivedMessageBuffer, sizeof(receivedMessageBuffer));
      // 使用 strtok() 函数解析 C 风格字符串
      char *token = strtok(receivedMessageBuffer, " ");
      // 如果 token 不为空，则说明成功分割到了字符串
      if (token != NULL) {
      // 第一个值为土壤湿度值
      float soilHumidity = atof(token);
      // 继续获取下一个 token
      token = strtok(NULL, " ");
      // 第二个值为气压
      float airPressure = atof(token);
      // 继续获取下一个 token
      token = strtok(NULL, " ");
      // 第三个值为湿度
      float humidity = atof(token);
      // 继续获取下一个 token
      token = strtok(NULL, " ");
      // 最后一个值为温度值
      float temp = atof(token);

      DynamicJsonDocument json_msg(512);
      DynamicJsonDocument json_data(512);

      json_data["temp"] = temp;
      json_data["Airpressure"] = airPressure;
      json_data["humity"] = humidity;
      json_data["soil_humity"] = soilHumidity;

    
      json_msg["params"] = json_data;
      json_msg["version"] = "1.0.0";


      serializeJson(json_msg,payload);
    }
  }
    //Serial.print("Sending message to topic: ");
    //Serial.println(outTopic);
    //Serial.println(payload);

    bool retained = false;
    int qos = 1;
    bool dup = false;

    mqttClient.beginMessage(outTopic, payload.length(), retained, qos, dup);
    mqttClient.print(payload);
    mqttClient.endMessage();
    //Serial.println();
    count++;
  }
}
void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  //Serial.print("Received a message with topic '");
  //Serial.print(mqttClient.messageTopic());
  //Serial.print("', duplicate = ");
  //Serial.print(mqttClient.messageDup() ? "true" : "false");
  //Serial.print(", QoS = ");
  //Serial.print(mqttClient.messageQoS());
  //Serial.print(", retained = ");
  //Serial.print(mqttClient.messageRetain() ? "true" : "false");
  //Serial.print("', length ");
  //Serial.print(messageSize);
  //Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    char inChar =(char)mqttClient.read();
    inputString +=inChar;
    if(inputString.length()==messageSize) 
    {
      //{"deviceType":"CustomCategory","iotId":"ugQfUxxxxxxvKC8Ek0sz10","requestId":"null","checkFailedData":{"code":{"code":6306,"message":"tsl parse: data type is not int -> code","time":1707149551046,"value":"15"}},"productKey":"k0sxxxxx4CN","gmtCreate":1707149551050,"deviceName":"mobile_app","items":{}}    
      if(inputString.length()>150){
      DynamicJsonDocument json_msg(1024);
      DynamicJsonDocument json_item(1024);
      DynamicJsonDocument json_value(1024);

      deserializeJson(json_msg,inputString);

      String checkFailedData = json_msg["checkFailedData"];

      deserializeJson(json_item,checkFailedData);
      String code = json_item["code"];

      deserializeJson(json_value,code);
      int value = json_value["value"].as<int>();
      Serial.print(value);//print输出不带有回车及换行
      //Serial.println(inputString);
    }
    else{
            StaticJsonDocument<200> doc;
            // 解析 JSON 数据
            DeserializationError error = deserializeJson(doc, inputString);
            // 提取 "message" 值并检查是否为 "success"
            const char* message = doc["message"];
            if (strcmp(message, "success") == 0) {
            //Serial.println("Received success message!");
            } else {
            //Serial.println("Received message, but not success!");
            }
       }
      inputString="";//数组清零
    }
  }
  //Serial.println();
}
