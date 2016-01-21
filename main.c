#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <alpm.h>
#include <alpm_list.h>
#include <curl/curl.h>
#include <syslog.h>
#include "cJSON.h"

#define MAX_PKGS_FROM_ARM_FOR_USER 30
#define PATH_TO_PACMAN_CACHE "/var/cache/pacman/pkg/"

#define VERSION "1.9.0-1"

#include "variables.h"
#include "Interface.h"
#include "Actions.h"

int main(int argc, char **argv) {
	char *package;
	int param;
	int quiet_downgrade=listing=0;
	
	if (argc==2) package = argv[1];
	else if (argc==3) package = argv[2];
	else { ShowHelpWindow(); return 0; }
	while ((param = getopt (argc, argv, "q:hl:")) != -1)
	switch (param) {
		case 'h':{ ShowHelpWindow(); return 0; }			
		case 'q':{ silent = 1; break; }
		case 'l':{ listing = 1; break; }
	}
	init = Initialization(package);
	if (init) return -1;
	
	if (listing) { // Show possible packages list when downgrade
		int pac_num;
		sprintf(tmp_string,"Downgrade package: %s",package);
		syslog(LOG_INFO, tmp_string);
		sprintf (full, "\033[1;%dm %s \033[0m \n", 31, tmp_string);
		dgr_output(full);
		str = GetChoiseForPackage(package);
		if (ret<0) { Deinitialization(); return 0; }
		else if (!strcmp(str,"q")) return 0;
		else pac_num = atoi(str);
		strcpy(install_command,"sudo pacman -U "); strcat(install_command, arm_pkgs[pac_num].link);
		system(install_command);
	}
	else if (package) { // Downgrade single package
		sprintf(tmp_string,"Downgrade package: %s",package);
		syslog(LOG_INFO, tmp_string);
		sprintf (full, "\033[1;%dm %s \033[0m \n", 31, tmp_string);
		dgr_output(full);
		ret = CheckDowngradePossibility(package);
		if (ret<0) { Deinitialization(); return 0; }
		
		if (!quiet_downgrade) { sprintf (tmp_string, "Installed version: %s\n",installed_pkg_ver); dgr_output(tmp_string);}
		int down_result = DowngradePackage(package);
	}
	return 0;	
}
