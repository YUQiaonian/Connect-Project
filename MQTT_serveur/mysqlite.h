/*
 *Project name:
 *File name:
 *Description:
 *Author: LU & JIANG
 *Date:
*/

#ifndef __MYSQLITE_H__
#define __MYSQLITE_H__

#ifdef __cplusplus

#pragma message ("C++")
extern "C" {
#endif
#include <sqlite3.h>
sqlite3 *myCreatedb(char *dbname);
void createTable(sqlite3 *db,const char *tableinfo);
void createControltable(sqlite3 *db);
sqlite3 *CreateControlDB(char *filename);

void insertIntoDatabase(sqlite3 *db,char *info);
void deleteFromTable(sqlite3 *db,char *info);
void deleteclientbytable_nb(sqlite3 *db,int table_nb);

int select_callback(void * data, int col_count, char ** col_values, char ** col_Name);
void databaseQuery(sqlite3 *db,char *tablename);

#ifdef __cplusplus
}
#endif

#endif
