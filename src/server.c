/*
Author: Igor Kashin
Project: POP3 Server
Company: BMSTU
Description: server.c
*/


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include "logger.h"
#include "server.h"


