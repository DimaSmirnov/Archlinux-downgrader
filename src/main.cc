#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <var.h>


int main(int argc, char **argv)  
{
		FILE *pFile;
		int start_flag=2;
	
		pFile=fopen("/var/log/pacman.log","r");
		long int actions_counter = GetPacmanSize(pFile); // узнаем нужный размер структуры и выделяем память для базы
		struct packs  *packages = (struct packs *)malloc(sizeof(struct packs)*actions_counter);
		//struct cur_packs  *cur_packages = (struct cur_packs *)malloc(sizeof(struct cur_packs)*actions_counter);
		rewind(pFile);
        actions_counter = ReadPacmanLog(&packages[0], pFile);
        fclose(pFile);

		if (argc>1) {
			//if(!strcmp(argv[1], "-d")) start_flag=1;
			//else 
			if(!strcmp(argv[1], "-h")) start_flag=6;
			else if(!strcmp(argv[1], "-l")) start_flag=3;
			else if(!strcmp(argv[1], "-p")) start_flag=4;
			else if(!strcmp(argv[1], "-s")) start_flag=5;
		}

		//if (start_flag==1) DowngradeToDate (argv[2]);
		//else 
		if (start_flag==2) ShowStartWindow(actions_counter,&packages[0]);
		else if (start_flag==3) DowngradeLastPackages (atoi(argv[2]), actions_counter, &packages[0]);
		else if (start_flag==4) SilentDowngradePackage (argv[2], actions_counter,&packages[0]);
		else if (start_flag==5) PacmanStatistic (actions_counter);
		else if (start_flag==6) ShowHelpWindow();

		free (packages);
		return 0;
}
