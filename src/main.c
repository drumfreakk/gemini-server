#include <stdio.h>
#include <tls.h>

int main(){
	printf("%i\n", tls_init());
	struct tls_config* config = tls_config_new();
	tls_config_set_protocols(config, TLS_PROTOCOL_TLSv1_2);
	printf("Hello World!\n");
	return 0;
}
