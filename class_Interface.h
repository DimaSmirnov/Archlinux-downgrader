class Interface {
	public:
	void ShowHelpWindow();
		////
};
//////////////////////////////////////////////////
void Interface::ShowHelpWindow() {
	printf("\nDowngrader, version 1.3.0\n");
	printf(" usage: downgrader [ -h ] [ -n <packages q-ty> ] [ -l <package name> ] <PACKAGE_NAME>\n");
	printf("  options:\n");
	printf("   -h   <print this help and exit>\n");
	printf("   -q   <quiet working>\n");
	printf("   -l N   <gave a list of available packages versions for downgrade package N>\n");
	printf("   -n N <downgrade N last packages>\n\n");
}
