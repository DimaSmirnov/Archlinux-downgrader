#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <var.h>
//#include <locale.h>
#include <alpm.h>
#include <alpm_list.h>

long int ReadPacmanLog(struct packs packages[], FILE  *pFile2) {

int action_counter=0;
char string[251];
char *p, *chop, *date, *time, *operat, *pack_name, *cur_version, *prev_version;

//struct arm_packs  *arm_packages = (struct arm_packs *)malloc(sizeof(struct arm_packs)*1000);

        while (!feof(pFile2)) {  // Читаем историю апгрейдов из логов
				
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
								strcpy(packages[action_counter].date,date);
								strcpy(packages[action_counter].time,time);
								strcpy(packages[action_counter].name,pack_name);
								strcpy(packages[action_counter].action,operat);
								strcpy(packages[action_counter].cur_version,cur_version);
								strcpy(packages[action_counter].prev_version,prev_version);
								action_counter++;
				}
				if (!strcmp(operat,"installed")) {
								cur_version = strtok(NULL,")");
								cur_version++;				
								strcpy(packages[action_counter].date,date);
								strcpy(packages[action_counter].time,time);
								strcpy(packages[action_counter].name,pack_name);
								strcpy(packages[action_counter].action,operat);
								strcpy(packages[action_counter].cur_version,cur_version);
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
////////////////////////////////
int pkg_cmp(const void *v1, const void *v2)
{
    pmpkg_t *p1 = (pmpkg_t *)v1;
    pmpkg_t *p2 = (pmpkg_t *)v2;
    unsigned long int s1 = alpm_pkg_get_size(p1);
    unsigned long int s2 = alpm_pkg_get_size(p2);
    return(s2 - s1);
}
/////////////////////////////////////////////
int SilentDowngradePackage (char* pack_name, long int actions_counter, struct packs packages[]) {
	char *architecture, *string, *chop, syztem[100], full_pack_name[50], full_path_to_packet[100], pack_ver[20], pack_prev_ver[40];
	long int actions = actions_counter;
	int pac_flag=0;
	FILE *pFile;

	struct arm_packs  *arm_packages = (struct arm_packs *)malloc(sizeof(struct arm_packs)*1000);

	
	if(sizeof(void*) == 4) architecture = (char *)"i686";
	else if (sizeof(void*) == 8) architecture = (char *)"x86_64";

//	pmdb_t *db = NULL;
//	alpm_list_t *i;
//	alpm_initialize();
//	alpm_option_set_dbpath("/var/lib/pacman");
 //	db = alpm_db_register_local();

	
	printf("\n \033[1;%dm --> Downgrade package %s \033[0m", 31, pack_name);
	

	int aur_result, res;
	char *inp, *line = NULL;
	size_t len = 0;
	aur_result = IsPackageInAur (pack_name); // Первым проверяем аур, пакет в нем  ?
	if (aur_result) { 
		printf("\nPackage %s is in AUR, downgrade cancelled, delete this package from system ? [y/n] ", pack_name);
		while (getline (&line, &len, stdin) >= 0) {
			res = rpmatch (line);
			if (res >= 0) {
                           char buf[50];
                           sprintf(buf,"sudo pacman -R %s\n", pack_name);
                           system(buf);
			}
		}
		free (line);
		return 1;
	} // В Аур пакета нет
	

	
	strcpy (full_pack_name,pack_name);
	strcat (full_pack_name,"-");

	for (;actions_counter>0;actions_counter--) {
		if (!strcmp(pack_name,packages[actions_counter].name) && !strcmp("installed",packages[actions_counter].action)) { // нашли версию
				strcpy (pack_ver,packages[actions_counter].cur_version);
				pac_flag=1;
		}
		
		if (!strcmp(pack_name,packages[actions_counter].name) && !strcmp("upgraded",packages[actions_counter].action)) { // нашли нужный пакет для апгрейда
			strcpy (pack_ver,packages[actions_counter].cur_version);
			pac_flag=1;
			strcpy (full_pack_name,pack_name);
			strcat (full_pack_name,"-");
			if (strcmp(packages[actions_counter].cur_version, packages[actions_counter].prev_version)) { // если был апгрейд на ту же версию, то ищем дальше
					strcpy (pack_prev_ver,packages[actions_counter].cur_version);
					strcat (full_pack_name, packages[actions_counter].prev_version);
					strcpy (pack_ver,packages[actions_counter].cur_version);
					strcat (full_pack_name,"-");
					strcat (full_pack_name,architecture);
					strcat (full_pack_name,".pkg.tar.xz");
					strcpy (full_path_to_packet,"/var/cache/pacman/pkg/");
					strcat (full_path_to_packet,full_pack_name);
					break;
			}
		}
	}
					printf("\033[1;%dm(%s) \033[0m  \n", 31, pack_prev_ver);
					pFile=fopen(full_path_to_packet,"r");
					if (!pFile) {// предыдущая версия пакета существует в локалке, кайф!
						printf("Downgrade %s from cache\n", full_pack_name);
						strcpy(syztem,"sudo pacman -U "); // установка
						strcat(syztem,full_path_to_packet);
						system(syztem);
						return 1;
					}
					else fclose(pFile);

	if (!pac_flag) { printf ("\nNo information in logs about this package. Terminating\n"); return 0; }
	int arm_flag = ReadArm(pack_name, &arm_packages[0]);
	if (arm_flag==1) { printf ("\nSorry, noone source have not information about this package. Terminating\n"); return 1; }
	char* arm = IsPackageInArm(pack_name, pack_ver, &arm_packages[0]);
	if (arm) { 
			printf("\033[1;%dm(%s) \033[0m  \n", 31, pack_ver);	
			printf("Downgrade %s from ARM\n", pack_name);
			strcpy(syztem,"sudo pacman -U "); // установка
			strcat(syztem,arm);
			system(syztem);
			return 1;
	}
	return 0;
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

/////////////////////////////////////////
char* IsPackageInArm(char *package, char *version, struct arm_packs arm_packages[]) {
	char *arm;
	int i=0;
	while(arm_packages[i].full_path) {
		if (strstr(arm_packages[i].full_path,version)) { return arm_packages[i-1].full_path; }
		i++;
	}
	return 0;
}
