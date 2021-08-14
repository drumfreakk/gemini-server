#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>


#define LG_FTL	1
#define LG_ERR	2

void logger(const char *tag, const char *message, char flags);

#endif
