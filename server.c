#include "main.h"

typedef struct
{
    sqlite3 *db;
    int newSocket;
} ThreadArgs;

void *print_messages()
{
    while (1)
    {
        sleep(10);
        sqlite3 *db;
        sqlite3_stmt *stmt;
        int rc;
        rc = sqlite3_open("test.db", &db);
        if (rc)
        {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
            return 0;
        }

        const char *sql = "SELECT m_int FROM messages WHERE m_type_int = 1 AND m_int != 0 ORDER BY m_int ASC;";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            return 0;
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            int m_int = sqlite3_column_int(stmt, 0);
            printf("__%d__\n", m_int);
        }

        if (rc != SQLITE_DONE && rc != SQLITE_ROW)
        {
            fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
}

const char *timefunc()
{
    time_t current_time;
    struct tm *local_time;
    static char time_string[9];
    time(&current_time);
    local_time = localtime(&current_time);
    strftime(time_string, sizeof(time_string), "%H:%M:%S", local_time);
    return time_string;
}

void insert_message(sqlite3 *db, int m_type_int, const char *m_data, const char *m_date, int m_int)
{

    const char *sql = "INSERT INTO messages (m_type_int, m_data, m_int, m_date) VALUES (?, ?, ?, ?);";
    sqlite3_stmt *stmt;

    int rc;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Can't prepare insert statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, m_type_int);
    sqlite3_bind_text(stmt, 2, m_data, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, m_int);
    sqlite3_bind_text(stmt, 4, m_date, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Insert failed: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
}

void *listing(void *args)
{
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    sqlite3 *db = threadArgs->db;
    int newSocket = threadArgs->newSocket;

    while (1)
    {
        char buffer1[1024];
        memset(buffer1, 0, sizeof(buffer1));
        if (recv(newSocket, buffer1, 1024, 0) < 0)
        {
            fprintf(stderr, "Error receiving data\n");
            close(newSocket);
        }
        const char *time = timefunc();
        char *data = buffer1 + 3;
        char type[10];
        strncpy(type, buffer1, 2);
        type[2] = '\0';

        if (strcmp(type, "p1") == 0)
        {
            insert_message(db, 1, data, time, atoi(data));
        }
        else if (strcmp(type, "p2") == 0)
        {
            insert_message(db, 0, data, time, 0);
        }

        printf("TIME = %s | message = %s | type = %s | data = %s\n", time, buffer1, type, data);
    }
    free(threadArgs);
    return NULL;
}

int main()
{
    // db connection
    sqlite3 *db;
    int rc;
    rc = sqlite3_open("test.db", &db);
    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    // reading config file
    FILE *config = fopen("fconfig.ini", "r");
    if (config == NULL)
    {
        fprintf(stderr, "Error opening config file\n");
        sqlite3_close(db);
        return 1;
    }
    char buff[255];
    char buff1[10];
    if (fgets(buff, 255, config) == NULL)
    {
        fprintf(stderr, "Error reading IP from config file\n");
        fclose(config);
        sqlite3_close(db);
        return 1;
    }
    char *ip = buff + 10;

    if (fgets(buff1, 255, config) == NULL)
    {
        fprintf(stderr, "Error reading port from config file\n");
        fclose(config);
        sqlite3_close(db);
        return 1;
    }
    int port = atoi(buff1 + 7);
    fclose(config);

    pthread_t thread1;
    if (pthread_create(&thread1, NULL, print_messages, NULL) != 0)
    {
        fprintf(stderr, "Error creating thread\n");
        sqlite3_close(db);
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "Error creating socket\n");
        sqlite3_close(db);
        return 1;
    }

    struct sockaddr_in serverAddr;
    int newSocket;
    struct sockaddr_in newAddr;
    socklen_t addr_size;
    char buffer[1024];

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip);

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        fprintf(stderr, "Error binding socket\n");
        close(sockfd);
        sqlite3_close(db);
        return 1;
    }

    if (listen(sockfd, 5) < 0)
    {
        fprintf(stderr, "Error listening on socket\n");
        close(sockfd);
        sqlite3_close(db);
        return 1;
    }

    pthread_t threads[5];
    int i = 0;
    while (1)
    {
        ThreadArgs *threadArgs = malloc(sizeof(ThreadArgs));
        newSocket = accept(sockfd, (struct sockaddr *)&newAddr, &addr_size);
        if (newSocket < 0)
        {
            fprintf(stderr, "Error accepting connection\n");
            close(sockfd);
            sqlite3_close(db);
            return 1;
        }

        printf("client number %d entered.\n", i + 1);

        threadArgs->db = db;
        threadArgs->newSocket = newSocket;

        if (pthread_create(&threads[i], NULL, listing, threadArgs) != 0)
        {
            fprintf(stderr, "Error creating thread\n");
            close(sockfd);
            sqlite3_close(db);
            return 1;
        }

        i++;
    }

    close(sockfd);
    sqlite3_close(db);
    return 0;
}