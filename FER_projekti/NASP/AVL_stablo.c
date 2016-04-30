#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct cvor cvor;
struct cvor
{
	cvor *lijevi;
	cvor *desni;
	int FR;
};

int main(int argc, char** argv) {
	FILE *f;
	char buffer[100] = { 0 };

	if (argc != 2) return 1;
	f=fopen("file", "w");
	if (f == NULL) return 1;
	memset(buffer, 0, 100);
	while (fscanf(f, "%s ", buffer))
	{
		printf("%s", buffer);
		memset(buffer, 0, 100);
	}
	//system("pause");
	return 0;
}
