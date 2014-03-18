#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#define CHALLOC(x)	(char *)malloc(x)

char *tmp_file, *buf, *file, *file_dest, *str, *curDir, *str1;
int f_tmp = 0, f_file = 0, tail_length = 0, fsize = 0;

void printerr(char *err, int errno){
	printf("<script language=JavaScript>function notify(){parent.Loaded(\"%s!\");}</script></head><body onLoad=\"notify();\">\n%s</body></html>\n", err, err);
	if(f_tmp > 0){
		close(f_tmp);
	}
	if(f_file > 0 && errno){
		close(f_file);
		unlink(file_dest);
	}
	free(tmp_file); free(buf); free(file); free(file_dest); free(curDir);
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

int get_param(char *qs, char *param, char *meaning, int l){
	char *tok, *val, *par, str[2048];
	int stat = 0;
	strncpy(str, qs, l);
	if(par = strstr(str, param)){
		if(val = strchr(par, '=')){
			val+=2;
			stat = 1;
			par = strchr(val, '"');
			*par = 0;
			strncpy(meaning, val, l);
		}
	}
	return stat;
}

void remove_spaces(char *name){
	char *ptr = name;
	while(ptr = strchr(ptr, ' '))
		*ptr++ = '_';
}

int main(){
	int n, i;
	tmp_file=CHALLOC(512); file=CHALLOC(512); file_dest=CHALLOC(1024); curDir=CHALLOC(512);
	buf=CHALLOC(16384);
	printf("Content-type: text/html\n\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=koi8-r\">\n<html><head>\n");
	sprintf(tmp_file, "/var/www/tmp/newfile_XXXXXX");
	mkstemp(tmp_file);
	if( (f_tmp = open(tmp_file, O_RDWR|O_CREAT)) == -1)
		printerr("can't write temporary file", 1);
	chmod(tmp_file, 00666);
	unlink(tmp_file);
	n = read(0, buf, 16384);
	if(n <= 2)
		printerr("no query string", 10);
	if(!get_param(buf, "filename", file, 512))
		printerr("can't determine filename", 2);
	get_param(buf, "name", curDir, 512);
	str = curDir;
	unhexdump(str);
	if(strchr(file, ' ')) remove_spaces(file);
	str1 = strstr(str, "/Inc/");
	if(str1){
		str1 += 5;
		if(strlen(str1) > 1)
			sprintf(file_dest, "/var/www/tmp/Incoming/%s/%s", str1, file);
		else
			sprintf(file_dest, "/var/www/tmp/Incoming/%s", file);
	}
	else
		sprintf(file_dest, "/var/www/tmp/Incoming/%s", file);
	str = buf;
	for(i=0; i<4; i++){ // пропускаем первые четыре строки (заголовок)
		str = strstr(str, "\r");
		if(!str) break;
		if(i == 0) tail_length = str - buf + 6; // длина хвоста
		str++;
	}
	if(!str)
		printerr("stream header damaged", 3);
	str++; // пропускаем \n
	i = n - (str - buf); // длина оставшихся данных
	write(f_tmp, str, i);
	while(n = read(0, buf, 16384))
		write(f_tmp, buf, n);
	fsize = lseek(f_tmp, -tail_length, SEEK_END); // получаем искомый размер файла
	lseek(f_tmp, 0, SEEK_SET);
	if( (f_file = open(file_dest, O_RDWR|O_CREAT)) == -1)
		printerr("can't save file", 4);
	chmod(file_dest, 00666);
	while(n = read(f_tmp, buf, 16384)){
		if(n > fsize) n = fsize;
		write(f_file, buf, n);
		fsize -= n;
	}
	printerr("", 0);
}

