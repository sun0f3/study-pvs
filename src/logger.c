/*
Author: Eugene Varlamov
Project: POP3 Server
Company: BMSTU
Description: logger.c
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "logger.h"

void open_logf(char *fname );
void extract_messages(void);
void *logthread(void *param);


void init_logger(char * logfname)
{
    struct mq_attr matr;
    
    pthread_mutex_init( &logger.mutex_log, NULL );
    open_logf(logfname);
    logger.mqueue = mq_open( PMQ_NAME, O_RDWR|O_CREAT, S_IRWXU|S_IRWXG, NULL);
    mq_getattr( logger.mqueue, &matr );
    logger.maxsz = matr.mq_msgsize + 10;
    logger.message_buf = (char *)malloc(logger.maxsz);
    if (matr.mq_curmsgs) extract_messages();
    mqlog( "Queue \"%s\":\n\t- stores at most %ld messages\n\t- large at most %ld bytes each\n\t- currently holds %ld messages\n", PMQ_NAME, matr.mq_maxmsg, matr.mq_msgsize, matr.mq_curmsgs);
}


int start_logger(char * logfname)
{
    init_logger(logfname);
    mqlog("Starting logger...");
    logger.fStop = 0;
    return pthread_create(&logger.thrd, NULL, logthread, NULL);
}


void stop_logger(void)
{
    void *status;
    logger.fStop = 1;
    mqlog("Stopping Logger...");
    pthread_join(logger.thrd, &status);
    destroy_logger();
}

void log_string( const char *str )
{
    mq_send( logger.mqueue, str, strlen(str)+1, 0 );
}

void *logthread(void *param)
{
    while ((logger.fStop != 1) && (logger.mqueue > 0)) extract_messages();
    return NULL;
}

void open_logf(char *fname )
{
    char *str;
    
    asprintf( &str, "\\begin{verbatim}\n" );
    if ((logger.logfd = creat(fname, S_IRUSR|S_IWUSR )) < 0)
        printf( "Fail to create log file '%s'.\n", fname );
    else
        write(logger.logfd, str, strlen(str) );
    free(str);
}



void drop_logs( char *fromfn, char *tofn )
{
    char *str = NULL;
    
    pthread_mutex_lock(&logger.mutex_log);
    if (logger.logfd > 0) close( logger.logfd );
    logger.logfd = -1;
    asprintf( &str, "cp %s %s", fromfn, tofn );
    system(str);
    free(str);
    open_logf(fromfn);
    pthread_mutex_unlock(&logger.mutex_log);
}


