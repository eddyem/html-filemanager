#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "query.c"


#define	COL1	"<td><input type=checkbox></td>"
#define	COL3	"<td></td>"
#define	COL5	"<td></td>"


char buf[1024], buf1[1024], *qs;
char Dir[512], RealDir[512];
void printerr(char *err){
	printf("<h1>Error: %s!</h1></body></html>\n", err);
	exit(1);	
}

struct files{
	char *name;
	int size;
};

void back(char *Dir_, char *Ret){
	char *tok; char lasttok[512], Dir__[512];
	int flag = 0;
	strcpy(Dir__, Dir_);
	sprintf(Ret, "");
	sprintf(lasttok, "");
	tok = strtok(Dir__, "/");
	do{
		if(flag) sprintf(Ret, "%s/%s", Ret, lasttok);
		strncpy(lasttok, tok, 512);
		flag = 1;
	}while( (tok = strtok(NULL, "/")) != NULL );
	if(strcmp(Ret, "/") == 0)
		strcpy(Ret, "");
	//sprintf(Ret, "%s\0", Ret);
}

/*void basename(char *name, char *Ret){
	char *tok; char lasttok[512];
	tok = strtok(name, "/");
	Ret[0] = 0;
	lasttok[0] = 0;
	do strncpy(lasttok, tok, 512);
	while( (tok = strtok(NULL, "/")) != NULL );
	sprintf(Ret, "%s", lasttok);
}*/

/*void Descr(char *Dir, char *Fdescr){
}
Descr="descript.ion"
Name () # this function returns file description
{
    if [ ! -e "$DIR/$Descr" ]; then
	echo -e "$*\c"
	return
    fi
    name=`cat "$DIR/$Descr"|grep "^$*:"|awk -F: '{print $2}'`
    if [ "$name" != "" ]; then 
	echo -e "$name\c"
    else
	echo -e "$*\c"
    fi
}*/

static int cmpfiles(const void *p1, const void * p2){
	return strcmp(((struct files*)p1)->name, ((struct files*)p2)->name);
}

static int cmpstrings(const void *p1, const void *p2){
	return strcmp(* (char * const *) p1, * (char * const *) p2);
}

void hexdump(char *inp, char* outp, int flag){
	char *i_ptr = inp, *o_ptr = outp;
	unsigned char ch;
	strcpy(outp, "");
	while(*i_ptr){
		ch = *i_ptr++;
		//if(ch == ' ') ch = '_';
		if(ch == '/' && flag){ *o_ptr++ = '/';}
		else
		if(ch > 31 ){
			sprintf(o_ptr, "%%%x", ch);
			o_ptr += 3;
		}
	}
	*o_ptr = 0;
}

void unhexdump(char *inp){
	char tmp[512], *o_ptr = inp, *tok;
	unsigned char ch;
	int a;
	strncpy(tmp, inp, 512);
	tok = strtok(tmp, "%");
	do{
		sscanf(tok, "%x", &a);
		if(a > 31){
			ch = a;
			*o_ptr++ = ch;
		}
	}while(tok = strtok(NULL, "%"));
	*o_ptr = 0;
}

void linkfiles(){
	char name[1024], tmp[1024];
	struct files Filelist[1024];
	int cntr = 0, i;
	DIR *dir; struct dirent *de;
	struct stat st;
	if (!(dir = opendir (RealDir)))
		printerr("can't read directory");
	while ((de = readdir (dir)) != NULL){	     
		snprintf(tmp, 1024, "%s/%s", RealDir, de->d_name);
		stat(tmp, &st);
		if(!(S_ISREG(st.st_mode))) continue;
		Filelist[cntr].name = malloc(strlen(de->d_name) + 1);
		strcpy(Filelist[cntr].name, de->d_name);
		Filelist[cntr++].size = st.st_size;
	}
	qsort(Filelist, cntr, sizeof(struct files), cmpfiles);
	for(i = 0; i<cntr; i++){
		snprintf(tmp, 1024, "%s/%s", Dir, Filelist[i].name);
		hexdump(tmp, name, 0);
		printf("<tr><td><input type=checkbox name=\"file\" id=\"%s\"></td>\n", name);
		hexdump(tmp, name, 1);
		printf("<td align=right><div class=ralign onClick='get_file(\"%s\");'>\n", name);
		printf("%s</div></td>%s<td>%d</td>%s</tr>\n", Filelist[i].name, COL3, Filelist[i].size, COL5);
		free(Filelist[i].name);
	}
}

void linkdirs(){
	char name[512], *names[1024], tmp[512];
	int cntr = 0, i;	
	DIR *dir; struct dirent *de;
	struct stat st;
	if (!(dir = opendir (RealDir)))
		printerr("can't read directory");
	while ((de = readdir (dir)) != NULL){	   
		if(strcmp(de->d_name, "..") == 0 || strcmp(de->d_name, ".") == 0) continue;
		sprintf(name, "%s/%s", RealDir, de->d_name);
		stat(name, &st);
		if(!(S_ISDIR(st.st_mode))) continue;
		names[cntr] = malloc(strlen(de->d_name));
		strcpy(names[cntr++], de->d_name);
	}
	qsort(names, cntr, sizeof(char*), cmpstrings);
	for(i = 0; i<cntr; i++){
		if( !strstr(names[i], "_files") && !strstr(names[i], ".files") ){
			snprintf(tmp, 512, "%s/%s", Dir, names[i]);
			hexdump(tmp, name, 0);
			printf("<tr><td><input type=checkbox name=\"file\" id=\"%s\"></td>\n", name);
			printf("<td align=right><div class=boldstring onClick='get_content(\"%s\");'>\n", name);
			printf("%s</div></td>%s<td>директория</td>%s</tr>\n", names[i], COL3, COL5);
		
		}
	}
}

void linkback(char *Up){
	char name[512];
	if(strcmp(Up, "") == 0) return;
	hexdump(Up, name,0);
	printf("<tr>%s<td><div class=string onClick='get_content(\"%s\");'>../</div></td>%s<td>\n", COL1, name, COL3);
	printf("<div class=string onClick='get_content(\"%s\");'>назад</div></td>%s</tr>\n", name, COL5);
}

int main(){
	char *pDir = Dir, *ptr, Up[512];
	printf("Content-type: text/html\n\n<html><meta http-equiv=\"Content-Type\" content=\"text/html; charset=koi8-r\">\n<head>\n");
	printf("<title>Files viewer</title></head>\n<body>");
	if(!(qs = get_qs(buf, 1024)))
		printerr("no query string");
	if(!get_qs_param(qs, "Dir", pDir, 512))
		printerr("no directory selected");
	if(!strstr(pDir, "/Inc"))
		unhexdump(pDir);
	ptr = strstr(pDir, "/Inc");
	if(ptr) pDir = ptr + 4;
	ptr = strstr(pDir, ".."); // ищем уловки вроде /Inc/../../../../etc/passwd
	if(ptr) printerr("Bad filename");
	ptr = strstr(pDir, "/");
	if(!ptr || ptr == pDir) sprintf(RealDir, "/var/www/tmp/Incoming%s", pDir);
	else sprintf(RealDir, "/var/www/tmp/Incoming/%s", pDir);
	back(Dir, Up);
	printf("<table cols=5><thead><th width=20px><input type=checkbox name=\"all\" id=\"all\" onClick='select_all();'></th><th align=center>Имя</th><th width=10px></th>\n");
	printf("<th align=center>Размер (байт)</th><th width=10px></th></thead><tbody>\n");
	linkback(Up);
	linkdirs();
	linkfiles();
	linkback(Up);
	printf("</tbody></table></body></html>\n");
}

