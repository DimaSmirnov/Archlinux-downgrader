#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <var.h>
//#include <locale.h>

long int ReadPacmanLog(struct packs packages[], FILE  *pFile2) {

int action_counter=0;
char string[251];
char *p, *chop, *date, *time, *operat, *pack_name, *cur_version, *prev_version;

        while (!feof(pFile2)) {  // Читаем историю апгрейдов
				
                chop = fgets(string,250,pFile2); if (!chop) break;
				date = strtok(string," ");
				date++;
				time = strtok(NULL,"] ");
				operat = strtok(NULL," ");
				pack_name = strtok(NULL," ");

				if (!strcmp(operat,"upgraded")) {
								prev_version = strtok(NULL," ");
								prev_version++;
								cur_version = strtok(NULL," ");
								cur_version = strtok(NULL,")");				
								//printf("date: %s\n",date);
								strcpy(packages[action_counter].date,date);
								//printf("time: %s\n",time);
								strcpy(packages[action_counter].time,time);
								//printf("package: %s\n",pack_name);
								strcpy(packages[action_counter].name,pack_name);
								//printf("Action: %s\n",operat);
								strcpy(packages[action_counter].action,operat);
								//printf("Current version: %s\n",cur_version);
								strcpy(packages[action_counter].cur_version,cur_version);
								//printf("Prev version: %s\n",prev_version);
								strcpy(packages[action_counter].prev_version,prev_version);
								//printf("\n");
								action_counter++;
				}
				if (!strcmp(operat,"installed")) {
								cur_version = strtok(NULL,")");
								cur_version++;				
								//printf("date: %s\n",date);
								strcpy(packages[action_counter].date,date);
								//printf("time: %s\n",time);
								strcpy(packages[action_counter].time,time);
								//printf("package: %s\n",pack_name);
								strcpy(packages[action_counter].name,pack_name);
								//printf("Action: %s\n",operat);
								strcpy(packages[action_counter].action,operat);
								//printf("Current version: %s\n",cur_version);
								strcpy(packages[action_counter].cur_version,cur_version);
								//printf("\n");
								action_counter++;
				}
                 if (!strcmp(operat,"removed")) {
                              cur_version = strtok(NULL,")");
                              cur_version++;
                              //printf("date: %s\n",date);
                              strcpy(packages[action_counter].date,date);
                              //printf("time: %s\n",time);
                              strcpy(packages[action_counter].time,time);
                              //printf("package: %s\n",pack_name);
                              strcpy(packages[action_counter].name,pack_name);
                              //printf("Action: %s\n",operat);
							  strcpy(packages[action_counter].action,operat);
                              //printf("Current version: %s\n",cur_version);
                              strcpy(packages[action_counter].cur_version,cur_version);
                              //printf("\n");
                              action_counter++;
                 }
       }
   return action_counter;
}
/////////////////////////////////////////////
int DowngradeToDate (char *date) {
	printf ("Downgrade to date: %s",date);
}
/////////////////////////////////////////////
int DowngradeLastPackages (int pack_qty, long int actions_counter,  struct packs packages[]) {
	int i=0;
	printf ("Downgrade last %d packages\n", pack_qty);
	for (;actions_counter>0;actions_counter--) {
		if (!strcmp("upgraded",packages[actions_counter].action)) { // нашли нужный пакет для апргрейда
			if (strcmp(packages[actions_counter].cur_version, packages[actions_counter].prev_version)) { // если был апгрейд на другую версию, то ищем дальше
				int reso = IsPackageInAur(packages[actions_counter].name);
				if (reso==1) printf("Package %s located in AUR, downgrade cancelled\n",packages[actions_counter].name);
				else {
					SilentDowngradePackage(packages[actions_counter].name, actions_counter, &packages[0]);
					pack_qty--;
					if (pack_qty==0) break;
				}
				i++;
			}
		}
	}
}
/////////////////////////////////////////////
int SilentDowngradePackage (char* pack_name, long int actions_counter, struct packs packages[]) {
	char *architecture, ARMContent[30000], *string, *chop, syztem[100], full_pack_name[50], full_path_to_packet[100];
	long int actions = actions_counter;
	FILE *pFile;
	
	if(sizeof(void*) == 4) architecture = (char *)"i686";
	else if (sizeof(void*) == 8) architecture = (char *)"x86_64";

	printf("\n \033[1;%dm --> Downgrade package %s \033[0m", 31, pack_name);
	strcpy (full_pack_name,pack_name);
	strcat (full_pack_name,"-");

	for (;actions_counter>0;actions_counter--) {
		if (!strcmp(pack_name,packages[actions_counter].name) && !strcmp("upgraded",packages[actions_counter].action)) { // нашли нужный пакет для апгрейда
			if (strcmp(packages[actions_counter].cur_version, packages[actions_counter].prev_version)) { // если был апгрейд на ту же версию, то ищем дальше
				printf("\033[1;%dm(%s) \033[0m  \n", 31, packages[actions_counter].cur_version);				
				strcat (full_pack_name, packages[actions_counter].prev_version);
				strcat (full_pack_name,"-");
				strcat (full_pack_name,architecture);
				strcat (full_pack_name,".pkg.tar.xz");
				strcpy (full_path_to_packet,"/var/cache/pacman/pkg/");
				strcat (full_path_to_packet,full_pack_name);	
				pFile=fopen(full_path_to_packet,"r");
				if (pFile==NULL) { // в локалке пакет не найден
					ReadARM(pack_name, &ARMContent[0]); 
					char* pack_path = GetPrevPackageFromARM(pack_name, full_pack_name, &ARMContent[0]);
					if (pack_path) { // зато он есть в АРМ!
						printf("Downgrade %s from ARM\n", full_pack_name);						
						strcpy(syztem,"sudo pacman -U ");
						strcat(syztem, pack_path);
						system(syztem);
						return 1;
					}
				}
				else { // предыдущая версия пакета существует в локалке, кайф!
					printf("Downgrade %s from cache\n", full_pack_name);
					strcpy(syztem,"sudo pacman -U "); // установка
					strcat(syztem,full_path_to_packet);
					system(syztem);
					fclose(pFile);
					return 1;
				}
				break;
			}
		}
	}

	int result, res; // пакета нет в локалке и АРМ
	char *inp, *line = NULL;
	size_t len = 0;
	result = IsPackageInAur (pack_name); // а может он в аур ?
	if (result) { 
		printf("Package %s is in AUR, downgrade cancelled, delete this package from system ? [y/n] ", pack_name);
		while (getline (&line, &len, stdin) >= 0) {
			res = rpmatch (line);
			if (res >= 0) {
				if (res > 0) printf("sudo pacman -R %s\n", pack_name);
				break;
			}
		}
		free (line);
		return 1;
	}
	else { // значит откатываемый пакет был установлен, но не "прокачан" ни разу
		result = SearchPrevVersionInARM (pack_name, actions, &packages[0], &ARMContent[0]);
		if (result==1) { // в ARM предыдущей версии нет тоже =(   skype, например
			printf("Sorry, but previously version %s not found, delete this package from system ? [y/n] ", pack_name);
			while (getline (&line, &len, stdin) >= 0) {
				int res = rpmatch (line);
				if (res >= 0) {
					if (res > 0) printf("sudo pacman -R %s\n", pack_name);
					break;
				}
			}
			free (line);
			return 1;
		}
		else {
			printf("Package %s is new, and never downgrade, probably problem in this package.\n", pack_name);
			printf("Otherwise previously version possible in ARM. What to do ?\n", pack_name);
			printf("1 - delete package.  2 - downgrade package: ");
			int i=0;  char buv[50];
			scanf("%d",&i);
			if (i==1) {
				sprintf(buv, "sudo pacman -R %s", pack_name);
				printf("%s\n", buv);
			}
			//if (i==2) {  InstallPrevVersionFromARM(); }

		}
	}
	return 0;
}
/////////////////////////////////////////////
int PacmanStatistic (int actions_counter) {
	printf ("Статистика pacman:\n"); 
	printf ("Всего операций в логах: %d\n", actions_counter); 	
}
/////////////////////////////////////////////
long int GetPacmanSize(FILE  *pFile) {
	long int action_counter=0;
	char string[2], *chop;
    while (!feof(pFile)) {  // Читаем историю пакетов
        chop = fgets(string,2,pFile); if (!chop) break;
		action_counter++;
	}
	return action_counter;
}
/////////////////////////////////////////////
int VersionParser(char* version1, char* version2) {
	int a1,b1, ret; 
	int a = strlen(version1); 
	int b = strlen(version2);
	if (b>a) a=b;
	for (int i=0;i<a;i++) {
		a1 += version1[i];
		b1 += version2[i];
	}
	if (b1>a1) ret = 1 ; // вторая версия свежее
	else if (b1==a1) ret=-1; // версии равны
	else ret = 0; // первая версия свежее
	return ret;
}

/////////////////////////////////////////////
int SearchPrevVersionInARM (char* pack_name,  long int actions_counter, struct packs packages[], char* ARMbuffer) {
	char *architecture, full_pack_name[50];

	if(sizeof(void*) == 4) architecture = (char *)"i686";
	else if(sizeof(void*) == 8) architecture = (char *)"x86_64";
	
	strcpy (full_pack_name,pack_name);
	strcat (full_pack_name,"-");
	for (;actions_counter>0;actions_counter--) {
		if (!strcmp(pack_name,packages[actions_counter].name) && !strcmp("installed",packages[actions_counter].action)) {	// Нашли текущую установленную версию
			printf("Нашли текущую установленную версию: %s\n", packages[actions_counter].cur_version);
			//char* ver2 = "2.4.4-4";
			//char* ver1 = "20110528-2";
			//int res = VersionParser(ver1, ver2);
			//if (res==0) printf("First version is fresh!\n");
			//if (res==1) printf("Second version is fresh!\n");
			//if (res==-1) printf("Versions is equal!\n");
		}
	}
	return 0;
}
