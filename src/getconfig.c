#include "getconfig.h"

int getconfig(char *log_file, int *log_level, char *keyfile, char *certfile, int *allow_tlsv1_2, char *webroot, int *port){
	int config_fd = open("/home/kip/gemini-server/config.ini", O_RDONLY);
	char config_file[1024];	//Is this enough?
	char section[1024];
	char *current_char;
	char value[1024];
	int to_copy;
	
	char sections[][20] = {"[logger]", "[tls]", "[site]"};
	char keys[][3][20] = {{"\nfile=", "\nlevel="},
	                     {"\nkeyfile=", "\ncertfile=", "\nallow_tlsv1_2="},
						 {"\nwebroot=", "\nport="}};

	config_file[read(config_fd, config_file, 1024)] = '\0';
	close(config_fd);

	current_char = config_file;
	char *tempchar;

	while(*current_char != '\0'){
		if(*current_char == '\t' || *current_char == ' '){
			tempchar = current_char;
			while(*(tempchar) != '\0'){
				*tempchar = *(tempchar+1);
				tempchar++;
			}
			current_char--;
		}
		current_char++;
	}
	

	for(int i = 0; i < 3; i++){
		current_char = strstr(config_file, sections[i]);
		if(current_char == NULL){
			continue;
		}
		current_char += strlen(sections[i]);
		tempchar = strchr(current_char, '[');
		if(tempchar == NULL){
			tempchar = config_file + strlen(config_file);	
		}
		strncpy(section, current_char, tempchar-current_char);
	//TODO endline can also be crlf, so check for cr also not just lf?
		for(int j = 0; j < 3; j++){
			current_char = strstr(section, keys[i][j]);
			if(current_char == NULL){ continue; }
			current_char += strlen(keys[i][j]);
			to_copy = strchr(current_char, '\n')-current_char;
			strncpy(value, current_char, to_copy);
			value[to_copy] = '\0';

			if(!strcmp(sections[i], "[logger]")){
				if(!strcmp(keys[i][j], "\nfile=")){
					strncpy(log_file, value, 1024);
				}
				else if(!strcmp(keys[i][j], "\nlevel=")){
					if(!strcmp(value, "DBG")){
						*log_level = LVL_DBG;
					} else if(!strcmp(value, "WARN")){
						*log_level = LVL_WARN;
					} else if(!strcmp(value, "CRIT")){
						*log_level = LVL_CRIT;
					} else{
						*log_level = LVL_INFO;
					}
				}
			}
			else if(!strcmp(sections[i], "[tls]")){
				if(!strcmp(keys[i][j], "\nkeyfile=")){
					strncpy(keyfile, value, 1024);	
				}
				else if(!strcmp(keys[i][j], "\ncertfile=")){
					strncpy(certfile, value, 1024);
				}
				else if(!strcmp(keys[i][j], "\nallow_tlsv1_2=")){
					if(!strcmp(value, "true")){
						*allow_tlsv1_2 = 1;
					} else if(!strcmp(value, "false")){
						*allow_tlsv1_2 = 0;
					}
				}
			}
			else if(!strcmp(sections[i], "[site]")){
				if(!strcmp(keys[i][j], "\nwebroot=")){
					strncpy(webroot, value, 1024);
				}
				else if(!strcmp(keys[i][j], "\nport=")){
					*port = strtol(value, NULL, 10);
				}
			}
		}
	}
	printf("\n");
	printf("Level: %i\nFile: %s\n", *log_level, log_file);
	printf("Certfile: \"%s\"\nKeyfile: \"%s\"\nAllow TLSv1_2: %i\n", certfile, keyfile, *allow_tlsv1_2);
	printf("Webroot: \"%s\"\nPort: %i\n", webroot, *port);
	return 0;
}

