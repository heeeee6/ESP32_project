#ifndef DEMO1_MQTT_CONFIG_H
#define DEMO1_MQTT_CONFIG_H

typedef enum {
    OFFLINE=0,
    ONLINE,
//    MQTT_CODE_RGB,
    MQTT_CODE_MOTOR,
    MQTT_SUBMIT=105,
}MQTT_Code;

typedef enum {
    MOTOR=0,//马达
    RGB,
}Device_Node;
#endif //DEMO1_MQTT_CONFIG_H
