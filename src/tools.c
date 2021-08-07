#include "tools.h"

//TODO newline
//TODO to file

void fatal(const char *message){
	logger("FTL", message);
	perror(NULL);
	exit(-1);
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
