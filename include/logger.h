#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#define LG_FTL		1
#define LG_ERR		2

#define TG_DBG		0
#define	TG_HDR		1
#define	TG_BDY		2
#define	TG_CONN		3
#define	TG_MSG		4
#define	TG_FD		5
#define	TG_SOCK		6
#define	TG_TLS		7

#define LVL_DBG		0
#define LVL_INFO	3
#define LVL_WARN	4
#define LVL_CRIT	10

struct logger_config {
	int level;
	char file[1024];
} logger_config;

void logger(int tag, const char *message, char flags);

#endif
