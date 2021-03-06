	typedef enum {
		DGR_NO_PKG_IN_ARM = 1,
		DGR_NO_PKG_IN_AUR = 2,
		DGR_PKG_NOT_INSTALL = 3,
		DGR_READ_ARM_ERROR = 4,
		DGR_READ_AUR_ERROR = 5
	} DwgrRetSatus;

	typedef struct _dgr_data {
		char string1[500];
		int int1;
	} dgr_data;
	dgr_data DgrData;
	// DgrData.int1 =3;

	int Initialization(char *);
	int Deinitialization();
	int IsPackageAvailable( char *);
	int IsPackageInLogs( char *);
	int IsPackageInCache( char *);
	int IsPackageInAur( char *);
	int IsPackageInArm( char *, char *);
	int IsPackageInstalled(char *);
	void ReadPacmanLog();
	int ReadArm(char *);
	int ReadALA(char *);
	int IsPackageInAla(char*, char *);
	int PrepareView(char *);
	int DowngradePackage( char *);
	char* GetChoiseForPackage( char *);
	char* str_replace(char*,char*,char*);
 	int dgr_output( char *);
 	
	alpm_handle_t *alpm_handle;
	alpm_db_t *db;
	alpm_pkg_t *pkg;
	
	char *ptr, *i_com;
	char *str, *architecture;
	const char *pkgname;
	char full_path_to_packet[300], tmp_string[200], full[500];
	char install_command[300]; // Команда для установки
	char install_version[30]; // Версия пакета для установки
	const char* installed_pkg_ver;  // Текущая установленная версия
	long int pacmanlog_length;
	int ret, loglines_counter;
	int tmpint, pkgsinarm, pkgsinala, pkginala, init;
	int single, silent, counter;
	FILE *pFile;

	struct packs{ // -- Действия с пакетами из логов пакмана
		char name[50]; // название пакета
		char date[10]; // дата операции
		char time[20]; // время операции
		char action[20]; // название операции (upgraded, installed, removed)
		char cur_version[50]; // предыдущая версия
		char prev_version[50]; // предыдущая версия
	} *pkgs;
	//struct packs *pkgs;
	struct arm_packs{ // -- список пакетов в ARM
		char full_path[400]; // полный адрес до пакета
		char version[20]; // Version of package
		char name[50]; // Name of package
		char repository[20]; // Repository of package
		char link[100]; //Link for download
		int pkg_release; //Package release
	} *arm_pkgs;
	//struct arm_packs *arm_pkgs;
	struct ala_packs{ // -- список пакетов в ALA
		char full_path[400]; // полный адрес до пакета
		char name[50]; // Name of package (with version and release)
		char version[10]; // Version of package
		char full_name[50]; // Name of package with version and .tar.xz
	} *ala_pkgs;
	//struct ala_packs *ala_pkgs;
	struct user_packs{ // -- Список пакетов для вывода пользователю
		char name[50]; // название пакета
		char version[10]; // Version of package
		char link[200]; //Link for download
		char repo[20]; // ARM, ALA, Cache
	} *user_pkgs;
	//struct user_packs *user_pkgs;
	
	struct curl_MemoryStruct {
		char *memory;
		size_t size;
	};
	struct curl_MemoryStruct chunk;
	CURL *curl;
	CURLcode result;
