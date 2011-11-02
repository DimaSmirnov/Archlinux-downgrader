class Actions {
	public:
	int PacmanInit();
	int PacmanDeinit();
	int IsPackageInstalled(char *);
	int IsPackageInCache(char *);
	int IsPackageInAur(char *);
	int IsPackageInArm();
	void ReadPacmanLog();
	int ReadArm(char *);
	char* IsPackageInArm(char *, char *);
	char* ReverseString(char *);
	int DowngradePackage(char *, int);
	pmdb_t *db_local;
	alpm_list_t *i;
	alpm_list_t *dbnames;
	pmpkg_t *pkg;
	char *dbpath, *tmpchar;
	char install_command[200]; // Клманда для установки
	char install_version[30]; // Версия пакета для установки
	const char *installed_pac_ver;  // Текущая установленная версия
	long int pacmanlog_length;
	FILE *pFile, *pFile2;
	struct  packs{ // -- Действия с пакетами из логов пакмана
			char name[40]; // название пакета
			char date[10]; // дата операции
			char time[20]; // время операции
			char action[20]; // название операции (upgraded, installed, removed)
			char cur_version[20]; // предыдущая версия
			char prev_version[20]; // предыдущая версия
	}; packs  *packages;
	struct  arm_packs{ // -- Структура список пакетов в ARM
		char full_path[100]; // полный адрес до пакета
		char repository[30]; // Repositroy of package
		char version[20]; // Version of package
		char name[50]; // Name of package
	}; arm_packs  *arm_packages;
	int package_never_upgraded;
	int action_counter;
	int tmpint, debug;
};
//////////////////////////////////////////////////
int Actions::DowngradePackage(char *package, int quiet_downgrade) {
	tmpchar=NULL;
		if (!quiet_downgrade) printf ("\033[1;%dm Downgrade package: %s \033[0m \n", 31, package);

	    int isinstalled = Actions::IsPackageInstalled(package);
	    if (isinstalled) {
			if (!quiet_downgrade) printf("Installed version: %s\n",installed_pac_ver);
		}
		else {
			if(!quiet_downgrade) printf("Package %s not installed. Terminating\n", package); 
			return 1;			
		}
		
	    int isinaur = Actions::IsPackageInAur(package);
	    if (isinaur) { 
			if(!quiet_downgrade) printf("Package in AUR. Downgrade impossible. Terminating\n"); 
			return 1; 
		}
		
	    int isincache = Actions::IsPackageInCache(package); // Here also parsing pacman.log and using flag actions.package_never_upgraded
	    if (isincache) {
			if (!quiet_downgrade) printf("Downgrading from Cache, to version %s\n",install_version);
			system(install_command); // Organize a downgrading from cache
			return 0;
		}
		else { // ELSE Organize checking in  ARM
			int isarmavailable = Actions::ReadArm(package);
			if (!isarmavailable) tmpchar = Actions::IsPackageInArm(package, install_version);
			if (tmpchar) {
				if (!quiet_downgrade) printf("Downgrade %s from ARM to version %s\n", package,install_version);
				strcpy(install_command,"sudo pacman -U "); strcat(install_command,tmpchar);
				system(install_command);
				return 0;
			}
			free (arm_packages);
		}
		if (package_never_upgraded==1) printf ("Package %s never upgraded.You can try to remove this package.\n",package); //Package never upgrades, but installed
		else if (!isinstalled) printf ("Package %s not installed.Terminating.\n", package);
		else printf("Package %s not found in AUR, local cache or ARM. Terminating\n", package);
		return 1;
}
//////////////////////////////////////////////////
int Actions::IsPackageInstalled(char *package) {
	
	pkg = alpm_find_satisfier(alpm_db_get_pkgcache(db_local), package);
	if(!pkg) return 0;// пакет не найден в системе
	else {
		installed_pac_ver = alpm_pkg_get_version(pkg); // Вывод версии пакета
		return 1;
	}
}
//////////////////////////////////////////////////
int Actions::IsPackageInCache(char *package) {
	char *architecture, *string, *chop, syztem[100], full_pack_name[50], full_path_to_packet[200], pack_prev_ver[40];
	char full_pack_name2[50], full_path_to_packet2[200];
	if(sizeof(void*) == 4) architecture = (char *)"i686";
	else if (sizeof(void*) == 8) architecture = (char *)"x86_64";
	package_never_upgraded = 1;
	for (;pacmanlog_length>0;pacmanlog_length--) {
		if (!strcmp(package,packages[pacmanlog_length].name) && !strcmp("upgraded",packages[pacmanlog_length].action)) { // нашли нужный пакет для апгрейда
			if (strcmp(packages[pacmanlog_length].cur_version, packages[pacmanlog_length].prev_version)) { // если был апгрейд на ту же версию, то ищем дальше
					strcpy (full_path_to_packet,"/var/cache/pacman/pkg/");
					strcat (full_path_to_packet,package);
					strcat (full_path_to_packet,"-");				
					strcat (full_path_to_packet,packages[pacmanlog_length].prev_version);
                    strcat (full_path_to_packet,"-");
                    strcat (full_path_to_packet,architecture);

					strcpy (full_path_to_packet2,full_path_to_packet);

					strcat (full_path_to_packet,".pkg.tar.xz");
                    strcat (full_path_to_packet2,".pkg.tar.gz");
					package_never_upgraded = 0; // Package upgraded at least 1 time
					break;

			}
		}
	}
	//printf("\033[1;%dm(%s) \033[0m  \n", 31, packages[pacmanlog_length].prev_version);
	strcpy(install_version,packages[pacmanlog_length].prev_version);
	pFile=fopen(full_path_to_packet,"r");
	pFile2=fopen(full_path_to_packet2,"r");
	if (pFile) {  // предыдущая версия пакета существует в локалке
		strcpy(syztem,"sudo pacman -U "); // установка
		strcat(syztem,full_path_to_packet);		
		strcpy(install_command,syztem);
		fclose(pFile);
//		printf("%s\n",install_command);
		return 1;
	}
	else if (pFile2) {
        strcpy(syztem,"sudo pacman -U "); // установка
        strcat(syztem,full_path_to_packet2);
        strcpy(install_command,syztem);
        fclose(pFile2);
//		printf("%s\n",install_command);
        return 1;
	}
	else return 0;
}
//////////////////////////////////////////////////
int Actions::IsPackageInAur(char *package) {
	struct sockaddr_in *remote;
	int sock, port=80;
	char ip[20], tmp[4], full_pack_name[30];
	char buf[BUFSIZ], host[50], tpl[100], *query, page[80];
	char useragent[]="HTMLGET 1.0";
	char htmlcontent[100000];
	char *getpage = page;
	
	strcpy (ip,"208.92.232.29"); // Aur
	strcpy(host,"aur.archlinux.org");
	strcpy(page,"rpc.php?type=search&arg=");
	strcat(page,package);

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
	remote->sin_family = AF_INET;
	tmpint = inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));
	remote->sin_port = htons(port);
	connect(sock, (struct sockaddr *)remote, sizeof(struct sockaddr));
	strcpy(tpl,"GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n");
	query = (char *)malloc(strlen(host)+strlen(tpl)+strlen(getpage)+strlen(useragent));
	sprintf(query, tpl, getpage, host, useragent);
	getpage=query;
  	memset(buf, 0, sizeof(buf));
    tmpint = send(sock, query, strlen(query), 0);
	while((tmpint = recv(sock, buf, sizeof(buf), 0)) > 0){
		strcat(htmlcontent,buf);	
	}
	//printf("%d :: %d\n",sizeof(htmlcontent), sizeof(buf));
  free(query);
  free(remote);
  close(sock);
  
  
//// Parsing AUR response
	sprintf(tmp,"%c",'"');
	strcpy(full_pack_name,tmp);
	strcat(full_pack_name,package);
	sprintf(tmp,"%c",'"');	
	strcat(full_pack_name,tmp);
	tmpchar=strtok(htmlcontent,",");
	while (htmlcontent) {
		tmpchar=strtok(NULL,","); if (!tmpchar) break;
		char* pch = strstr(tmpchar,full_pack_name); 
		if (pch) return 1; // package in AUR
	}
  	return 0; // package not in AUR
}
///////////////////////////////////////////////////////
void Actions::ReadPacmanLog() {
	
	action_counter=0;
	char string[650];
	char *p, *chop, *date, *time, *operat, *pack_name, *cur_version, *prev_version;
	pFile=fopen("/var/log/pacman.log","r");
	while (!feof(pFile)) {  // Count lines q-ty in pacman.log
		chop = fgets(string,2,pFile); if (!chop) break;
		action_counter++;
	}
	rewind (pFile);
	packages = new packs[action_counter];
	action_counter=0;
	int i=0;
        while (!feof(pFile)) { // Parsing file pacman.log for upgrades history on this machine
			chop = fgets(string,650,pFile); if (!chop) break;
			// DEBUG: printf("Line: %d\n",i+1);
			date = strtok(string," ");
			date++;
			time = strtok(NULL,"] ");
			operat = strtok(NULL," ");
			pack_name = strtok(NULL," ");
			if (!strcmp(operat,"upgraded")) {
				//DEBUG: printf("Upgraded: %s, line: %d\n", pack_name, i+1);
				prev_version = strtok(NULL," ");
				prev_version++;
				cur_version = strtok(NULL," ");
				cur_version = strtok(NULL,")");				
				strcpy(packages[action_counter].date,date);
				strcpy(packages[action_counter].time,time);
				strcpy(packages[action_counter].name,pack_name);
				strcpy(packages[action_counter].action,operat);
				strcpy(packages[action_counter].cur_version,cur_version);
				strcpy(packages[action_counter].prev_version,prev_version);
				action_counter++;
			}
			i++;
      }
   fclose(pFile);
   pacmanlog_length = action_counter;
}
///////////////////////////////////////////////////////
int Actions::ReadArm(char *package) {
  struct sockaddr_in *remote;
  int sock, port=80;
  char *architecture,  ip[20], buf[10000];
  char host[50], *query, page[80], tpl[100];
  char useragent[]="HTMLGET 1.0";
  char  htmlcontent[10000], *str, *last;
  char *getpage = page;
    
	arm_packages = new arm_packs[action_counter];
	
	if(sizeof(void*) == 4) { architecture = (char *)"32";  }
	else if (sizeof(void*) == 8) { architecture = (char *)"64"; }
	
	strcpy (ip,"173.236.246.175"); // ARM
	strcpy(host,"arm.konnichi.com");
	sprintf (page,"search/raw.php?a=%s&q=^%s%24&core=1&extra=1&community=1",architecture,package);
	
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
	remote->sin_family = AF_INET;
	tmpint = inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));
	remote->sin_port = htons(port);
	connect(sock, (struct sockaddr *)remote, sizeof(struct sockaddr));
	strcpy(tpl,"GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n");

	query = (char *)malloc(strlen(host)+strlen(tpl)+strlen(getpage)+strlen(useragent));
	sprintf(query, tpl, getpage, host, useragent);
	getpage=query;
  	memset(buf, 0, sizeof(buf));
    tmpint = send(sock, getpage, strlen(getpage), 0);

	while((tmpint = recv(sock, buf, sizeof(buf), 0)) > 0){
		strcat(htmlcontent,buf);
		for (int i=0;sizeof(buf)>i;i++) { //Clear buffer for new data 
			buf[i]='\0';
		}	
	}

  free(query);
  free(remote);
  close(sock);
// Processing data from ARM
	int i=0;
	if (!strlen(htmlcontent)) return 1;

	str = strtok(htmlcontent, "\n");	
	if (strstr(str,"http://")) { 
		strcpy(arm_packages[i].full_path,str);
		i++;
	}
	while(str = strtok(NULL, "\n")) {
		if (!str) break;
		last = &str[strlen(str)-3];
		if (strcmp(last,"sig") && strstr(str,"http://")) { 
			strcpy(arm_packages[i].full_path,str);
			i++;
		}
	}
// Get info about packages in ARM: version, package name
	for (int l=0;strlen(arm_packages[l].full_path)>0;l++) {
	char full[50];
	char *fully = full;
	strcpy(full,arm_packages[l].full_path);
	fully = ReverseString(full);
	str = strtok(full, "-");
	char release[10];
	strcpy(release,strtok(NULL, "-")); // release
	char version[50];
	strcpy(version,strtok(NULL, "-")); // version
	strcat(release,"-");
	strcat(version,release);
	strcpy(release,version);
	fully = ReverseString(release);
	//printf("%s\n",release);
	strcpy(arm_packages[l].version,fully); // Copy version of package
	strcpy(version,strtok(NULL, "/"));
	fully = ReverseString(version);
	//printf("%s\n",version);
	strcpy(arm_packages[l].name,fully); // Copy name of package

}
return 0;
}
////////////////////////////////////////////////////////////////
char* Actions::IsPackageInArm(char *package, char *version) {
	int arm_flag=0;
	char temp_pack[100];
	strcpy(temp_pack,package);
	strcat(temp_pack,"-");
	strcat(temp_pack,version);
	for(tmpint=0;arm_packages[tmpint].full_path;tmpint++) {
		if (strstr(arm_packages[tmpint].full_path,temp_pack)) {
			arm_flag=1;
			break;
		}
	}
	if (arm_flag==1) return arm_packages[tmpint].full_path; 
	else return NULL;
}
////////////////////////////////////////////////////////////////
char* Actions::ReverseString(char *string) {
	char t;
	for (int i=0, j=strlen(string)-1;i<j;i++,--j) {  // Reverse string;
        t=string[i];
        string[i] = string[j];
        string[j]=t;
    }
    return string;
}
////////////////////////////////////////////////////////////////
int Actions::PacmanInit() {
	int ret;
	ret = alpm_initialize();
	if(ret != 0) return(ret);
	alpm_option_set_root("/root");
	ret = alpm_option_set_dbpath("/var/lib/pacman");
	if(ret != 0) return(ret);
	db_local = alpm_option_get_localdb();
	if(!db_local) return(1);
	
	ReadPacmanLog();
	
	return(0);	
}
int Actions::PacmanDeinit() {
	free (packages);
	alpm_release();
}
