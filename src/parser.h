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




#include <stdio.h>
#include <string.h>
#include <unistd.h> //вызовы read/write
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "logger.h" //логгер
#include "parser.h"


void countMails();
void UpdateMails(void);

//флаг fAdded выставляется, если клиент только что был добавлен.
//По спецификации, мы должны отправить приветствие
int handle_client(int fd, int fAdded){
    int sz = 0;

    parser.current_fd = fd;
    if (fAdded != 0) {
        POP3OK( sz = , parser.buf, sizeof(parser.buf), "POP3 server ready" );
        sz++;// = strlen(parser.buf)+1;
        parser.state=POP3_STATE_AUTHORIZATION;
        parser.deletedMsgs=0;
    } else if ((sz = read( fd, parser.buf, sizeof(parser.buf))) > 0) {
            memset(&parser.buf[sz], 0x00, sizeof(parser.buf)-sz);
            mqlog( "String readed from client: '%s'", parser.buf );
            sz = processCMD(parser.buf, sizeof(parser.buf))+1;
        } else return 0;

    mqlog( "String written to client: '%s'", parser.buf );
    if (sz < 0) return -sz;
    return write( fd, parser.buf, sz );
}

int processCMD(char *buf, int len) {
    static int i, j;
    static char upper_cmd[BUF_LEN];

    for (i = 0; i < sizeof(commands)/sizeof(struct commands); i++) {
        for (j = 0; j <= strlen(commands[i].CMD); j++) upper_cmd[j] = toupper(buf[j]);
        if (!strncmp(upper_cmd, commands[i].CMD, strlen(commands[i].CMD) ))
            return commands[i].exec( buf, len );
    }

    POP3OK( return, buf, len, "POP3 server response stub for %s", __func__);
}

int processSTUB(char* buf, int len){
    char *str = NULL;

    asprintf( &str, "+OK POP3 server response stub for '%s'", buf );
    strncpy(buf, str, len);
    free(str);
    return strlen(buf);
}

//Передаёт серверу имя пользователя.
int processUSER(char* buf, int len) {
	buf[len-2]=0;	//Избавление от символов конца

	//buf[strlen(buf)-2] = 0;
	strncpy(parser.userName,buf+5, sizeof(parser.userName));
	//strncpy(parser.userName,buf+5, 5);
	snprintf(parser.userHome,sizeof(parser.userHome),"%s/%s",MAIL_ROOT_PATH,parser.userName);

	struct stat st;
	if(stat(parser.userHome,&st) != 0)
		POP3ERR( return, buf, len, "User %s's Home '%s' not found '%i' bebebe",parser.userName, parser.userHome,strlen(buf));
	if(parser.state!=POP3_STATE_AUTHORIZATION)
		POP3ERR( return, buf, len, "%s","Not in authorization state.");
	parser.state = POP3_STATE_TRANSACTION;
	POP3OK( return, buf, len, "%s","User authorized");
}

int processQUIT(char* buf, int len){
    int res = 0;
    if (parser.state==POP3_STATE_AUTHORIZATION)    {
    	POP3OK( return, buf, len, "POP3 server signing off");
	}
	if(parser.state==POP3_STATE_TRANSACTION){
		parser.state=POP3_STATE_UPDATE;
		if (parser.deletedMsgs!=0){
			deleteMsgs();
			free(parser.deletedMsgs);
		}
		countMails();
		POP3OK( return , buf, len, "POP3 server signing off (%d messages left)",parser.totalMailCount);
	}

    return res;
}

//Сервер возвращает количество сообщений в почтовом ящике
int processSTAT(char* buf, int len)
{
	if(parser.state != POP3_STATE_TRANSACTION)
		POP3ERR( return, buf, len, "POP3 server is not in TRANSACTION state");

	parser.lastMsg=1;
	countMails();

	POP3OK( return, buf, len, "%d %d\r\n", parser.totalMailCount, parser.totalMailSize);
}
//Если был передан аргумент, то сервер выдаёт информацию об указанном сообщении.
//Если аргумент не был передан, то сервер выдаёт информацию обо всех сообщениях, находящихся в почтовом ящике.
//Сообщения, помеченные для удаления, не перечисляются.
int processLIST(char* buf, int len){
	buf[len-2]=0; //избавление от символов конца
	int msg_id=atol(buf+5); //LIST [msg_id]

	if(parser.state != POP3_STATE_TRANSACTION)
		POP3ERR( return, buf, len, "POP3 server is not in TRANSACTION state\r\n");

	if(msg_id>0)
	{
		int msgSize=getMessageSize(msg_id);
		if (msgSize<0)
			POP3ERR( return, buf, len, "no such message\r\n");
		else
			POP3OK( return, buf, len, "%d %d\r\n", msg_id, msgSize);
	}
	//parser.lastMsg=1;
	//countMails();

	countMails();

	if (parser.totalMailCount<1)
	{
		POP3OK( return, buf, len, "mailbox is empty\r\n");
	}
	else
	{
		char listbuf[parser.totalMailCount*MAIL_ATTR_SIZE];

		sprintf(listbuf,"+OK messages %d (%d octets)\n",parser.totalMailCount, parser.totalMailSize);
		listMailbox(listbuf);
		strcat(listbuf,"\r\n.\r\n");
		write(parser.current_fd,listbuf,strlen(listbuf));
		return -1*strlen(listbuf);
	}
}
//Сервер передаёт сообщение с указанным номером.
int processRETR(char* buf, int len){

	buf[len-2]=0;//избавление от символов конца
	int msg_id=atol(buf+5);//RETR msg_id

	if(parser.state!=POP3_STATE_TRANSACTION)
	{
		POP3ERR( return, buf, len, "POP3 server is not in TRANSACTION state\r\n");;
	}

	countMails();
	if (parser.totalMailCount<1)
	{
		POP3OK( return, buf, len, "mailbox is empty\r\n");
	}

	if(msg_id<parser.totalMailCount)
	{
		int msgSize=getMessageSize(msg_id);
		if (msgSize<0)
			POP3ERR( return, buf, len, "no such message\r\n");
		else
			sendMessageFile(msg_id,parser.current_fd);
	}



	return -1*strlen(buf);

}

//Сервер помечает указанное сообщение для удаления.
//Сообщения, помеченные на удаление, реально удаляются только после закрытия транзакции
//(закрытие транзакций происходит обычно после посыла команды QUIT
int processDELE(char* buf, int len){
	buf[len-2]=0;
	int msg_id=atol(buf+5);

	if (msg_id <= 0)
	{
		POP3ERR( return, buf, len, "msg_id is missing\r\n");
	}
	if(parser.state!=POP3_STATE_TRANSACTION)
	{
		POP3ERR( return, buf, len, "POP3 server is not in TRANSACTION state\r\n");
	}
	countMails();
	if(msg_id>parser.totalMailCount)
	{
		POP3ERR( return, buf, len, "There is no such message in mailbox\r\n");
	}

	if (parser.deletedMsgs == 0)
	{
		parser.deletedMsgs = (int*)calloc(parser.totalMailCount,sizeof(int));
		int i;
		for (i=0; i<parser.totalMailCount; parser.deletedMsgs[i++]=0);
	}

	if (parser.deletedMsgs[msg_id-1])	{
		POP3ERR( return, buf, len, "Message %d is already deleted\r\n",msg_id);
	}
	else{
		parser.deletedMsgs[msg_id-1]=1;
		POP3OK( return, buf, len, "Message %d is deleted\r\n",msg_id);
	}
	return 0;
}

//Этой командой производится откат транзакций внутри сессии.
//Например, если пользователь случайно пометил на удаление какие-либо сообщения, он может убрать эти пометки, отправив эту команду
int processRSET(char* buf, int len){
	if(parser.state!=POP3_STATE_TRANSACTION)
	{
		POP3ERR( return, buf, len, "POP3 server is not in TRANSACTION state\r\n");
	}
	if (parser.deletedMsgs != 0)	{
		int i;
		for(i=0; i<parser.totalMailCount; parser.deletedMsgs[i++]=0);
	}
	countMails();
	POP3OK(return, buf, len, "maildrop has %d messages (%d octets)\r\n",parser.totalMailCount,parser.totalMailSize);
}

int getMessageSize(int msg_id){
	   	int msgSize=-1;
        DIR *d;
        struct dirent *pDirEnt;

        if( (d = opendir(parser.userHome)) != NULL)
        {
        	pDirEnt = readdir(d);//для того, чтобы не читать "." и ".."
        	pDirEnt = readdir(d);

        	int i;
            for(i=0; i<msg_id && pDirEnt != NULL; i++)
			{
				pDirEnt = readdir(d);
			}

			struct stat fstat;
			char fileName[MAX_PATH];
			sprintf(fileName,"%s/%s",parser.userHome,pDirEnt->d_name);
			if (stat(fileName, &fstat)!= 0)
				{
					perror("Error reading stat()\n");
				}
			else
				msgSize = fstat.st_size;

			closedir(d);
        }
        return msgSize;
}

//добавляет номера и размеры писем в буфер
int listMailbox(char *buf){
        DIR *d;
        struct dirent *pDirEnt;
        struct stat fstat;
        char fileName[MAX_PATH];

        if( (d = opendir(parser.userHome)) != NULL)
        {
        	pDirEnt = readdir(d);//для того, чтобы не читать "." и ".."
        	pDirEnt = readdir(d);
        	pDirEnt = readdir(d);

        	int mailCount = 0;
            while(pDirEnt != NULL)
			{
				sprintf(fileName,"%s/%s",parser.userHome,pDirEnt->d_name);
				if (stat(fileName, &fstat)!= 0)
				{
					perror("Error reading stat()\n");
				}
				mailCount++;
				sprintf(buf+strlen(buf),"%d %d\n",mailCount,fstat.st_size);
				pDirEnt = readdir(d);
			}

			closedir(d);
        }
}

//считает количество файлов в юзерской папке и их размер
void countMails(){
        parser.totalMailSize=0;
        parser.totalMailCount = 0;
        DIR *d;
        struct dirent *pDirEnt;
        struct stat fstat;
        char fileName[MAX_PATH];

        if( (d = opendir(parser.userHome)) != NULL)
        {
        	pDirEnt = readdir(d);//для того, чтобы не читать "." и ".."
        	pDirEnt = readdir(d);
        	pDirEnt = readdir(d);

            while(pDirEnt != NULL)
			{
				sprintf(fileName,"%s/%s",parser.userHome,pDirEnt->d_name);
				if (stat(fileName, &fstat)!= 0)
				{
					perror("Error reading stat()\n");
				}
				parser.totalMailSize += fstat.st_size;
				parser.totalMailCount++;
				pDirEnt = readdir(d);
			}

			closedir(d);
        }
}

//отправка письма
int sendMessageFile(int msg_id, int fdw){
        DIR *d;
        struct dirent *pDirEnt;
        struct stat fstat;
        char fileName[MAX_PATH];

        if( (d = opendir(parser.userHome)) != NULL)
        {
        	pDirEnt = readdir(d);//"." и ".."
        	pDirEnt = readdir(d);

        	int i;
            for(i=0; i<msg_id && pDirEnt != NULL; i++)
			{
				pDirEnt = readdir(d);
			}

			printf("entering the state\n");
			int fdr = -1;
			sprintf(fileName,"%s/%s",parser.userHome,pDirEnt->d_name);

			if (stat(fileName, &fstat)!= 0)
				{
					char err[] = "-ERR error opening message\r\n";
					write(fdw,err,strlen(err));
				}
			else if ((fdr = open( fileName, O_RDONLY )) > 0)			{
				printf("filename:%s, bufsize: %d\n",fileName, fstat.st_size);

				int res = 0;
				unsigned char tmpbuf[fstat.st_size];

				sprintf(tmpbuf,"+OK %d octets\r\n",fstat.st_size);
				write(fdw,tmpbuf,strlen(tmpbuf));

				while ((res = read( fdr, tmpbuf, fstat.st_size-1 )) > 0) {
				    if (tmpbuf[res-1] == 0x0A) res--;
				    write( fdw, tmpbuf, res );
                }
				sprintf(tmpbuf,"\r\n.\r\n");
				write(fdw,tmpbuf,strlen(tmpbuf));

				close(fdr);
			}	//AB-AB: криво:
			/*else if ((fdr = fopen(fileName,"r")) > 0)			{
				fdr = fopen(fileName,"r");
				printf("filename:%s\n",fileName);

				char tmpbuf[fstat.st_size];

				sprintf(tmpbuf,"+OK %d octets\r\n",fstat.st_size);
				write(fdw,tmpbuf,strlen(tmpbuf));

				if(fread(tmpbuf,sizeof(char),fstat.st_size,fdr)>0)
				{
					write(fdw,tmpbuf,strlen(tmpbuf));
				}
				sprintf(tmpbuf,"\r\n.\r\n");
				write(fdw,tmpbuf,strlen(tmpbuf));

				fclose(fdr);
			}*/
			else			{
				char err[] = "-ERR error reading message\r\n";
				write(fdw,err,strlen(err));
			}


			closedir(d);
        }
        printf("return\n");
        return 0;
}

//удаление файлов писем, помеченных как удаленные
int deleteMsgs()
{
        DIR *d;
        struct dirent *pDirEnt;

        if( (d = opendir(parser.userHome)) != NULL)
        {
        	pDirEnt = readdir(d);//"." и ".."
        	pDirEnt = readdir(d);

        	char fileName[MAX_PATH];
        	char *cmd = NULL;
        	int i;
            for(i=0; pDirEnt != NULL; i++)
			{
				pDirEnt = readdir(d);
				if (parser.deletedMsgs[i])
				{
					sprintf(fileName,"%s/%s",parser.userHome,pDirEnt->d_name);
					asprintf(&cmd, "rm %s -f", fileName);
					system(cmd);
				}
			}

			closedir(d);
        }
        return 0;
}
