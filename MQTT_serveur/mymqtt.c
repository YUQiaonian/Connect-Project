#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "MQTTClient.h"
#include "mymqtt.h"

extern MQTTClient client;
extern MQTTClient_message pubmsg;
extern MQTTClient_deliveryToken token;
extern volatile MQTTClient_deliveryToken deliveredtoken;

int mqtt_client_publish(char *top,char *msg)
{
  int rc;
  if (client == NULL)
    return -1;

  pubmsg.payload = msg;
  pubmsg.payloadlen = strlen(msg);
  pubmsg.qos = QOS;
  pubmsg.retained = 0;

  MQTTClient_publishMessage(client, top, &pubmsg, &token);
  printf("Waiting for up to %d seconds for publication of %s"
          "on topic %s for client.\n",
          (int)(TIMEOUT/1000), msg, top);

  rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
  printf("Message with delivery token %d delivered\n", rc);
  return 0;
}

int mqtt_client_subscribe(char *top)
{
  if (client == NULL)
    return -1;

  MQTTClient_subscribe(client,top,QOS);
    return 0;
}

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("cause: %s\n", cause);
}
