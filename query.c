#include <string.h>
char* get_qs(char* buf, int l){
	char *m, *qs = NULL;
	if((m = getenv("REQUEST_METHOD")) && strcasecmp(m, "POST") == 0)
		qs = fgets(buf, l, stdin);
	else if( (qs = getenv("QUERY_STRING")) )
		qs = strncpy(buf, qs, l);
	if(qs && strlen(qs) < 1) qs = NULL;
	return qs;
}

int get_qs_param(char *qs, char *param, char *meaning, int l){
	char *tok, *val, *par, str[l+1];
	int stat = 0;
	strncpy(str, qs, l);
	tok = strtok(str, "&,;|\n");
	do{
		if((val = strchr(tok, '=')) == NULL) continue;
		*val++ = '\0';
		par = tok;
		if(strcasecmp(par, param)==0){
			stat = 1;
			strncpy(meaning, val, l);
			break;
		}
	}while((tok = strtok(NULL, "&,;|\n"))!=NULL);
	return stat;
}

/*
int list_qs_params(char *qs, char **params, int l, int n){
	char *tok, *val, *par, str[l+1];
	int num = 0, i = 0;
	strncpy(str, qs, l);
	tok = strtok(str, "&,; |\n");
	do{
		if((val = strchr(tok, '=')) == NULL) continue;
		*val++ = '\0';
		num++;
		par = tok;
		if(i++ < n)
			strncpy(params[i], val, l);
		else break;
	}while((tok = strtok(NULL, "&,; |\n"))!=NULL);
	return num;	
}
*/
