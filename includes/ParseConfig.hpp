
/* Повышенной интенсивности (High Intensty) */

#ifndef ParseConfigIG_HPP
# define ParseConfigIG_HPP

# define HI_BLACK		"\e[0;90m"
# define HI_RED			"\e[0;91m"
# define HI_GREEN		"\e[0;92m"
# define HI_YELLOW		"\e[0;93m"
# define HI_BLUE		"\e[0;94m"
# define HI_PURPLE		"\e[0;95m"
# define HI_CYAN		"\e[0;96m"
# define HI_WHITE		"\e[0;97m"

/* Жирный шрифт с повышенной интенсивностью (Bold High Intensty) */

# define BHI_BLACK		"\e[1;90m"
# define BHI_RED		"\e[1;91m"
# define BHI_GREEN		"\e[1;92m"
# define BHI_YELLOW 	"\e[1;93m"
# define BHI_BLUE		"\e[1;94m"
# define BHI_PURPLE		"\e[1;95m"
# define BHI_CYAN		"\e[1;96m"
# define BHI_WHITE		"\e[1;97m"

/* Сброс */

# define RESET			"\e[0m"

#include <fcntl.h>
#include <iostream>
#include <map>
#include <vector>
#include <string.h>
#include "get_next_line/get_next_line.h"

class ParseConfig
{
	public:
		ParseConfig(char *configPath);
		~ParseConfig();

		// SET
		void setFlagLoc(bool FlagLoc);
		void setLocPath(std::string LocPath);

		// GET
		int getPosServ(void);
		bool getFlagLoc(void);
		std::vector <ParseConfig *> getServInfo(void);
		std::vector <std::string> getValueLocPath(void);
		std::vector < std::vector <std::string> > getLocPath(void);
		std::map < int, std::map<int, std::string> > &getMethods(void);
		std::map < std::string, std::map<std::string, std::string> > &getMapLoc(void);
		std::map <std::string, std::string> &getErrorPages(void);
		std::map <std::string, std::string> &getMapHeadFields(void);

		// METHODS
		void ParseConf(void);
		void ReadConf(void);
		void TestPrint(void);
		void ParseStart(char *line);
		void CheckDelimiter();
		void DelSpaceChars(std::string *str);
		void Parse1stLevel(std::string str);
		void Parse2ndLevel(std::string str);
		void FieldValidCheck(std::string *str);
		void CheckTabs(std::string *str);


	private:
		char *_configPath;
		bool _flag_loc;
		int _count_serv;
		int _pos_loc;
		int _pos_serv;
		int _count_tab;

		std::vector <ParseConfig *> _servinfo;
		std::vector < std::vector <std::string> > _loc_path;
		std::map < int, std::map<int, std::string> > _methods;
		std::map <std::string, std::string> _error_pages;
		std::map < std::string, std::map<std::string, std::string> > _location;
		std::map <std::string, std::string> _head_fields;
};


#endif
