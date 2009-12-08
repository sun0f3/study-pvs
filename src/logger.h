/*
Author: Eugene Varlamov
Project: POP3 Server
Company: BMSTU
Description: Logger.h 
*/
#ifndef _LOGGER_H__
#define _LOGGER_H__

#define PMQ_NAME "/pop3smq"

struct logger_struct {
    int fStop;
    pthread_t thrd;
    mqd_t mqueue;
    long maxsz;
    int logfd;
    char *message_buf;
    pthread_mutex_t mutex_log;

} logger;




#endif
