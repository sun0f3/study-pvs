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

