#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <var.h>

/////////////////////////////////////////////////////
int IsPackageInAur(char* package) {

  struct sockaddr_in *remote;
  int sock, tmpres, reto=0, port=80;
  char *ip, buffer[BUFSIZ], *get, *string, full_pack_name[30];
  char buf[5000], host[50], tpl[100], *query, page[60];
  char *getpage = page, useragent[]="HTMLGET 1.0";
	
	strcpy(host,"aur.archlinux.org");
	strcpy(page,"rpc.php?type=search&arg=");
	strcat(page,package);

  if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
    perror("Can't create TCP socket. Internal error 3");
    exit(1);
  }

  ip = get_ip(2);
  remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
  remote->sin_family = AF_INET;
  tmpres = inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));
  remote->sin_port = htons(port);
  if(connect(sock, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0){
    perror("Could not connect to AUR. Please check your internet connection");
    exit(1);
  }

  strcpy(tpl,"GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n");
  query = (char *)malloc(strlen(host)+strlen(getpage)+strlen(useragent)+strlen(tpl)-5);

  sprintf(query, tpl, getpage, host, useragent);
  get = query;

  int sent = 0;
  while(sent < strlen(get))
  { 
    tmpres = send(sock, get+sent, strlen(get)-sent, 0);
    if(tmpres == -1){
      perror("Can't send query");
      exit(1);
    }
    sent += tmpres;
  }
  	memset(buf, 0, sizeof(buf));
  	char * htmlcontent;
	tmpres = recv(sock, buf, BUFSIZ, 0);
	htmlcontent = strstr(buf, "\r\n\r\n");
	htmlcontent += 4;
	strcpy(buffer,htmlcontent );
	if(tmpres < 0) perror("Error receiving data from AUR. Please check your internet connection");

  free(get);
  free(remote);
  free(ip);
////
	char bo[4];
	sprintf(bo,"%c",'"');
	strcpy(full_pack_name,bo);
	strcat(full_pack_name,package);
	sprintf(bo,"%c",'"');	
	strcat(full_pack_name,bo);
	string=strtok(buffer,",");
	while (buffer) {
		string=strtok(NULL,","); if (!string) break;
		char* pch = strstr(string,full_pack_name); 
		if (pch) return 1; // пакет находится в aur
	}
  	return 0; // пакета нет в аур
}
///////////////////////////////////////////////////////
char *get_ip(int i) {
   int iplen = 15; //XXX.XXX.XXX.XXX
    char *ip = (char *)malloc(iplen+1);
    memset(ip, 0, iplen+1);
    if (i==1) strcpy (ip,"173.236.246.175"); // ARM
	if (i==2) strcpy (ip,"208.92.232.29"); // Aur
	
    return ip;
}

char *build_get_query(char *host, char *page) {
  char *query;
  char *getpage = page;
  char useragent[]="HTMLGET 1.0";
  char *tpl;
	
  strcpy(tpl,"GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n");
  query = (char *)malloc(strlen(host)+strlen(getpage)+strlen(useragent)+strlen(tpl)-5);
  sprintf(query, tpl, getpage, host, useragent);
  return query;

}
///////////////////////////////////////////////////////
int ReadArm(char* package, struct arm_packs arm_packages[]) {
  struct sockaddr_in *remote;
  int sock, tmpres, port=80;
  char arch1[80], *architecture,  *ip, *get, buf[BUFSIZ+1];
  char host[40], page[80];
	
	if(sizeof(void*) == 4) { architecture = (char *)"32";  }
	else if (sizeof(void*) == 8) { architecture = (char *)"64"; }
	
	strcpy(host,"arm.konnichi.com");
	strcpy(page,"search/raw.php?a=");
	strcat(page,architecture);
	strcat(page,"&q=^");	
	strcat(page,package);	
	strcat(page,"%24&core=1&extra=1&community=1");

  if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    perror("Can't create TCP socket. Internal error 1");
    exit(1);
  }
  ip = get_ip(1);
  //strcpy (ip,"173.236.246.175");
  remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
  remote->sin_family = AF_INET;
  tmpres = inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));
	
  if( tmpres < 0) {
    perror("Can't set remote->sin_addr.s_addr. Internal error 2");
    exit(1);
  }
  else if(tmpres == 0) {
    fprintf(stderr, "%s is not a valid IP address\n", ip);
    exit(1);
  }
  remote->sin_port = htons(port);
  if(connect(sock, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0) {
    perror("Could not connect to ARM. Please check your internet connection\n");
    exit(1);
  }
	
  char *query2, *getpage2 = page;
  char tpl[50], useragent[]="HTMLGET 1.0";

  strcpy(tpl,"GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n");
  query2 = (char *)malloc(strlen(host)+strlen(getpage2)+strlen(useragent)+strlen(tpl)-5+100);
  sprintf(query2, tpl, getpage2, host, useragent);
  get = query2;


  int sent = 0;
  while(sent < strlen(get)) { 
    tmpres = send(sock, get+sent, strlen(get)-sent, 0);
    if(tmpres == -1) {
      perror("Can't send query to ARM");
      exit(1);
    }
    sent += tmpres;
  }
  memset(buf, 0, sizeof(buf));
  int htmlstart = 0;
  char  *htmlcontent, *string, *str, *pch, *first, *last;
char temp[100];
	
  while((tmpres = recv(sock, buf, BUFSIZ, 0)) > 0){
    if(htmlstart == 0) {
		  htmlcontent = strstr(buf, "\r\n\r\n");
		  if(htmlcontent != NULL) {
				htmlstart = 1;
				htmlcontent += 4;
		  }
    }
	else {
		htmlcontent = buf;
   }
  }
  if(tmpres < 0) {
    perror("Error receiving data from ARM. Please check your internet connection");
  }
  free(get);
  free(remote);
  free(ip);

if (!strlen(htmlcontent)) {
	return 1;
}
// Обрабатываем данные из арм
else {
	int i=0;
	str = strtok(htmlcontent, "\n");
	strcpy(arm_packages[i].full_path,str);
	while(str = strtok(NULL, "\n")) {
		if (!str) break;
		i++;
		last = &str[strlen(str)-3];
		if (strcmp(last,"sig")) {
			strcpy(arm_packages[i].full_path,str);
		}
	}
}
return 0;
}
