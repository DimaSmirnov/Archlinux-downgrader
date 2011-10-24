#include <stdio.h>  
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "var.h"
#include "functions.cc"
#include "http.cc"
#include "interface.cc"

int main(int argc, char **argv)  
{
		FILE *pFile;
		int start_flag=2, ret, c;
		char *value = NULL;

		pFile=fopen("/var/log/pacman.log","r");
		long int actions_counter = GetPacmanSize(pFile); // узнаем нужный размер структуры и выделяем память для базы
		struct packs  *packages = (struct packs *)malloc(sizeof(struct packs)*actions_counter);
		struct arm_packs  *arm_packages = (struct arm_packs *)malloc(sizeof(struct arm_packs)*actions_counter);
	
		rewind(pFile);
        actions_counter = ReadPacmanLog(&packages[0], pFile);
        fclose(pFile);
		ret = alpm_local_init();

	 	if(argc == 1) { ShowStartWindow(actions_counter, &packages[0]); goto stop; }
       while ((c = getopt (argc, argv, "hl:")) != -1)
         switch (c)
           {
         case 'h':
             ShowHelpWindow();
             goto stop;
         case 'l':
			 value = optarg;
             DowngradeLastPackages (atoi(value), actions_counter, &packages[0]);
            goto stop;
   		case ':':
    		ShowHelpWindow();
    		goto stop;
        case '?':
             ShowHelpWindow();
             goto stop;			   
           }
	// если передано просто название пакета
	SilentDowngradePackage (argv[1], actions_counter, &packages[0]);
	
////////////////////
// сюда переходим после выполнения всех операций
stop:
		alpm_local_free();
		free (packages);
		return 0;
}
