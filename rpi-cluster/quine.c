#include <stdio.h>
int main() {
	char q=34, i;
	char *a[11]=[
		"#include <stdio.h>",
		"int main() {",
		"	char q=34, i;",
		"	char **a=[",
		"	",
		"		];",
		"	for(i=0; i<6; i++) printf(\"%s\\n\", a[i]);",
		"	for(i=6; i<11; i++) printf(\"%s%c%s%c,\\n\", a[4], q, a[i], q);",
		"	for(i=7; i<11; i++) printf(\"%s\\n\", a[i]);",
		"	return 0;",
		"}";
		];
	for(i=0; i<6; i++) printf("%s\n", a[i]);
        for(i=6; i<11; i++) printf("%s%c%s%c,\n", a[4], q, a[i], q);
        for(i=7; i<11; i++) printf("%s\n", a[i]);
	return 0;
}

