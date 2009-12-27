/*
Author: Eugene Varlamov
Project: POP3 Server
Company: BMSTU
Description: main file pop3server
*/

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "logger.h"
#include "server.h"

char *log_fname = NULL;
char *drop_fname = NULL;


void sigstop(int signum)
{
    mqlog("Received signal %d. Stopping server.", signum);
    server.fStop = 1;
}

void sigdrop(int signum)
{
    mqlog( "Received signal %d. Drop logs to '%s'.", signum, drop_fname);
    drop_logs( log_fname, drop_fname );
}



int main(int argc, char** argv)
{

    return 0;
}


