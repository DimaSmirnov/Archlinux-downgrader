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
	printf("   -l N <downgrade N last packages>\n\n");
}
