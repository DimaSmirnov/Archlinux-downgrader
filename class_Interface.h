class Interface {
	public:
	void ShowHelpWindow();
		////
};
//////////////////////////////////////////////////
void Interface::ShowHelpWindow() {
	printf("\nDowngrader, version %s (libalpm.so.8)\n",VERSION);
	printf(" usage: downgrader [ -h ] [ -l <package name> ] <PACKAGE_NAME>\n");
	printf("  options:\n");
	printf("   -h   <print this help and exit>\n");
	//printf("   -q   <quiet working> (in progress)\n");
	printf("   -l N   <gave a list of available packages versions for downgrade package N>\n");
}
