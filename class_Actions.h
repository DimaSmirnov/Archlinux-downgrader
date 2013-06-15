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
	char install_command[200]; // Команда для установки
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
		char version[50]; // Version of package
		char name[50]; // Name of package
	}; arm_packs  *arm_packages;
	int package_never_upgraded;
	int action_counter;
	int packages_in_arm;
	int tmpint, debug, show_list, quiet_downgrade;
	char package_number[2];
	int def_pac;

};
//////////////////////////////////////////////////
int Actions::DowngradePackage(char *package) {
	tmpchar=NULL;
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
		if (ret) return 1;
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
			//else printf("\n");
		}
		printf ("Please enter package number, [q] to quit, [d] to install default package: ");
		scanf ("%s",Actions::package_number);

		return 0;
}
//////////////////////////////////////////////////
int Actions::IsPackageInstalled(char *package) {
    const char *local;
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
		return 1;
	}
	else if (pFile2) {
        strcpy(syztem,"sudo pacman -U "); // установка
        strcat(syztem,full_path_to_packet2);
        strcpy(install_command,syztem);
        fclose(pFile2);
        return 1;
	}
	else return 0;
}
//////////////////////////////////////////////////

static int curl_handler(char *data, size_t size, size_t nmemb, string *buffer) {
	  int result = 0;
	  if (buffer != NULL) {
		buffer->append(data, size * nmemb);
		result = size * nmemb;
	  }
	  return result;
}

//////////////////////////////////////////////////
int Actions::IsPackageInAur(char *package) {

	char *name, query[300];
  	char conte[800000];
	const char *cont = conte;
    CURL *curl;
    CURLcode result;
	string content;

	curl = curl_easy_init();
	sprintf(query,"https://aur.archlinux.org/rpc.php?type=search&arg=%s",package);
	curl_easy_setopt(curl, CURLOPT_URL, query);
	curl_easy_setopt(curl, CURLOPT_HEADER, 0);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_handler);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);
	result = curl_easy_perform(curl);
	cont=content.c_str();

	//// Parsing AUR response
	cJSON *root = cJSON_Parse(cont);

	cJSON *item = cJSON_GetObjectItem(root,"results");
	for (int i=0;i<cJSON_GetArraySize(item);i++) {
		cJSON *subitem=cJSON_GetArrayItem(item,i);
		name = cJSON_GetObjectItem(subitem,"Name")->valuestring;
		if (!strcmp(name,package)) return 1; // package in AUR
	}
	cJSON_Delete(root);
	curl_easy_cleanup(curl);

  	return 0; // package not in AUR
}
///////////////////////////////////////////////////////
void Actions::ReadPacmanLog() {

	action_counter=0;
	char strr[3]; // Заменить на string нафиг, достало сегфолтится
	char *cstr, *p, *chop, *date, *time, *operat, *pack_name, *cur_version, *prev_version, *fake;
	int i=0;
	pFile=fopen("/var/log/pacman.log","r");
	while (!feof(pFile)) {  // Count lines q-ty in pacman.log
		chop = fgets(strr,2,pFile); if (!chop) break;
		action_counter++;
	}
	fclose(pFile);
	packages = new packs[action_counter];
	action_counter=0;

	string line;
	ifstream log ("/var/log/pacman.log");
	  if (log.is_open()) {
		while ( log.good()) { // Parsing file pacman.log
			getline (log,line);
			i++;
			//printf("Line: %d\n",i); // DEBUG:
			cstr = new char [line.size()+1];
			strcpy (cstr, line.c_str());
			date = strtok(cstr," ");
			date++;
			time = strtok(NULL,"] ");
			fake = strtok(NULL," ");
			operat = strtok(NULL," ");
			pack_name = strtok(NULL," ");

			//printf("Line: %d, operat: %s\n",i, operat); // DEBUG:
			if (!operat) continue;
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
				//printf ("date: %s, time: %s, operat: %s, pack_name: %s\n", date, time, operat, pack_name);
			}

			delete[] cstr;
		}
		log.close();
	  }
   pacmanlog_length = action_counter;
}
///////////////////////////////////////////////////////
int Actions::ReadArm(char *package) {

	char  *str, *last, *architecture, conte[8000000];
    CURL *curl;
    CURLcode result;
	string content;
	const char *cont = conte;
	int i=0;

	arm_packages = new arm_packs[10000]; //[action_counter];

	if(sizeof(void*) == 4) { architecture = (char *)"32";  }
	else if (sizeof(void*) == 8) { architecture = (char *)"64"; }

	curl = curl_easy_init();
	sprintf (conte,"http://arm.konnichi.com/search/raw.php?a=%s&q=^%s%24&core=1&extra=1&community=1",architecture,package);
	curl_easy_setopt(curl, CURLOPT_URL, conte);
	curl_easy_setopt(curl, CURLOPT_HEADER, 0);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_handler);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);
	result = curl_easy_perform(curl);
	cont=content.c_str();
	strcpy(conte,content.c_str());

// Processing data from ARM
	if (!strlen(cont)) return 1;
	str = strtok(conte, "\n");
	strcpy(arm_packages[i].full_path,str);
	//printf("arm_packages[i].full_path: %s\n",arm_packages[i].full_path); // DEBUG
	i++;
	while(str = strtok(NULL, "\n")) {
		if (!str) break;
		last = &str[strlen(str)-3];
		if (strcmp(last,"sig") && strstr(str,"http://")) {
			strcpy(arm_packages[i].full_path,str);
			//printf("%s\n",arm_packages[i].full_path); // DEBUG
			i++;
		}
	}
	int l=0; // Get info about packages in ARM: version, package name
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
		fully = ReverseString(version);
		strcat(fully,"-");
		strcat(fully,release);
		strcpy(arm_packages[l].version,fully);
		strcpy(version,strtok(NULL, "/"));
		fully = ReverseString(version);
		strcpy(arm_packages[l].name,fully); // Copy name of package
	}

	else {
		strcpy(release,str); // release
		strcpy(version,strtok(NULL, "-")); // version
		fully = ReverseString(version);
		strcat(fully,"-");
		strcat(fully,release);

		strcpy(arm_packages[l].version,fully); // Copy version of package
		strcpy(version,strtok(NULL, "/"));
		fully = ReverseString(version);
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
    db_local = alpm_get_localdb(alpm_handle);
    //if(!db_local) {
    //    printf("Databse error!\n");
    //    return 1;
    //}
    ReadPacmanLog();
    return 0;
}
int Actions::PacmanDeinit() {
	free (packages);
	alpm_release(alpm_handle);
}
