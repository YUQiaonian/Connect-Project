#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "MQTTClient.h"
#include "mymqtt.h"
#include "mysqlite.h"

MQTTClient client;
MQTTClient_deliveryToken token;
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
MQTTClient_message pubmsg = MQTTClient_message_initializer;
volatile MQTTClient_deliveryToken deliveredtoken;
sqlite3* db = NULL;

int table_nb_fin = 0;


void get_time(int *hour, int *min, int *sec)
{
  char wday[10];
  char mon[10];
  char mday[10];
  char year[10];
  time_t timep;
  time(&timep);
  sscanf(ctime(&timep),"%s %s %s %d:%d:%d %s",wday,mon,mday,hour,min,sec,year);
}

void get_timefromtable(char *time, int *hour, int *min, int *sec)
{
  char day[30];
  sscanf(time,"%s %d:%d:%d\n",day,hour,min,sec);

}

void calc_time(int *hour, int *min, int *sec, int wait_time)
{
  *hour = (*hour + (*min + wait_time)/60)%24;
  *min = (*min + wait_time)%60;
}

void calc_time_test(int *hour, int *min, int *sec, int wait_time)
{

  *hour = (*hour + (*min+((*sec+wait_time)/60))/60)%24;
  *min = (*min+((*sec+wait_time)/60))%60;
  *sec = (*sec+wait_time)%60;
}

int finddishtime(int dishnb)
{
  int dishtime = 0;
	char sql[256];
	sqlite3_stmt * stmt;

  sprintf(sql,"select * from MENU where CID='%d'",dishnb);
  sqlite3_prepare(db,sql,-1,&stmt,NULL);
  if(sqlite3_step(stmt) == SQLITE_ROW)
  {
   dishtime = sqlite3_column_int(stmt,1);
  }
  sqlite3_finalize(stmt);
  return dishtime;
}

void calcmenutime(int *menutime,int dishnb)
{
  *menutime += finddishtime(dishnb);
}

int Convertseconds(int hour,int min, int sec)
{
  return hour*360+min*60+sec;
}

void calcwaittime(int *wait_time,int menutime){
  int nrow = 0;
  int ncolumn = 0;
  char *zErrMsg = NULL;
  char **azResult= NULL;

  int hour = 0;
  int min = 0;
  int sec = 0;

  int hour_now = 0;
  int min_now = 0;
  int sec_now = 0;

  int wait_prochan=0;
  get_time(&hour_now,&min_now,&sec_now);

  char *sql="select *from TABLES";
  sqlite3_get_table( db , sql , &azResult , &nrow , &ncolumn , &zErrMsg );
  if((nrow+1)*ncolumn<=4)
  {
    *wait_time = menutime;
  }
  else{
    get_timefromtable(azResult[(nrow+1)*ncolumn -1],&hour,&min,&sec);
    wait_prochan = atoi(azResult[(nrow+1)*ncolumn-2]);
    if(Convertseconds(hour,min,sec) + wait_prochan <= Convertseconds(hour_now,min_now,sec_now)){
      *wait_time = menutime + 0;
    }
    else{
      *wait_time = menutime +(Convertseconds(hour,min,sec) + wait_prochan - Convertseconds(hour_now,min_now,sec_now));
    }
  }
}

void valider()
{
  deleteclientbytable_nb(db,table_nb_fin);
  table_nb_fin = 0;
}

void processor()
{
  int nrow = 0;
  int ncolumn = 0;
  char *zErrMsg = NULL;
  char **azResult= NULL;

  int hour = 0;
  int min = 0;
  int sec = 0;

  int hour_now = 0;
  int min_now = 0;
  int sec_now = 0;

  get_time(&hour_now,&min_now,&sec_now);

  int wait_time = 0;
  int table_nb = 0;


  char playload[20];
  char *sql="select *from TABLES";
  sqlite3_get_table( db , sql , &azResult , &nrow , &ncolumn , &zErrMsg );
  //printf("nrow=%d ncolumn=%d\n",nrow,ncolumn);
  if((nrow+1)*ncolumn<=4)
  {
    printf("Dont have client");
    mqtt_client_publish(TOPICROB,"0");
  }
  else{
    for(int i = 4;i<(nrow+1)*ncolumn;i+=ncolumn)
    {
      table_nb = atoi(azResult[i+1]);
      wait_time = atoi(azResult[i+2]);

      get_timefromtable(azResult[i+3],&hour,&min,&sec);

      calc_time_test(&hour, &min, &sec, wait_time);
      if(Convertseconds(hour,min,sec)<=Convertseconds(hour_now,min_now,sec_now))
      {
        table_nb_fin=table_nb;
        printf("%d:%d:%d %d\n",hour_now,min_now,sec_now,table_nb_fin);
        printf("%d:%d:%d %d\n",hour,min,sec,table_nb_fin);
        break;
      }
    }
    sprintf(playload,"%d",table_nb_fin);
    mqtt_client_publish(TOPICROB,playload);
  }

}

void control(char ch)
{

  switch(ch)
  {
      case 'R':
        processor();
        break;
      case 'O':
        valider();
      default:
        break;
  }
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message){

  char payloadptr[256]="\0";
  memcpy(payloadptr,message->payload,message->payloadlen);


  printf("Message arrived\n");
  printf("{\n");
  printf("topic:{%s}\n", topicName);
  printf("message: {%s}\n}\n",payloadptr);

  control(payloadptr[0]);

  MQTTClient_freeMessage(&message);
  MQTTClient_free(topicName);
  return 1;
}

int mqtt_client_open(){
    int rc;

    MQTTClient_create(&client, ADDRESS,CLIENTID,MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

    conn_opts.keepAliveInterval = 200;
    conn_opts.cleansession = 1;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS){
        printf("Failed to connect, return code %d \n", rc);
        return -1;
    }
    printf("\nSuccess to connect!\n");
    return 0;
}

int main(int argc, char* argv[]){

  db = CreateControlDB("test.db");
  mqtt_client_open();
  mqtt_client_subscribe(TOPICPC);

  int ch;

  do{
    ch = getchar();	
  }while(ch!='q'||ch!='Q');

  MQTTClient_disconnect(client, 10000);
  MQTTClient_destroy(&client);

  return 0;
}
