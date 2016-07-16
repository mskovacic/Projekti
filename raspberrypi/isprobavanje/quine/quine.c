#include <stdio.h>
int main() {
	char q=34, n=10, i;
	const char *a[19];
	
	a[0]="#include <stdio.h>";
	a[1]="int main() {";
	a[2]="	char q=34, n=10, i;";
	a[3]="	const char *a[19];";
	a[4]="	";
	a[5]="	for(i=0; i<5; i++) printf(";
	a[6]="%s%c";
	a[7]=", a[i], n);";
	a[8]="	for(i=0; i<19; i++) printf(";
	a[9]="	a[%d]=%c%s%c;%c";
	a[10]=", i, q, a[i], q, n);";
	a[11]="	for(i=5; i<17; i=i+3) printf(";
	a[12]="%s%c%s%c%s%c";
	a[13]=", a[i], q, a[i+1], q, a[i+2], n);";
	a[14]="	for(i=17 ; i<19 ; i++) printf(";
	a[15]="%s%c";
	a[16]=", a[i], n);";
	a[17]="	return 0;";
	a[18]="}";
	for(i=0; i<5; i++) printf("%s%c", a[i], n);
        for(i=0; i<19; i++) printf("	a[%d]=%c%s%c;%c", i, q, a[i], q, n);
        for(i=5; i<17; i=i+3) printf("%s%c%s%c%s%c", a[i], q, a[i+1], q, a[i+2], n);
	for(i=17 ; i<19 ; i++) printf("%s%c", a[i], n);
	return 0;
}

