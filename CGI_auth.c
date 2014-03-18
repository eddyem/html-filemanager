#define _XOPEN_SOURCE 501
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sqlite3.h>
// в приложении, которое должно работать с аутентификацией, добавить
// extern int get_auth_level();

#define		NO_QS			0
#define		NO_LOGIN		-1
#define		NO_PASSWD		-2
#define		WRONG_PASSWD	-3
#define		BAD_URL			-4
#define		BAD_IP			-5
#define		NO_DB			-6
#define		NO_REG			-7
#define		BAD_SERV		-8

static void die(int error){
	fprintf(stderr, "Auth error %d\n", error);
	exit(error);
}

static char *db_path = "/var/www/SSL/cgi-bin/user-pass";

static char* get_key(){
	char *tok, *val, *par, *ret = NULL, *cookie;
	cookie = getenv("HTTP_COOKIE");
	if(!cookie) return NULL;
	tok = strtok(cookie, "; \n");
	do{
		if((val = strchr(tok, '=')) == NULL) continue;
		*val++ = '\0';
		par = tok;
		if(strcasecmp(par, "key")==0){
			if( strlen(val) > 0 )
				ret = strdup(val);
			break;
		}
	}while((tok = strtok(NULL, "; \n"))!=NULL);
	return ret;
}

static char *SQLbuf = NULL;
int callback(void *NotUsed, int argc, char **argv, char **azColName){
	if(!*argv) return 1;
	SQLbuf = strdup(*argv);
	return 0;
}

static char *SQLexec(char *par, char *key){
	sqlite3 *db;
	char *zErrMsg = NULL, *ret = NULL;
	char *q_str = (char*)calloc(512, 1);
	int rc = sqlite3_open(db_path, &db);
	if(rc){ sqlite3_close(db); die(NO_DB);}
	if(SQLbuf){ free(SQLbuf); SQLbuf = NULL;}
	snprintf(q_str, 512, "select %s from keyid where key=\"%s\"", par, key);
	rc = sqlite3_exec(db, q_str, callback, NULL, &zErrMsg);
	if(rc)
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
	else if(SQLbuf)
		ret = strdup(SQLbuf);
	free(q_str);
	sqlite3_close(db);
	return ret;
}

static char *get_id(char *key, int *level){
	char *ret=NULL, *tmp;
	ret = SQLexec("id", key);
	if( !(tmp = SQLexec("level", key)) ) *level = 0;
	else *level = atoi(tmp);
	free(tmp);
	return ret;
}

static char *sha(char *str, char *salt){
	char *tmp = crypt(str, salt);
	return strdup(tmp);
}

int get_auth_level(){
	char *IP, *URL, *id, *id_ori, *key, *salt;
	int level;
	salt = calloc(128, 1);
	IP = getenv("REMOTE_ADDR");
	if(!IP) die(BAD_IP);
	URL = getenv("HTTP_REFERER");
	if(!URL) die(BAD_URL);
	key = get_key();
	if(!key) die(BAD_SERV);
	snprintf(salt, 128, "$5$%s$", IP);
	id = sha(URL, salt);
	id = strrchr(id, '$') + 1;
	id_ori = get_id(key, &level);
	if(!id_ori) die(NO_REG);
	if(strcmp(id, id_ori)) die(BAD_SERV);
	free(salt);
	return(level);
}

