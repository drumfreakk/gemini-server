#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h> /* exit() */
#include "logger.h"

/* Print an error message and exit */
void fatal(const char *message);

/* Show a hexadecimal version of a string */
void dump(const char *string, const size_t len);

#endif
