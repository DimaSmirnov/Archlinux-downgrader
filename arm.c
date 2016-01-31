int ReadArm(char *package) {
	int counter;
	
	chunk.memory = malloc(1);
	chunk.size = 0;
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	sprintf (tmp_string,"http://repo-arm.archlinuxcn.org/search?arch=%s&pkgname=%s",architecture,package);
	curl_easy_setopt(curl, CURLOPT_URL, tmp_string);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_handler);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
	result = curl_easy_perform(curl);
	if(result != CURLE_OK) {
		dgr_output ("Please check you internet connection. ARM reading error\n");
		return -1; // Exit with error
	}		
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	counter=0;
	str = chunk.memory;
	
	char *pch = strchr(str,'\n');
	while (pch!=NULL) { counter++;  pch=strchr(pch+1,'\n'); }
	//sprintf(tmp_string, "1. Packages in ARM: %d (with testing)\n",counter); dgr_output(tmp_string); // DEBUG
	arm_pkgs = realloc(arm_pkgs, (counter+1)*sizeof(struct arm_packs));
	pkgsinarm = counter;
	if (!pkgsinarm) return 0;
	counter=0;
	str = chunk.memory;
	str = strtok(str, "\n");
	strcpy(arm_pkgs[counter].full_path,str);
	counter++;
	for (;str = strtok(NULL, "\n"); counter++) { 
		strcpy(arm_pkgs[counter].full_path,str); 
		//printf("%d: string: %s\n",counter, arm_pkgs[counter].full_path); // DEBUG
	}
	counter=0;
	int notest_cntr=1;
	while (counter<pkgsinarm) { // Get info about packages in ARM
		str = strtok(arm_pkgs[counter].full_path, "|");
		if (!strstr(str,"testing")) { // Exclude packages from `testing`
			//printf("%d: string: %s\n",notest_cntr, arm_pkgs[counter].full_path); // DEBUG
			strcpy(arm_pkgs[notest_cntr].repository,str); 
			str = strtok(NULL, "|");
			strcpy(arm_pkgs[notest_cntr].name,str);
			str = strtok(NULL, "|");
			str = strtok(NULL, "|");
			strcpy(arm_pkgs[notest_cntr].version,str);
			str = strtok(NULL, "|");
			strcpy(arm_pkgs[notest_cntr].link,str);
			str = strtok(NULL, "|");
			arm_pkgs[notest_cntr].pkg_release=atoi(str);
			if (notest_cntr==MAX_PKGS_FOR_USER) break;
			notest_cntr++;
		}
		counter++;
	}
	pkgsinarm = notest_cntr-1;
	//if(!quiet_downgrade) { sprintf(tmp_string, "2. Packages in ARM: %d (without testing)\n",pkgs_in_arm); dgr_output(tmp_string); } // DEBUG
	free(chunk.memory);

return pkgsinarm;
}
int IsPackageInArm( char *package, char *version) {
	int arm_flag=0;
	char t_pack[100];
	
	sprintf(t_pack,"%s-%s",package,version);
	for(tmpint=0;strlen(arm_pkgs[tmpint].full_path)>0;tmpint++) {
		if (strstr(arm_pkgs[tmpint].full_path,t_pack)) {
			arm_flag=1;
			break;
		}
	}
	if (arm_flag==1) return tmpint; // Возвращает номер в массиве arm
	else return 0; // или 0 если пакет не найден
}
