#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include "include/common.h"
#include "include/typedef.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        LOGD("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    LOGD("\n");
    return 0;
}

int sql3_insert(int ID, char *fs_code ) {

    char *sql;
    //sprintf(sql, "INSERT INTO FS_DATE VALUES (%d, '%s', %.1f, %.1f, %.1f)", ID, date, get, get_max, get_min);
}

int sql3_test() {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    
    /* Open database */
    rc = sqlite3_open("test.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }

    /* Create SQL statement */
    sql = "CREATE TABLE FS_DATE("  \
            "ID INT PRIMARY KEY     NOT NULL," \
            "DATE           TEXT ," \
            "TOTAL-GET      REAL ," \
            "TOTAL-GET-MAX  REAL ," \
            "TOTAL-GET-MIN  REAL );";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
    } else {
        fprintf(stdout, "Table created successfully\n");
    }
    sqlite3_close(db);
    return 0;
}
