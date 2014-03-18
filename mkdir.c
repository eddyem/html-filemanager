#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "query.c"

void printerr(char *err, int errno){
	printf("<script language=JavaScript>function notify(){parent.Loaded(\"%s!\");}</script></head><body onLoad=\"notify();\">\n%s</body></html>\n", err, err);
	exit(errno);	
}

void unhexdump(char *inp){
	char tmp[512], *o_ptr = inp, *tok;
	unsigned char ch;
	unsigned int a;
	strncpy(tmp, inp, 512);
	tok = strtok(tmp, "%");
	do{
		sscanf(tok, "%x", &a);
		ch = a;
		*o_ptr++ = ch;
	}while(tok = strtok(NULL, "%"));
	*o_ptr = 0;
}

int main(){
	char curDir[512], *cd=curDir, newDir[512], *curtok, *newtok, RealDir[1024], buf[2048], *ptr;
	printf("Content-type: text/html\n\n<html><meta http-equiv=\"Content-Type\" content=\"text/html; charset=koi8-r\"><body>\n");
	if(!(ptr = get_qs(buf, 2048)))
		printerr("no query string", 1);
	if(!get_qs_param(ptr, "DIR", curDir, 512))
		printerr("no directory selected", 2);
	if(!get_qs_param(ptr, "NEWDIR", newDir, 512))
		printerr("no name of new directory", 3);
	unhexdump(newDir);
	if(!strstr(curDir, "/Inc"))
		unhexdump(curDir);
	ptr = strstr(curDir, "/Inc");
	if(ptr) cd = ptr + 4;
	ptr = strstr(curDir, "..");
	if(ptr) printerr("bad directory name", 4);
	curtok = strtok(cd, "/");
	newtok = strtok(newDir, "/");
	if(!newtok) printerr("no directory name", 5);
	if(!curtok) sprintf(RealDir, "/var/www/tmp/Incoming/%s", newtok);
	else sprintf(RealDir, "/var/www/tmp/Incoming/%s/%s", curtok, newtok);
	if(mkdir(RealDir, 0777) < 0) printerr("can't create directory", 7);
	chmod(RealDir, 0777);
	printerr("directory created", 0);
}
