#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sqlite3.h>
#include <pthread.h>
#include <unistd.h>

typedef struct
{
        sqlite3 *db;
        int newSocket;
} ThreadArgs;

void *print_messages();
const char *timefunc();
void insert_message(sqlite3 *db, int m_type_int, const char *m_data, const char *m_date, int m_int);
void *listing(void *args);

#endif
