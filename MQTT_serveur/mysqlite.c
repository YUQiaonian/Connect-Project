#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "mysqlite.h"

#define BUFSIZE 200
sqlite3 *myCreatedb(char *dbname)
{
    sqlite3 *db = NULL;
    int rc;
    rc = sqlite3_open(dbname, &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    return db;
}

void createTable(sqlite3 *db,const char *tableinfo)
{
    char * pErrMsg = "0";
    int result = sqlite3_exec(db, tableinfo,NULL,NULL, &pErrMsg );
    if( result != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", pErrMsg);
        sqlite3_free(pErrMsg);
    }
    //sqlite3_close(db);
}

void CreateMenu(sqlite3 *db)
{
  const char *tableinfo = " PRAGMA FOREIGN_KEYS=ON;\
    DROP TABLE IF EXISTS MENU;\
    CREATE TABLE MENU(\
	  CID INTEGER PRIMARY KEY AUTOINCREMENT,\
	  COOKINGTIME INTEGER NOT NULL\
    );";
  createTable(db,tableinfo);
  insertIntoDatabase(db,"INSERT INTO MENU(COOKINGTIME) VALUES(10);");
  insertIntoDatabase(db,"INSERT INTO MENU(COOKINGTIME) VALUES(5);");
  insertIntoDatabase(db,"INSERT INTO MENU(COOKINGTIME) VALUES(15);");
  insertIntoDatabase(db,"INSERT INTO MENU(COOKINGTIME) VALUES(10);");
  insertIntoDatabase(db,"INSERT INTO MENU(COOKINGTIME) VALUES(20);");
  insertIntoDatabase(db,"INSERT INTO MENU(COOKINGTIME) VALUES(15);");
  insertIntoDatabase(db,"INSERT INTO MENU(COOKINGTIME) VALUES(5);");
  insertIntoDatabase(db,"INSERT INTO MENU(COOKINGTIME) VALUES(10);");
  insertIntoDatabase(db,"INSERT INTO MENU(COOKINGTIME) VALUES(10);");
  insertIntoDatabase(db,"INSERT INTO MENU(COOKINGTIME) VALUES(15);");
}


void CreateClient(sqlite3 *db)
{
  const char *tableinfo = " DROP TABLE IF EXISTS CLIENT;\
    CREATE TABLE CLIENT(\
    CID INTEGER PRIMARY KEY AUTOINCREMENT,\
    TABLE_ID INTEGER NOT NULL,\
	  DISH_ID INTEGER NOT NULL,\
	  FOREIGN KEY(DISH_ID) REFERENCES MENU(CID) on delete cascade on update cascade\
    );";
  createTable(db,tableinfo);
}

void CreateCarcontrol(sqlite3 *db)
{
  const char *tableinfo = " DROP TABLE IF EXISTS TABLES;\
    CREATE TABLE TABLES(\
    CID INTEGER PRIMARY KEY AUTOINCREMENT,\
	  TABLE_ID INTEGER NOT NULL,\
    WAIT_TIME INTEGER NOT NULL,\
	  DATE_INSERTION TIMESTAMP not null default (datetime('now','localtime'))\
    );";
  createTable(db,tableinfo);
}

sqlite3 *CreateControlDB(char *filename)
{
  sqlite3 *db = myCreatedb(filename);
  CreateMenu(db);
  CreateClient(db);
  CreateCarcontrol(db);

  return db;
}


void insertIntoDatabase(sqlite3 *db,char *info)
{
    char * pErrMsg = 0;
    int result = sqlite3_exec(db, info, NULL,NULL, &pErrMsg);
    if(result != SQLITE_OK){
	     printf("DEBUG:KO,cannot INSERT VALUES\n");
	     return;
  	}
}

void deleteFromTable(sqlite3 *db,char *info)
{
    char *pErrMsg = 0;
    sqlite3_exec(db, info, NULL,NULL, &pErrMsg);
}

void deleteclientbytable_nb(sqlite3 *db,int table_id)
{
  char sql[256];
  sprintf(sql,"delete from TABLES where TABLE_ID=%d",table_id);
  int rc = sqlite3_exec(db,sql,NULL,NULL,NULL);
  if(SQLITE_OK == rc)
	{
		printf("OK\n");
	}
	else
	{
		printf("KO\n");
	}
	return ;
}

void conditionselectFromTable(sqlite3 *db,char *tablename,char *condition ,char **dbresult,int j,int nrow,int ncolumn,int index){
  char * pErrMsg = 0;
  char dbinfo[256];
  sprintf(dbinfo,"select *from %s where %s",tablename,condition);
  int ret = sqlite3_get_table(db,dbinfo,&dbresult,&nrow,&ncolumn,&pErrMsg);
  if(ret != SQLITE_OK){
      printf("DEBUG:condition select from table errer\n");
  }
}

int select_callback(void * data, int col_count, char ** col_values, char ** col_Name)
{
    int i;
    for( i=0; i < col_count; i++){
        printf( "i= %d %s = %s\n",i, col_Name[i], col_values[i] == 0 ? "NULL" : col_values[i] );
    }

    return 0;
}

void databaseQuery(sqlite3 *db,char *tablename)
{
    char * pErrMsg = 0;
    char info[BUFSIZE];
    sprintf(info,"select * from %s",tablename);
    sqlite3_exec(db,info, select_callback, 0, &pErrMsg);
    sqlite3_close(db);
}

