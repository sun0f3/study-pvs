/*
Author: Igor Kashin
Project: POP3 Server
Company: BMSTU
Description: Parser for protocol
*/

#pragma once

#define BUF_LEN 100
#define MAX_PATH 256
#define MAIL_ATTR_SIZE 10

/*!
 * \todo Вообще говоря нужно параметром передавать
 */
#define MAIL_ROOT_PATH "maildrop"

#define POP3_STATE_AUTHORIZATION 1
#define POP3_STATE_TRANSACTION 2
#define POP3_STATE_UPDATE 4


/* Сделаем указатель на функци... */
typedef int (*cmd_processor)(char *buf, int len);

/* ... чтобы связать в структуре команду с её обработчиком */
struct commands {
    const char *CMD;
    cmd_processor exec;
};


struct Parser_struct
{
	char buf[BUF_LEN];  //перенесён сюда, чтобы не создавать переменную при каждом вызове функции
	char userName[MAX_PATH];
	char userHome[MAX_PATH];
	unsigned int state;
	unsigned int lastMsg;
	int totalMailCount;
	int totalMailSize;
	int current_fd;
	int *deletedMsgs;
} parser;


#define POP3OK(res, buf, len, msg...) do {\
    snprintf(buf,len,"+OK "msg); \
    res strlen(buf); \
} while (0)

#define POP3ERR(res, buf, len, msg...) do {\
    snprintf(buf,len,"-ERR "msg); \
    res strlen(buf); \
} while (0)


/*!
 * \name handle_client
 *
 * \param fd дескриптор
 * \param fAdded Флаг выставляется, если клиент был добавлен только что
 *
 * По спецификации необходимо отправлять клиенту приветствие
 */
extern int handle_client(int, int);


/* обработчики для команд */
int processCMD(char *buf, int len);
int processSTUB(char* buf, int len);
int processUSER(char* buf, int len);
int processQUIT(char* buf, int len);
int processSTAT(char* buf, int len);
int processLIST(char* buf, int len);
int processRETR(char* buf, int len);
int processDELE(char* buf, int len);
int processRSET(char* buf, int len);


const struct commands commands[] = {
    { "USER", processUSER },
    { "PASS", processSTUB },
    { "QUIT", processQUIT },
    { "STAT", processSTAT },
    { "LIST", processLIST },
    { "RETR", processRETR },
    { "DELE", processDELE },
    { "RSET", processRSET }
};


