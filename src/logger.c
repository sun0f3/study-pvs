/*
Author: Eugene Varlamov
Project: POP3 Server
Company: BMSTU
Description: logger.c
*/
#include "logger.h"

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


