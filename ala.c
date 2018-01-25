int ReadALA(char *package) {
	char substr[2];
	char ala_path[250], tmp_string2[200];
	char *fullpkg, *ptr, *env_url;
	
	strcpy(tmp_string,package);
	strncpy(substr, package, 1);
	substr[1]='\0';
	env_url=getenv("DOWNGRADE_ALA_URL");
	if(env_url!=NULL) {
		sprintf (ala_path, "%s/%s/%s/",env_url,substr,tmp_string);
	} else {
		sprintf (ala_path, "https://archive.archlinux.org/packages/%s/%s/",substr,tmp_string);
	}
	strcpy(package,tmp_string);

	chunk.memory = malloc(1);
	chunk.size = 0;
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, ala_path);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_handler);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
	result = curl_easy_perform(curl);
	if(result != CURLE_OK) {
		dgr_output ("Please check you internet connection. ALA reading error\n");
		return -1; // Exit with error
	}		
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	pkginala=counter=0;
	
	str = chunk.memory;
	char *pch = strstr(str,tmp_string);
	while (pch!=NULL) { pkginala++;  pch = strstr(pch+3,tmp_string); }
	pkginala = pkginala/2;
	ala_pkgs = realloc(ala_pkgs, (pkginala+1)*sizeof(struct ala_packs));
	//sprintf(tmp_string2, "Packages in ALA: %d\n",pkginala); dgr_output(tmp_string2); // DEBUG
	if (pkginala<1) return 0;
	
	str = chunk.memory;
	str = strtok(str, "<");
	counter++;
	while(str = strtok(NULL, "<")) {	
		if (strstr(str,"href=") && strstr(str,tmp_string)) { //printf("%d: %s\n",counter, str); // DEBUG
			strtok(NULL, "="); fullpkg = strtok(NULL, ">");
			fullpkg++; fullpkg[strlen(fullpkg)-5]='\0'; //printf("%d: %s\n",counter, fullpkg); // DEBUG
			if (ptr=strstr(fullpkg,architecture)) {
				strcpy(ala_pkgs[counter].full_name,fullpkg); //printf("%d: ala_pkgs[counter].full_name = %s\n",counter, ala_pkgs[counter].full_name); // DEBUG
				sprintf(tmp_string2,"%s%s",ala_path, fullpkg);
				strcpy(ala_pkgs[counter].full_path,tmp_string2); //printf("%d: ala_pkgs[counter].full_path = %s\n",counter, ala_pkgs[counter].full_path); // DEBUG
				int a = strlen(package); strcpy(tmp_string2,fullpkg); tmp_string2[a]='\0';
				strcpy(ala_pkgs[counter].name,tmp_string2); //printf("%d: ala_pkgs[counter].name = %s\n",counter, ala_pkgs[counter].name); // DEBUG
				a=ptr-fullpkg; fullpkg[a-1] = '\0'; a = strlen(package); fullpkg = fullpkg+a+1;
				
				ptr = str_replace(fullpkg,"%3A",":"); // unescape symbol ":" for pkg "go" for example
				strcpy(ala_pkgs[counter].version, ptr); //printf("%d: ala_pkgs[counter].version = %s\n",counter, ala_pkgs[counter].version); // DEBUG

				counter++;
			}
		}
	}
return counter;
}

int IsPackageInAla(char* package, char *version) {
	int ala_flag=0;
	char t_pack[100];	
	
	sprintf(t_pack,"%s-%s",package,version);
	for(tmpint=0;strlen(ala_pkgs[tmpint].full_name)>0;tmpint++) {
		if (strstr(ala_pkgs[tmpint].full_name,t_pack)) {
			ala_flag=1; break;
		}
	}
	if (ala_flag==1) return tmpint; // Возвращает номер в массиве ALA
	else return 0; // или 0 если пакет не найден	
}	

