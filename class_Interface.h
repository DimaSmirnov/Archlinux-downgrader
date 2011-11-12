class Interface {
	public:
	void ShowHelpWindow();
		////
};
//////////////////////////////////////////////////
void Interface::ShowHelpWindow() {
	printf("\ndowngrader, version 0.20\n"); 
	printf(" usage: downgrader [ -h ] [ -l <packages q-ty> ] <PACKAGE_NAME>\n");
	printf("  options:\n");
	printf("   -h   <print this help and exit>\n");
	printf("   -q   <quiet working>\n");
	printf("   -l N   <gave a list of available packages versions for downgrade package N>\n");	
	printf("   -n N <downgrade N last packages>\n\n");
}
