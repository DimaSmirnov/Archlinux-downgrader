void ShowHelpWindow() {
	printf("\nDowngrader, version %s\n",VERSION);
	printf(" usage: downgrader [ -h ] [-q] [ -a ] <PACKAGE_NAME>\n");
	printf("  options:\n");
	printf("   -h   <print this help and exit>\n");
	printf("   -q   <quiet working, without any notifications> \n");
	printf("   -a   <automatic downgrade to previously version>\n");
} 
