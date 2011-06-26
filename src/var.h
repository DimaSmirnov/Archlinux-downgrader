long int ReadPacmanLog(struct packs packages[], FILE*);
long int GetPacmanSize(FILE*);
int DowngradeToDate(char*);
int DowngradeLastPackages (int, long int, struct packs packages[]);
int SilentDowngradePackage (char*, long int, struct packs packages[]);
int PacmanStatistic (int);
int VersionParser(char*, char*);
int IsPackageInAur(char*);
char* GetPrevPackageFromARM(char*, char*, char*);
void ShowStartWindow(long int, struct packs packages[]) ;
int ShowHelpWindow();
int SearchPrevVersionInARM (char*, long int, struct packs packages[], char *);
void ReadARM(char*, char*);


char *get_ip(char *host);
char *build_get_query(char *host, char *page);



struct  packs{ // -- Действия с пакетами
      	char name[40]; // название пакета
        char date[10]; // дата операции
        char time[20]; // время операции
		char action[20]; // название операции (upgraded, installed, removed)
        char prev_version[20]; // предыдущая версия
        char cur_version[20]; // текущая версия
};
//struct  cur_packs{ // -- Структура для списка устанавливаемых пакетов
 //     	char name[40]; // название пакета
   //     char version[20]; // устанавливаемая версия
//};
