#include <string.h>
#include <unistd.h>
#include <alpm.h>
#include <alpm_list.h>
#include <curl/curl.h>
#include "cJSON.h"

#define MAX_PKGS_FROM_ARM_TOTAL 10000
#define MAX_PKGS_FROM_ARM_FOR_USER 30
#define VERSION "1.6.7-2"

#include <fstream>
using namespace std;
#include "class_Interface.h"
#include "class_Actions.h"


int main(int argc, char **argv) {
	char *package;
	int param, quiet_downgrade = 0, list_downgrade = 0, show_list = 0;

	if (argc==2) package = argv[1];
	else if (argc==3) package = argv[2];
	else {
		Interface interface;
		interface.ShowHelpWindow();
		return 0;
	}
	while ((param = getopt (argc, argv, "q:hl:")) != -1)
		switch (param) {
			case 'q':{ // Quiet downgrade (test option)
				quiet_downgrade = 1;
				package = optarg;
				break;
			}
			case 'l':{ // Show list of possible packages for downgrade
				show_list = 1;
				package = optarg;
				break;
			}
			case 'h':{ // help
				Interface interface;
				interface.ShowHelpWindow();
				return 0;
			}
		}

	Actions actions;
	///////////////////////////// Show possible packages list when downgrade
	if (show_list) {
		int def_pac = 0;
		int pac_num;

		printf ("\033[1;%dm Downgrade package: %s \033[0m \n", 31, package);
		actions.show_list = show_list;
		int ret = actions.GetChoiseForPackage(package);
		if (ret) return 1; // Выход с ошибкой
		if (!strcmp(actions.package_number,"d")) pac_num = actions.def_pac;
		else if (!strcmp(actions.package_number,"q")) return 0;
		else pac_num = atoi(actions.package_number);
		strcpy(actions.install_command,"sudo pacman -U "); strcat(actions.install_command,actions.arm_packages[pac_num].full_path);
		system(actions.install_command);
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
		if(!quiet_downgrade) printf ("\033[1;%dm Downgrade package: %s \033[0m \n", 31, package);
		int ret = actions.CheckDowngradePossibility(package);
		if (ret) return 1;
		if (!quiet_downgrade) printf ("Installed version: %s\n",actions.installed_pkg_ver);
		int result = actions.DowngradePackage(package);
		actions.PacmanDeinit();
	    return result;
	}
	/////////////////////////// Else return with nonnull
	else return 1;
}
