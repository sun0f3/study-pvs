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

void sigstop(int signum)
{
    mqlog("Received signal %d. Stopping server.", signum);
    server.fStop = 1;
}


int main(int argc, char** argv)
{

    return 0;
}


