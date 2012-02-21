//#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <alpm.h>
#include <alpm_list.h>
#include <sys/stat.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
////////////////////////////////////
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
#include "class_Interface.h"
#include "class_Actions.h"

int main(int argc, char **argv) {
	char *package;
	int param, quiet_downgrade = 0, list_downgrade = 0, show_list = 0;

	if (argc) package = argv[1];
	while ((param = getopt (argc, argv, "q:hl:n:")) != -1)
		switch (param) {
			case 'q':{ // Quiet downgrade
				quiet_downgrade = 1;
				package = optarg;
				break;
			}
			case 'l':{ // Show list of possible packages on downgrade
				show_list = 1;
				package = optarg;
				//printf("Show on downgrade: %s\n",package);
				break;
			}
			case 'n':{ // Downgrade n packages
				list_downgrade = atoi(optarg);
				break;
			}
			case 'h':{ // help
				Interface interface;
				interface.ShowHelpWindow();
				return 0;
			}
		}
	Actions actions;
	/////////////////////////////// Downgrade list of packages
	if (list_downgrade) {
		int ispacmaninit = actions.PacmanInit();
	    if (ispacmaninit) {
			if(!quiet_downgrade) printf("Pacman not initialized! Interrupted\n");
			return 1;
		}
		printf ("Downgrade %d last packages\n",list_downgrade);
		for (;actions.pacmanlog_length>0;actions.pacmanlog_length--) {
			if (!strcmp("upgraded",actions.packages[actions.pacmanlog_length].action)) {
				int ret = actions.CheckDowngradePossibility(actions.packages[actions.pacmanlog_length].name);
				if (!ret) {
					if (!quiet_downgrade) {
						printf ("\033[1;%dm Downgrade package: %s \033[0m \n", 31, actions.packages[actions.pacmanlog_length].name);
						printf ("Installed version: %s\n",actions.installed_pac_ver);
					}
					int result = actions.DowngradePackage(actions.packages[actions.pacmanlog_length].name);
				}
				list_downgrade--;
				if (!list_downgrade) break;
			}
		}
		actions.PacmanDeinit();
		return 0;
	}
	///////////////////////////// Show possible packages list when downgrade
	else if (show_list) {
		int def_pac = 0;
		int pac_num;

		printf ("\033[1;%dm Downgrade package: %s \033[0m \n", 31, package);
		actions.show_list = show_list;
		int ret = actions.GetChoiseFromArm(package);
		if (ret) return 1; // Ошибка инициализации
		if (!strcmp(actions.package_number,"d")) pac_num = actions.def_pac;
		else if (!strcmp(actions.package_number,"q")) return 0;
		else pac_num = atoi(actions.package_number);
		strcpy(actions.install_command,"sudo pacman -U "); strcat(actions.install_command,actions.arm_packages[pac_num-1].full_path);
		system(actions.install_command);
		//printf ("%s\n", actions.install_command);
		actions.PacmanDeinit();
		return 0;
	}
	//////////////////////////// Downgrade single package
	else if (package) {
		actions.quiet_downgrade = quiet_downgrade;
		int ispacmaninit = actions.PacmanInit();
	    if (ispacmaninit) {
			if(!quiet_downgrade) printf("Pacman not initialized! Interrupted\n");
			return 1;
		}
		printf ("\033[1;%dm Downgrade package: %s \033[0m \n", 31, package);
		int ret = actions.CheckDowngradePossibility(package);
		if (ret) return 1;
		if (!quiet_downgrade) printf ("Installed version: %s\n",actions.installed_pac_ver);
		int result = actions.DowngradePackage(package);
		actions.PacmanDeinit();
	    return result;
	}
	/////////////////////////// Else return with nonnull
	else return 1;
}
