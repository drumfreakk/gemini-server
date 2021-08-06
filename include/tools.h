#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h> /* exit() */

/* Print an error message and exit */
void fatal(const char *message);

/* Print an error message */
void error(const char *message);

/* Show a hexadecimal version of a string */
void dump(const char *string, const size_t len);

#endif
