/*
Author: Eugene Varlamov
Project: POP3 Server
Company: BMSTU
Description: logger.c
*/
#include "logger.h"

void init_logger(char * logfname)
{
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
