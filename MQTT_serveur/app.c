#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "mysqlite.h"


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
  char *sql="select *from TABLES";
  sqlite3_get_table( db , sql , &azResult , &nrow , &ncolumn , &zErrMsg );
  if((nrow+1)*ncolumn<=4)
  {
    printf("Dont have client");

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






int main(int argc, char* argv[]){
  char dbinfo[256];

  db = myCreatedb("test.db");

  //test seulement

  int tabnb;
  int dishnb;
  int menutime = 0;
  int waittime = 0;

  do{
    menutime = 0;
    waittime = 0;
        printf("Enter table number:");
        scanf("%d",&tabnb);
        printf("Enter the dish number(1-10),Enter 0 to confirm the order:");
        scanf("%d",&dishnb);
        while(dishnb!=0){
          if(1<=dishnb&&dishnb<=10){
            sprintf(dbinfo,"INSERT INTO CLIENT(TABLE_ID,DISH_ID) VALUES(%d,%d);",tabnb,dishnb);
            insertIntoDatabase(db,dbinfo);
            calcmenutime(&menutime,dishnb);
            printf("menutime=%d\n",menutime);
            printf("Enter the dish number(1-10),Enter 0 to confirm the order:");
            scanf("%d",&dishnb);
          }
          else{
            printf("Invalid input,Enter the dish number(1-10),Enter 0 to confirm the order:");
            scanf("%d",&dishnb);
          }
        }
        printf("menutime=%d\n",menutime);
        calcwaittime(&waittime,menutime);
        sprintf(dbinfo,"INSERT INTO TABLES(TABLE_ID,WAIT_TIME) VALUES(%d,%d);",tabnb,waittime);
        insertIntoDatabase(db,dbinfo);

      }while(1);


  return 0;
}
