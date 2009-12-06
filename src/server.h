/*
Author: Igor Kashin
Project: POP3 Server
Company: BMSTU
Description: File for server
*/

#pragma once

#include <stdlib.h>
#include <sys/queue.h>
#include "parser.h"


struct client {
	STAILQ_ENTRY(client) next;
	int fd;
};


struct clients {
    STAILQ_HEAD(clientlist, client) cliqueue;
	fd_set read_set;
	int maxfd;
};


struct serv_struct {
        int fStop;
        struct clients sclients;
        char buf[255];
} server;
extern struct serv_struct server;


/*!
 * \name server_init
 *
 * Инициализация сервера
 */
extern void server_init(void);


/*!
 * \name server_start
 *
 * \param port
 *
 * Запуск сервера на прослушивание
 */
extern void server_start(int);


/*!
 * \name client_add
 *
 * \param servfd Дескриптор сокета
 *
 * Добавление клиента
 */
int client_add(int);


/*!
 * \name client_remove
 *
 * \param pClient
 *
 * Удаляет клиента
 */
void client_remove(struct client*);


/*!
 * \name client_accept
 *
 * \param servfd дескрипотор соктеа
 *
 * "Встреча" клиента
 */
int client_accept(int);


/*!
 * \name pop3bindsock
 *
 * \param port
 *
 * Связывание с сокетом
 */
int pop3_bindsock(int);
