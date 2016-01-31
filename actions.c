int DowngradePackage( char *package) {
	
	int isincache = IsPackageInLogs(package); // Here also parsing pacman.log and using flag actions.package_never_upgraded
	if (isincache) {
		if (!silent) { sprintf(tmp_string, "Downgrading from Cache, to version %s\n",install_version); dgr_output(tmp_string); }
		system(install_command);
		return 0;
	}
	strcpy(install_version,installed_pkg_ver);
	ret = IsPackageInArm(package, install_version);
	if (arm_pkgs[ret].link) {
		if (!silent) { sprintf(tmp_string, "Downgrade %s from ARM to version %s\n", package,arm_pkgs[ret+2].version); dgr_output(tmp_string); }
		strcpy(install_command,"sudo pacman -U "); strcat(install_command,arm_pkgs[ret+2].link);
		system(install_command);
		return 0;
	}
return 1;
}
char* GetChoiseForPackage( char *package) {

	int showpkgs = PrepareView(package); // Готовим массив со списком пакетов для отображения пользователю
	
	for (int i=1;i<=MAX_PKGS_FOR_USER && i<=showpkgs;i++) {
		sprintf(tmp_string, "%d: %s-%s %s\n", i, user_pkgs[i].name, user_pkgs[i].version, user_pkgs[i].repo); 
		dgr_output(tmp_string);
	}
	
	dgr_output (">> Please enter package number, [q] to quit ");
	scanf ("%s",tmp_string);
	return tmp_string;
}
int IsPackageAvailable(char *package) {
	alpm_siglevel_t siglevel=0;

	db = alpm_register_syncdb(alpm_handle, "core", siglevel);
	pkg = alpm_db_get_pkg(db,(const char*)package);
	const char *pkgr= alpm_pkg_get_url(pkg);
	if (pkgr) { int i = IsPackageInstalled(package); if (i==1) {  installed_pkg_ver = alpm_pkg_get_version(pkg);  return 0; } else return 1; }
	db = alpm_register_syncdb(alpm_handle, "extra", siglevel);
	pkg = alpm_db_get_pkg(db,(const char*)package);
	pkgr= alpm_pkg_get_url(pkg);
	if (pkgr) { int i = IsPackageInstalled(package); if (i==1) { installed_pkg_ver = alpm_pkg_get_version(pkg); return 0; } else return 1; }
	db = alpm_register_syncdb(alpm_handle, "community", siglevel);
	pkg = alpm_db_get_pkg(db,(const char*)package);
	pkgr= alpm_pkg_get_url(pkg);
	if (pkgr) { int i = IsPackageInstalled(package); if (i==1) { installed_pkg_ver = alpm_pkg_get_version(pkg); return 0; } else return 1; }
	return 2;
	
// return 0 - pkg available
// return 1 - pkg not installed
// return 2 - wrong pkg name
}

int IsPackageInstalled(char *package) {
	
	db = alpm_get_localdb(alpm_handle);
 	pkg = alpm_db_get_pkg(db,(const char*)package);
    if (alpm_pkg_get_name(pkg)) return 1;
    else return 0;
}

int PrepareView(char *package) {
	int cntr=0;
	
	if (WITH_ALA && pkgsinala) { // Создаем список пакетов для вывода по ALA
		user_pkgs = realloc(ala_pkgs, pkgsinala*sizeof(struct user_packs));
		while (pkgsinala) {
			strcpy(user_pkgs[cntr].name,ala_pkgs[pkgsinala].name);
			strcpy(user_pkgs[cntr].version,ala_pkgs[pkgsinala].version);
			
			sprintf(tmp_string,"%s-%s", user_pkgs[cntr].name, user_pkgs[cntr].version);
			ret = IsPackageInCache(tmp_string);			
			if (!strcmp(user_pkgs[cntr].version, installed_pkg_ver)) {
				if (ret==1) strcpy(user_pkgs[cntr].link,full_path_to_packet);
				else strcpy(user_pkgs[cntr].link,ala_pkgs[pkgsinala].full_path);
				strcpy(user_pkgs[cntr].repo," [installed]");
			}
			else if (ret==1) {
				strcpy(user_pkgs[cntr].link,full_path_to_packet);
				strcpy(user_pkgs[cntr].repo," (from cache)");
			}
			else {
				strcpy(user_pkgs[cntr].link,ala_pkgs[pkgsinala].full_path);
				strcpy(user_pkgs[cntr].repo," (from ALA)");
			}
			cntr++;
			pkgsinala--;
		}
	}
	else if 	(WITH_ARM && pkgsinarm) { // Создаем список пакетов для вывода по ARM
		user_pkgs = realloc(ala_pkgs, pkgsinarm*sizeof(struct user_packs));
		while (pkgsinarm) {
			strcpy(user_pkgs[cntr].name,arm_pkgs[cntr].name);
			strcpy(user_pkgs[cntr].version,arm_pkgs[cntr].version);
			
			sprintf(tmp_string,"%s-%s", user_pkgs[cntr].name, user_pkgs[cntr].version);
			ret = IsPackageInCache(tmp_string);
			if (!strcmp(user_pkgs[cntr].version, installed_pkg_ver)) {
				if (ret==1) strcpy(user_pkgs[cntr].link,full_path_to_packet);
				else strcpy(user_pkgs[cntr].link,ala_pkgs[cntr].full_path);
				strcpy(user_pkgs[cntr].repo," [installed]");
			}
			else if (ret==1) {
				strcpy(user_pkgs[cntr].link,full_path_to_packet);
				strcpy(user_pkgs[cntr].repo," (from cache)");
			}
			else {
				strcpy(user_pkgs[cntr].link,arm_pkgs[cntr].full_path);
				strcpy(user_pkgs[cntr].repo," (from ARM)");
			}
			cntr++;
		}		
	}
return cntr-1;
}

int IsPackageInLogs(char *package) {

	for (;pacmanlog_length>0;pacmanlog_length--) {
		if (!strcmp(package,pkgs[pacmanlog_length].name) && !strcmp("upgraded",pkgs[pacmanlog_length].action)) { // found necessary package
			if (strcmp(pkgs[pacmanlog_length].cur_version, pkgs[pacmanlog_length].prev_version)) { // if the same version - search next
				strcpy (full_path_to_packet,"/var/cache/pacman/pkg/");
				strcat (full_path_to_packet,package);
				strcat (full_path_to_packet,"-");
				strcat (full_path_to_packet,pkgs[pacmanlog_length].prev_version);
				strcat (full_path_to_packet,"-");
				strcat (full_path_to_packet,architecture);
				strcat (full_path_to_packet,".pkg.tar.xz");
				//printf("%s\n",full_path_to_packet); //DEBUG
				break; // Package upgraded at least 1 time
			}
		}
	}
	//printf("1: %s\n",full_path_to_packet); //DEBUG
	strcpy(install_version,pkgs[pacmanlog_length].prev_version);
	if(access(full_path_to_packet, F_OK) != -1) { // previously version available in cache
		strcpy(tmp_string,"sudo pacman -U "); // install
		strcat(tmp_string,full_path_to_packet);
		strcpy(install_command,tmp_string);
		//printf("install_command: %s\n",install_command); //DEBUG
		return 1;
	}
	else return 0;
}
int IsPackageInCache(char *package) {

	sprintf(full_path_to_packet,"/var/cache/pacman/pkg/%s-%s.pkg.tar.xz", package, architecture);
	//printf("%s\n",full_path_to_packet); //DEBUG
	if(access(full_path_to_packet, F_OK) != -1) { return 1; }// package in cache
	else return 0;
}

size_t curl_handler(char *data, size_t size, size_t nmemb, void *userp) {

	size_t realsize = size * nmemb;
	struct curl_MemoryStruct *mem = (struct curl_MemoryStruct *)userp;
	mem->memory = realloc(mem->memory, mem->size + realsize + 1);
	memcpy(&(mem->memory[mem->size]), data, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;
	return realsize;
}
int IsPackageInAur(char *package) {
	
	chunk.memory = malloc(1);
	chunk.size = 0;
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	sprintf(tmp_string,"https://aur.archlinux.org/rpc.php?type=search&arg=%s",package);
	curl_easy_setopt(curl, CURLOPT_URL, tmp_string);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_handler);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	result = curl_easy_perform(curl);
	if(result != CURLE_OK) {
		dgr_output ("Please check you internet connection. AUR reading error\n");
		return -1; // Exit with error
	}	
	//// Parsing AUR response
	cJSON *root = cJSON_Parse(chunk.memory);
	cJSON *item = cJSON_GetObjectItem(root,"results");
	for (int i=0;i<cJSON_GetArraySize(item);i++) {
		cJSON *subitem=cJSON_GetArrayItem(item,i);
		str= cJSON_GetObjectItem(subitem,"Name")->valuestring;
		if (!strcmp(str,package)) return 1; // package in AUR
	}
	cJSON_Delete(root);
	curl_easy_cleanup(curl);
	curl_global_cleanup();	
	if(chunk.memory) free(chunk.memory);
  	return 0; // package not in AUR
}

void ReadPacmanLog() {
	char *buff = NULL;
	size_t len;
	char *date, *time, *operat, *pack_name, *cur_version, *prev_version, *fake;
	int i=0;

	loglines_counter=0;
	pFile=fopen("/var/log/pacman.log","r");
	while (!feof(pFile)) {  // Count lines q-ty in pacman.log
		getline(&buff, &len, pFile);
		loglines_counter++;
	}
	rewind(pFile);
	pkgs = calloc(1, sizeof(struct packs));
	pkgs = realloc(pkgs, loglines_counter * sizeof(struct packs));

	loglines_counter=0;
	while (!feof(pFile)) {  // Count lines q-ty in pacman.log
		getline(&buff, &len, pFile);
		date = strtok(buff," ");
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
			strcpy(pkgs[loglines_counter].date,date);
			strcpy(pkgs[loglines_counter].time,time);
			strcpy(pkgs[loglines_counter].name,pack_name);
			strcpy(pkgs[loglines_counter].action,operat);
			strcpy(pkgs[loglines_counter].cur_version,cur_version);
			strcpy(pkgs[loglines_counter].prev_version,prev_version);
			loglines_counter++;
			//printf ("date: %s, time: %s, operat: %s, pack_name: %s\n", date, time, operat, pack_name); //DEBUG
		}
		i++;
	}
	fclose(pFile);
	pacmanlog_length =loglines_counter;
}

int dgr_output(char *string) {
	printf("%s",string);
	
	return 0;
}

int Initialization(char *package) {

	openlog("downgrader", LOG_PID|LOG_CONS, LOG_USER);
    alpm_handle = NULL;
    alpm_handle = alpm_initialize("/","/var/lib/pacman/",0);
    if(!alpm_handle) { dgr_output("Libalpm initialize error!\n"); return 1; }
    db = alpm_get_localdb(alpm_handle);
 	pkg = alpm_db_get_pkg(db,(const char*)package);
    pkgname = alpm_pkg_get_name(pkg);

	if(sizeof(void*) == 4) architecture = (char *)"i686"; // architecture of machine
	else if (sizeof(void*) == 8) architecture = (char *)"x86_64";
	user_pkgs = calloc(1, sizeof(struct user_packs));
	
	ret = IsPackageAvailable(package);
	if (ret==2) { // Wrong package name
		int ret1 = IsPackageInAur(package); // check AUR
		if (ret1>0) { if(!silent) { sprintf(tmp_string, "Package '%s' in AUR. Downgrade impossible.\n", package); dgr_output(tmp_string); return 1; } }
		else { if(!silent) { sprintf(tmp_string, "Package '%s' not available. Please check package name\n", package); dgr_output(tmp_string); return 1; } }
	}
	else if (ret==1) { if(!silent) { sprintf(tmp_string, "Package '%s' not installed.\n", package); dgr_output(tmp_string); return 1; } }
	
    ReadPacmanLog();
    
	if (WITH_ALA) {
		ala_pkgs = calloc(1, sizeof(struct ala_packs));
		pkgsinala = ReadALA(package);
		if (!pkgsinala) dgr_output ("No packages in ALA! Disable\n");
	}	
	if (WITH_ARM) {
		arm_pkgs = calloc(1, sizeof(struct arm_packs));
		pkgsinarm = ReadArm(package);
		if (!pkgsinarm) dgr_output ("No packages in ARM! Disable\n");
	}
	if (!pkgsinala && !pkgsinarm) { dgr_output ("No source for packages. Terminating\n"); return 1; }
    return 0;
}

int Deinitialization() {
	alpm_release(alpm_handle);
	free (pkgs);
	free (user_pkgs);
	if (pkgsinarm) free (arm_pkgs);
	if (pkgsinala) free (ala_pkgs);
	closelog();
	return 0;
}
