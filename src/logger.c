#include "logger.h"

//TODO enum or defs of classes to set level


void logger(const char *tag, const char *message, char flags){
	time_t now;
	time(&now);
	fflush(NULL);
	if((flags & LG_ERR) == LG_ERR){
		printf("%.24s [%-4s]: %s: %s\n", ctime(&now), tag, message, strerror(errno));
	} else {
		printf("%.24s [%-4s]: %s\n", ctime(&now), tag, message);
	}

	if((flags & LG_FTL) == LG_FTL){
		printf("Exiting, more information in previous log entry\n");
		exit(-1);
	}
}
