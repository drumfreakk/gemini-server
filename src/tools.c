#include "tools.h"

//TODO fix the string lengths

void fatal(const char *message){
	char error_message[100];

	strcpy(error_message, "[!!] Fatal Error ");
	strncat(error_message, message, 83);
	perror(error_message);
	exit(-1);
}

void error(const char *message){
	char error_message[100];

	strcpy(error_message, "[EE] Error ");
	strncat(error_message, message, 89);
	perror(error_message);
}

void warning(const char *message){
	char warn_message[100];
	
	strcpy(warn_message, "[WW] Warning: ");
	strncat(warn_message, message, 86);
	printf("%s\n", warn_message);
}

void info(const char *message){
	char msg[100];

	strcpy(msg, "[II] Info: ");
	strncat(msg, message, 89);
	printf("%s\n", msg);
}

void debug(const char *message){
	char msg[100];

	strcpy(msg, "[DD] Debug: ");
	strncat(msg, message, 88);
	printf("%s\n", msg);
}

void dump(const char* string, const size_t len){
	long unsigned int i, char_no;
	for(char_no = 0; char_no < len; char_no++){
		printf("%02x ", string[char_no]);
		if(((char_no%16)==15) || (char_no==len-1)){
			for(i = 0; i < 15-(char_no%16); i++){
				printf("   ");
			}
			printf("| ");
			for(i = char_no-(char_no%16); i<=char_no; i++){
				if((31 < string[i]) && (string[i] < 127)){
					printf("%c", string[i]);
				} else {
					printf(".");
				}
			}
			printf("\n");
		}
	}
}
