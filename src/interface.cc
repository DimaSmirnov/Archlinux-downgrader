#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <var.h>

#define PACKAGE    "downgrader"
#define VERSION    "0.18"

//////////////////////////////////////////////
void ShowStartWindow(long int actions_counter, struct packs packages[]) {
	int op_number, p_qty;
	char p_name[30];
	
	printf("Welcome to package downgrader. \nThis programm helps you to downgrade some packages, and resolve system unstability problems\n\n");	
	printf("Please select necessary operation:\n");
	printf("1. Downgrade 1 package\n");	
	printf("2. Downgrade some last packages\n");
	printf("Enter operation number: ");
	scanf ("%d",&op_number);

	if (op_number==1) { 
		printf("Please enter package name: ");
		scanf ("%s",p_name);
		SilentDowngradePackage (p_name, actions_counter, &packages[0]);
	}
	else if (op_number==2) {
		printf("Please enter package q-ty: ");
		scanf ("%d",&p_qty);
		DowngradeLastPackages (p_qty, actions_counter, &packages[0]);
	}
	else exit(1);
}
///////////////////////////////////////////
int ShowHelpWindow() {
 printf("\n%s, version %s\n", PACKAGE, VERSION); 
 printf(" usage: %s [ -h ] [ -l <packages q-ty> ] <PACKAGE_NAME>\n", PACKAGE);
 printf("  options:\n");
 printf("   -h   <print this help and exit>\n");
 printf("   -l N <downgrade N last packages>\n\n");
}
