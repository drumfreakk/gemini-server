#include "logger.h"

void logger(const char *tag, const char *message){
	time_t now;
	time(&now);
	printf("%.24s [%-4s]: %s\n", ctime(&now), tag, message);
	fflush(NULL);
}
