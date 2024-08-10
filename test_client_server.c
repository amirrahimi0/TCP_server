#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "client.h"
#include "server.h"

void *mock_server(void *arg)
{
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    char buffer[1024];

    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9000);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 1);

    addr_size = sizeof(serverAddr);
    newSocket = accept(serverSocket, (struct sockaddr *)&serverAddr, &addr_size);

    recv(newSocket, buffer, 1024, 0);
    printf("Received: %s\n", buffer);

    close(newSocket);
    close(serverSocket);
    return NULL;
}

START_TEST(test_create_client_socket)
{
    int clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    ck_assert_int_gt(clientSocket, 0);
    close(clientSocket);
}
END_TEST

START_TEST(test_connect_to_server)
{
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, mock_server, NULL);
    sleep(1);

    int clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9000);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ck_assert_int_eq(connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)), 0);
    close(clientSocket);

    pthread_cancel(server_thread);
    pthread_join(server_thread, NULL);
}
END_TEST

START_TEST(test_send_data)
{
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, mock_server, NULL);
    sleep(1);

    int clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9000);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    ck_assert_int_eq(send(clientSocket, "test_message", strlen("test_message"), 0), strlen("test_message"));
    close(clientSocket);

    pthread_cancel(server_thread);
    pthread_join(server_thread, NULL);
}
END_TEST

Suite *client_server_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("ClientServer");

    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_create_client_socket);
    tcase_add_test(tc_core, test_connect_to_server);
    tcase_add_test(tc_core, test_send_data);

    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = client_server_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
