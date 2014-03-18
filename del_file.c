#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

extern int get_auth_level();

void printerr(char *err, int errno){
	printf("<script language=JavaScript>function notify(){parent.Loaded(\"%s\");}</script></head><body onLoad=\"notify();\">\n</body></html>\n", err);
	if(errno) exit(errno);	
}

void unhexdump(char *inp){
	char *tmp, *o_ptr = inp, *tok;
	unsigned char ch;
	unsigned int a;
	tmp = (char*)malloc(16384);
	strncpy(tmp, inp, 16384);
	tok = strtok(tmp, "%");
	do{
		sscanf(tok, "%x", &a);
		ch = a;
		*o_ptr++ = ch;
	}while(tok = strtok(NULL, "%"));
	*o_ptr = 0;
}

int main(){
	char File[512], *buf, *qs, *ptr, *ptr1, *tok, *val, name[1024];
	int n, i;
	struct stat st;
	buf = (char*)malloc(16384);
	printf("Content-type: text/html\n\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=koi8-r\">\n<html><head>\n");
	qs = fgets(buf, 16384, stdin);
	if(!qs || (strlen(qs) < 1))
		printerr("no query string", 1);
	if(get_auth_level() < 3) printerr("need authorisation", 1);
	printerr("Deleting files...", 0);
	printf("delete files<br>\n");
	unhexdump(qs);
	tok = strtok(qs, ";");
	do{
		strncpy(name, tok, 1024);
		ptr = strstr(name, "/Inc");
		if(ptr) ptr += 4;
		else continue;
		ptr1 = strstr(ptr, "..");
		if(ptr1) continue;
		sprintf(File, "/var/www/tmp/Incoming%s", ptr);
		if(stat(File, &st) == 0){ // файл/директория существует
			if(S_ISREG(st.st_mode)){ // файл
				if(unlink(File) == 0) printf("file %s deleted<br>\n", name);
				else printf("can't delete %s<br>\n", name);
			}
			else if(S_ISDIR(st.st_mode)){ // директория
				if(rmdir(File) == 0) printf("dir %s deleted<br>\n", name);
				else printf("can't delete %s (maybe not empty)<br>\n", name);
			}
		}
		else printf("can't stat %s<br>\n", name);
	}while(tok = strtok(NULL, ";"));
	printerr("deleted", 0);
}
