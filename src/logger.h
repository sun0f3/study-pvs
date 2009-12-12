/*
Author: Eugene Varlamov
Project: POP3 Server
Company: BMSTU
Description: Logger.h 
*/
#ifndef _LOGGER_H__
#define _LOGGER_H__

#include <pthread.h>
#include <stdlib.h>
#include <mqueue.h>


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


int start_logger(char * logfname);
void stop_logger(void);
void drop_logs( char *fromfn, char *tofn );
void log_string( const char *str );


#endif

