#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <alpm.h>
#include <alpm_list.h>
#include <curl/curl.h>
#include <syslog.h>
#include "cJSON.h"

#define MAX_PKGS_FOR_USER 30
#define WITH_ALA 1
#define WITH_ARM 0
#define VERSION "2.0.0-1"


#include "variables.h"
#include "Interface.h"
#include "actions.c"
#include "ala.c"
#include "arm.c"

// git checkout -b testing

int main(int argc, char **argv) {
	char *package;
	int param, single=0;
	int quiet_downgrade;
	
	if (argc==2) package = argv[1];
	else if (argc==3) package = argv[2];
	else { ShowHelpWindow(); return 0; }
	while ((param = getopt (argc, argv, "q:ha")) != -1)
	switch (param) {
		case 'h':{ ShowHelpWindow(); return 0; }			
		case 'q':{ silent = 1; break; }
		case 'a':{ single = 1; break; }
	}
	init = Initialization(package);
	if (init) return -1;
	

	if (single) { // Downgrade single package
		sprintf(tmp_string,"Downgrade package: %s",package);
		syslog(LOG_INFO, tmp_string);
		sprintf (full, "\033[1;%dm %s \033[0m \n", 31, tmp_string);
		dgr_output(full);
		if (!quiet_downgrade) { sprintf (tmp_string, "Installed version: %s\n",installed_pkg_ver); dgr_output(tmp_string);}
		int down_result = DowngradePackage(package);
	}
	else { // Show possible packages list when downgrade
		int pac_num;
		sprintf(tmp_string,"Downgrade package: %s",package);
		syslog(LOG_INFO, tmp_string);
		sprintf (full, "\033[1;%dm %s \033[0m \n", 31, tmp_string);
		dgr_output(full);
		str = GetChoiseForPackage(package);
		if (!strcmp(str,"q")) { Deinitialization(); return 0; }
		else pac_num = atoi(str);
		strcpy(install_command,"sudo pacman -U "); strcat(install_command, user_pkgs[pac_num].link);
		//printf ("%s\n",install_command); // DEBUG
		system(install_command);
	}
	Deinitialization();
return 0;	
}
