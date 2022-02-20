#include <sys/mman.h>
//#include <sys/stat.h>
//#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define handle_error(msg) \
   do { perror(msg); exit(EXIT_FAILURE); } while (0)

void citaj_memoriju(int addr) {
	int s;
	s = write(STDOUT_FILENO,(char *) addr, 1000);
	if (s == -1) handle_error("write");
}	   
	   
int main(int argc, char *argv[]) {
	char *addr, buffer[100];
	int fd, i=0;

	addr = mmap(NULL, sizeof(char)*100000000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (addr == MAP_FAILED) handle_error("mmap");
	
	printf ("Stvorena memorija na lokaciji: %p\n\nUpiši neki tekst:", addr);
	memset(buffer, 0, 100);
	while (fgets(buffer, 100, stdin)!=NULL && i<10) {
	   strncpy(&addr[i*100], buffer, 100);
	   i++;
	   printf ("Upiši neki tekst:");
	   memset(buffer, 0, 100);
	}
	
	printf("\r                    \r");
	fflush(stdout);
   
	citaj_memoriju((int) addr);

	return 0;
}