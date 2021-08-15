#include "logger.h"

void logger(int tag, const char *message, char flags){
	time_t now;
	char tag_txt[5], output[2048];

	if(tag < logger_config.level && !((flags & LG_FTL) == LG_FTL)){
		return;
	}

	switch(tag){
		case TG_DBG:
			strcpy(tag_txt, "DBG");
			break;
		case TG_HDR:
			strcpy(tag_txt, "HDR");
			break;
		case TG_BDY:
			strcpy(tag_txt, "BDY");
			break;
		case TG_FD:
			strcpy(tag_txt, "FD");
			break;
		case TG_SOCK:
			strcpy(tag_txt, "SOCK");
			break;
		case TG_TLS:
			strcpy(tag_txt, "TLS");
			break;
		case TG_CONN:
			strcpy(tag_txt, "CONN");
			break;
		case TG_MSG:
			strcpy(tag_txt, "MSG");
			break;
		default:
			strcpy(tag_txt, "OTHR");
	}

	time(&now);
	fflush(NULL);
	if((flags & LG_ERR) == LG_ERR){
		snprintf(output, 2048, "%.24s [%-4s]: %s: %s\n", ctime(&now), tag_txt, message, strerror(errno));
	} else {
		snprintf(output, 2048, "%.24s [%-4s]: %s\n", ctime(&now), tag_txt, message);
	}
	printf("%s", output);

	if(logger_config.file[0] != '\0'){
		FILE *stream = fopen(logger_config.file, "a");
		if(stream == NULL){
			fprintf(stderr, "Unable to open log file for output: %s\n", strerror(errno));
		} else{
			fprintf(stream, "%s", output);
			fflush(stream);
			fclose(stream);
		}
	}

	if((flags & LG_FTL) == LG_FTL){
		fprintf(stderr, "Exiting, more information in previous log entry\n");
		exit(-1);
	}
}
