long int ReadPacmanLog(struct packs packages[], FILE*);
long int GetPacmanSize(FILE*);
int DowngradeToDate(char*);
int DowngradeLastPackages (int, long int, struct packs packages[]);
int SilentDowngradePackage (char*, long int, struct packs packages[]);
int VersionParser(char*, char*);
int IsPackageInAur(char*);
char* IsPackageInArm(char*, char*, struct arm_packs arm_packages[]);
void ShowStartWindow(long int, struct packs packages[]) ;
int ShowHelpWindow();
int ReadArm(char*, struct arm_packs arm_packages[]);
int IsPackageInstalled(char*);
char *get_ip(int);
char *build_get_query(char *host, char *page);
int alpm_local_init(void);
int alpm_local_free(void);

struct  packs{ // -- Действия с пакетами из логов пакмана
      	char name[40]; // название пакета
        char date[10]; // дата операции
        char time[20]; // время операции
		char action[20]; // название операции (upgraded, installed, removed)
        char prev_version[20]; // предыдущая версия
        char cur_version[20]; // текущая версия
};
struct  arm_packs{ // -- Структура список пакетов в ARM
	char full_path[80]; // полный адрес до пакета
};
struct  pack_list{ // -- Список обработанных пакетов
	char pack_name[80]; // название пакета
};
