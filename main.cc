#include <stdio.h>
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

#include "class_Interface.h"
#include "class_Actions.h"

int main(int argc, char **argv) {
	int param;
	char *package = NULL;
	int quiet_downgrade = 0, list_downgrade = 0;

	if (argc) package = argv[1];
	while ((param = getopt (argc, argv, "q:hl:")) != -1)
		switch (param) {
			case 'q':{
				quiet_downgrade = 1;
				package = optarg;
				break;
			}
			case 'l':{
				list_downgrade = atoi(optarg);
				break;
			}
			case 'h':{
				Interface interface;
				interface.ShowHelpWindow();
				return 0;
			}
		}
	Actions actions;
	if (list_downgrade) {
		int ispacmaninit = actions.PacmanInit();
	    if (ispacmaninit) { 
			if(!quiet_downgrade) printf("Pacman not initialized! Interrupted\n"); 
			return 1;
		}
		printf ("Downgrade %d last packages\n",list_downgrade);
		for (;actions.pacmanlog_length>0;actions.pacmanlog_length--) {
			if (!strcmp("upgraded",actions.packages[actions.pacmanlog_length].action)) {
				int result = actions.DowngradePackage(actions.packages[actions.pacmanlog_length].name,quiet_downgrade);
				list_downgrade--;
				if (!list_downgrade) break;
			}
		}
		actions.PacmanDeinit();
		return 0;
	}
	else if (package) {
		int ispacmaninit = actions.PacmanInit();
	    if (ispacmaninit) { 
			if(!quiet_downgrade) printf("Pacman not initialized! Interrupted\n"); 
			return 1;
		}
		int result = actions.DowngradePackage(package,quiet_downgrade);
		actions.PacmanDeinit();
	    return result;
	}
	else return 1;
}
