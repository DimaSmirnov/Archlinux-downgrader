class Actions {
	public:
	int PacmanInit();
	int PacmanDeinit();
	int IsPackageInstalled(char *);
	int IsPackageInCache(char *);
	int IsPackageInAur(char *);
	int CheckDowngradePossibility(char *);
	int IsPackageInArm();
	void ReadPacmanLog();
	int ReadArm(char *);
	char* IsPackageInArm(char *, char *);
	char* ReverseString(char *);
	int DowngradePackage(char *);
	int GetChoiseFromArm(char *);

	alpm_handle_t *alpm_handle;	
	alpm_db_t *db_local;
	alpm_pkg_t *pkg;
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
			char cur_version[50]; // предыдущая версия
			char prev_version[50]; // предыдущая версия
	}; packs  *packages;
	struct  arm_packs{ // -- Структура список пакетов в ARM
		char full_path[300]; // полный адрес до пакета
		//char repository[30]; // Repositroy of package
		char version[50]; // Version of package
		char name[50]; // Name of package
	}; arm_packs  *arm_packages;
	int package_never_upgraded;
	int action_counter;
	int packages_in_arm;
	int tmpint, debug, show_list, quiet_downgrade;
	char package_number[2];
	//char *pac_number = package_number;
	int def_pac;
	
};
//////////////////////////////////////////////////
int Actions::DowngradePackage(char *package) {
	tmpchar=NULL;
//		if (!quiet_downgrade) printf ("\033[1;%dm Downgrade package: %s \033[0m \n", 31, package);
/*
        int isinstalled = Actions::IsPackageInstalled(package);
        if (isinstalled) {
            if (!quiet_downgrade) printf("Installed version: %s\n",installed_pac_ver);
        }
        else {
            if(!quiet_downgrade) printf("Package %s not installed. Terminating\n", package);
            return 1;
        }
*/
	    int isincache = Actions::IsPackageInCache(package); // Here also parsing pacman.log and using flag actions.package_never_upgraded
	    if (isincache) {
			if (!quiet_downgrade) printf("Downgrading from Cache, to version %s\n",install_version);
			system(install_command); // Start downgrading from cache
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
		else printf("Package %s not found in AUR, local cache or ARM. Terminating\n", package);
		return 1;
}
//////////////////////////////////////////////////
int Actions::GetChoiseFromArm(char *package) {

		int pac_num;
		int ispacmaninit = Actions::PacmanInit();
	    if (ispacmaninit) { 
			if(!quiet_downgrade) printf("Pacman not initialized! Interrupted\n");
			return 1;
		}
		int ret = Actions::CheckDowngradePossibility(package);
		Actions::ReadPacmanLog();
		ret = Actions::ReadArm(package);
		ret = Actions::IsPackageInCache(package);
		if (!quiet_downgrade) {
			printf ("\033[1;%dm Downgrade package: %s \033[0m \n\n", 31, package);
		}
		for (int i=0;i<Actions::packages_in_arm;i++) {
			printf("%d: %s-%s", i+1, Actions::arm_packages[i].name,Actions::arm_packages[i].version);
			if (!strcmp(Actions::arm_packages[i].version,Actions::installed_pac_ver)) printf(" [installed]\n");
			if (!strcmp(Actions::arm_packages[i].version,Actions::install_version)) { printf(" [will be installed by default]\n"); Actions::def_pac=i+1; }
			else printf("\n");
		}
		printf ("Please enter package number, [q] to quit, [d] to install default package: ");
		scanf ("%s",Actions::package_number);

		return 0;
}
//////////////////////////////////////////////////
int Actions::IsPackageInstalled(char *package) {
    const char *local;

//  int a = alpm_db_get_valid(db_local); if (!a) printf ("Database is valid!\n"); else printf ("Database is INvalid!\n");
    pkg = alpm_db_get_pkg(db_local,package);
    local = alpm_pkg_get_name(pkg);
    if(!local) return 0;// пакет не найден в системе
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
	char htmlcontent[15000];
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
	//while((tmpint = recv(sock, buf, sizeof(buf), 0)) > 0){
		tmpint = recv(sock, buf, sizeof(buf), 0); // Принимаем первую часть ответа сервера, ее достаточно, чтобы понять есть ли пакет или нет
		strcat(htmlcontent,buf);	
	//}
	//printf("%d :: %d\n",sizeof(htmlcontent), sizeof(buf)); //DEBUG
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
			//printf("Line: %d\n",i+1); // DEBUG: 
			date = strtok(string," ");
			date++;
			time = strtok(NULL,"] ");
			operat = strtok(NULL," ");
			pack_name = strtok(NULL," ");
			if (!strcmp(operat,"upgraded")) {
				//printf("Upgraded: %s, line: %d\n", pack_name, i+1); //DEBUG: 
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
  char *architecture,  ip[20], buf[100000];
  char host[50], *query, page[80], tpl[100];
  char useragent[]="HTMLGET 1.0";
  char  htmlcontent[100000], *str, *last;
  char *getpage = page;
    
	arm_packages = new arm_packs[action_counter];
	
	if(sizeof(void*) == 4) { architecture = (char *)"32";  }
	else if (sizeof(void*) == 8) { architecture = (char *)"64"; }
	
	strcpy (ip,"173.236.246.175"); // ARM
	strcpy(host,"arm.konnichi.com");
	sprintf (page,"search/raw.php?a=%s&q=^%s%24&core=1&extra=1&community=1",architecture,package);
	//printf("%s",page); // DEBUG
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
		//printf("%s",buf); // DEBUG
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
	//printf("::: %d\n", strlen(htmlcontent)); //DEBUG
	if (!strlen(htmlcontent)) return 1;

	str = strtok(htmlcontent, "\n");	
	if (strstr(str,"http://")) { 
		strcpy(arm_packages[i].full_path,str);
		//printf("%s\n",arm_packages[i].full_path); // DEBUG
		i++;
	}
	while(str = strtok(NULL, "\n")) {
		if (!str) break;
		last = &str[strlen(str)-3];
		if (strcmp(last,"sig") && strstr(str,"http://")) { 
			strcpy(arm_packages[i].full_path,str);
			//printf("%s\n",arm_packages[i].full_path); // DEBUG
			i++;
		}
	}
// Get info about packages in ARM: version, package name
	int l=0;
	for (l=0;strlen(arm_packages[l].full_path)>0;l++) {
	char full[1000];
	char *fully = full;
	char release[10];
	char version[20];
	
	strcpy(full,arm_packages[l].full_path);
	fully = ReverseString(full);
	str = strtok(full, ".");
	str = strtok(NULL, ".");
	str = strtok(NULL, ".");
	str = strtok(NULL, "-");
	if (strstr(str,"46_68x") || strstr(str,"686i")) { // For stupid packages in ARM :(
		strcpy(release,strtok(NULL, "-")); // release
		strcpy(version,strtok(NULL, "-")); // version
		//strcpy(release,version); //printf("%s\n",release); // DEBUG
		fully = ReverseString(version); //printf("%s\n",fully); // DEBUG
		strcat(fully,"-");
		strcat(fully,release);		
		strcpy(arm_packages[l].version,fully); //printf("%s\n",version); // DEBUG// Copy version of package
		strcpy(version,strtok(NULL, "/"));
		fully = ReverseString(version);//printf("1: %s\n",fully); // DEBUG
		strcpy(arm_packages[l].name,fully); // Copy name of package
	}
	else {
		strcpy(release,str); // release
		strcpy(version,strtok(NULL, "-")); // version
		fully = ReverseString(version);
		strcat(fully,"-");
		strcat(fully,release);//printf("%s\n",fully); // DEBUG
		
		strcpy(arm_packages[l].version,fully); // Copy version of package
		strcpy(version,strtok(NULL, "/"));
		fully = ReverseString(version);//printf("2: %s\n",fully); // DEBUG	
		strcpy(arm_packages[l].name,fully); // Copy name of package				
	}
}
packages_in_arm = l;
return 0;
}
////////////////////////////////////////////////////////////////
char* Actions::IsPackageInArm(char *package, char *version) {
	int arm_flag=0;
	char temp_pack[100];
	strcpy(temp_pack,package);
	strcat(temp_pack,"-");
	strcat(temp_pack,version);
	for(tmpint=0;strlen(arm_packages[tmpint].full_path)>0;tmpint++) {
		//printf("%s\n",arm_packages[tmpint].full_path); // DEBUG
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
int Actions::CheckDowngradePossibility(char *package) {
	int isinstalled = Actions::IsPackageInstalled(package);
	if (!isinstalled) {
		if(!quiet_downgrade) printf("Package '%s' not installed.\n", package); 
		return 1;
	}
	int isinaur = Actions::IsPackageInAur(package);
	if (isinaur) {
		if(!quiet_downgrade) printf("Package '%s' in AUR. Downgrade impossible.\n", package);
		return 1;
	}
return 0;
}
////////////////////////////////////////////////////////////////
int Actions::PacmanInit() {

    alpm_handle = NULL;
    alpm_handle = alpm_initialize("/","/var/lib/pacman/",0);
    if(!alpm_handle) {
        printf("Libalpm initialize error!\n");
        return 1;
    }
    db_local = alpm_option_get_localdb(alpm_handle);
    if(!db_local) {
        printf("Databse error!\n");
        return 1;
    }
    ReadPacmanLog();
    return 0;
}
int Actions::PacmanDeinit() {
	free (packages);
	alpm_release(alpm_handle);
}
