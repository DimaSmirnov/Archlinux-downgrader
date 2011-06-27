#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <var.h>

//////////////////////////////////////////////
void ShowStartWindow(long int actions_counter, struct packs packages[]) {
	int op_number, p_qty;
	char p_name[30];
	
	system("clear");
	printf("Welcome to package downgrader. \nThis programm helps you to downgrade some packages, and resolve system unstability problems\n\n");	
	printf("Please select necessary operation:\n");
	printf("1. Downgrade 1 package\n");	
	printf("2. Downgrade some last packages\n");
	//printf("3. Downgrade all packages to date (restore packages situation to necessary date)\n\n");
	printf("Enter operation number: ");
	scanf ("%d",&op_number);

	if (op_number==1) { 
		printf("Please enter package name: ");
		scanf ("%s",p_name);
		SilentDowngradePackage (p_name, actions_counter, &packages[0]);
		printf ("\n \033[1;%dm --> If after downgrade problem isn`t resolved, \nyou can make one more downgrade for installing previously version \033[0m\n", 34);
	}
	else if (op_number==2) {
		printf("Please enter package q-ty: ");
		scanf ("%d",&p_qty);
		DowngradeLastPackages (p_qty, actions_counter, &packages[0]);
		printf ("\n \033[1;%dm --> If after downgrade problem isn`t resolved, please retry last operations for restoring previously versions \033[0m", 34);
	}
	//else if (op_number==3) {  }
	else exit(1);
}
///////////////////////////////////////////
int ShowHelpWindow() {
	printf ("Downgrader. ver 0.1\n Usage:\n -l N = Downgrade N packages\n -p NAME = Downgrade package NAME\n\n");
	
}
