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
    int i = 0;
    
    if (argc != 4) {
        printf("Simple POP3 server usage: <port> <log_file_name> <drop_log_filename>\n");
       return -1;
    }
    log_fname = argv[2];
    drop_fname = argv[3];    
    start_logger(log_fname);

    server_init();
    signal(SIGUSR1, sigstop);
    signal(SIGUSR2, sigdrop);

    server(atoi(argv[1]));
    stop_logger();	
    

    return 0;

    return 0;
}


