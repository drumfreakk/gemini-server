#ifndef GETCONFIG_H
#define GETCONFIG_H


#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "logger.h"

//TODO generalise
int getconfig(char *log_file, int *log_level, char *keyfile, char *certfile, int *allow_tlsv1_2, char *webroot, int *port);


#endif
