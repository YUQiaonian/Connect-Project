/*
 *Project name:
 *File name:
 *Description:
 *Author: LU & JIANG
 *Date:
*/

#ifndef __MYMQTT_H__
#define __MYMQTT_H__

#ifdef __cplusplus

#pragma message ("C++")
extern "C" {
#endif

#define ADDRESS   "192.168.43.149:1883"
//#define ADDRESS   "192.168.43.221:1883"
#define CLIENTID  "PCCONTROL"
#define TOPICPC   "PC"
#define TOPICROB  "robot"
#define QOS        0
#define TIMEOUT    10000L

int mqtt_client_publish(char *top,char *msg);
int mqtt_client_subscribe(char *top);
void delivered(void *context, MQTTClient_deliveryToken dt);
void connlost(void *context, char *cause);

#ifdef __cplusplus
}
#endif

#endif
