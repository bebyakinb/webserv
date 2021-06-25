#include "ParseConfig.hpp"

ParseConfig::ParseConfig(char *configPath)
{
	this->_configPath = configPath;
	this->_pos_loc = -1;
	this->_pos_serv = 0;
	this->_count_tab = 0;
	this->_count_serv = 1;
	this->_flag_loc = false;
	this->_loc_path.push_back(std::vector<std::string>());
}

ParseConfig::~ParseConfig()
{

}

// SET

void ParseConfig::setLocPath(std::string LocPath)
{
	this->_loc_path[this->_loc_path.size() - 1].push_back(LocPath);
}

void ParseConfig::setFlagLoc(bool FlagLoc)
{
	this->_flag_loc = FlagLoc;
}

// GET

int ParseConfig::getPosServ(void)
{
	return (this->_pos_serv);
}

std::vector <ParseConfig *> ParseConfig::getServInfo(void)
{
	return (this->_servinfo);
}

bool ParseConfig::getFlagLoc(void)
{
	return (this->_flag_loc);
}

std::vector <std::string> ParseConfig::getValueLocPath(void)
{
	return(this->_loc_path[0]);
}

std::vector < std::vector <std::string> > ParseConfig::getLocPath()
{
	return (this->_loc_path);
}

std::map < int, std::map<int, std::string> > &ParseConfig::getMethods(void)
{
	return (this->_methods);
}

std::map < std::string, std::map<std::string, std::string> > &ParseConfig::getMapLoc(void)
{
	return (this->_location);
}

std::map <std::string, std::string> &ParseConfig::getErrorPages(void)
{
	return (this->_error_pages);
}

std::map <std::string, std::string> &ParseConfig::getMapHeadFields(void)
{
	return (this->_head_fields);
}

void ParseConfig::DelSpaceChars(std::string *str)
{
	while((*str)[0] == '\t' || (*str)[0] == ' ')
		str->erase(0, 1);
}

void ParseConfig::CheckTabs(std::string *str)
{
	if (_servinfo[_pos_serv]->_flag_loc == true && _count_tab != 2)
		throw "invalide tabulation";
	else if (((str->substr(0, str->find(':')) == "server" && _count_tab > 0) ||
		(str->substr(0, str->find(':')) != "server" && _count_tab != 1) ||
		(str->substr(0, str->find(':')) == "location" && _count_tab != 1)) &&
		_servinfo[_pos_serv]->_flag_loc == false)
		throw "invalide tabulation";
}

void ParseConfig::FieldValidCheck(std::string *str)
{
	int flag_method_and_errors = 0;
	str->substr(0, str->find(':') == std::string::npos ? str->find(' ') : str->find(':')) == "method" ||
	str->substr(0, str->find(':') == std::string::npos ? str->find(' ') : str->find(':')) == "error_page" ?
	flag_method_and_errors = 1 : 0;
	CheckTabs(str);
	if (str->substr(0, str->find(' ')).find(':') == str->npos)
		throw "expected ':' after key name";
	str->erase(0, str->substr(0, str->find(' ')).find(':') + 1);
	DelSpaceChars(str);
	std::string tmp = str->substr(str->substr(0,
	str->find(' ') < 0 ? str->find('\t') : str->find(' ')).size(), str->size());
	DelSpaceChars(&tmp);
	if (!tmp.empty() && !flag_method_and_errors)
		throw "invalid symbols";
}

void ParseConfig::Parse2ndLevel(std::string str)
{
	std::string tmp;
	std::string second_level[5] = {"root", "index", "method", "cgi_extension", "autoindex"};

	tmp = str.substr(0, str.find(':') == std::string::npos ? str.find(' ') : str.find(':'));
	_servinfo[_pos_serv]->_location[_servinfo[_pos_serv]->getValueLocPath()[_pos_loc]].count(tmp) == 1 ? throw "double field" : 0;
	for (size_t i = 0; i < 5; i++)
	{
		if (tmp == "method")
		{
			char *tmp;

			FieldValidCheck(&str);
			tmp = strtok((char *)str.c_str(), " ");
			for (size_t i = 0; tmp; i++)
			{
				for (size_t a = 0; _servinfo[_pos_serv]->_methods[_pos_loc].find(a) !=
					_servinfo[_pos_serv]->_methods[_pos_loc].end(); a++)
				{
					if (_servinfo[_pos_serv]->_methods[_pos_loc][a] == tmp)
						throw "double method value";
				}
				_servinfo[_pos_serv]->_methods[_pos_loc][i] = std::string(tmp);
				tmp = strtok(NULL, " ");
			}
			return ;
		}
		if (second_level[i] == tmp)
		{
			FieldValidCheck(&str);
			_servinfo[_pos_serv]->_location[_servinfo[_pos_serv]->getValueLocPath()[_pos_loc]][tmp] = str.substr(0, str.find(' '));
			return ;
		}
	}
	if (!tmp.empty())
		throw "invalid symbols";
}

void ParseConfig::Parse1stLevel(std::string str)
{
	std::string tmp;
	std::string first_level[7] = {"server", "server_name", "host", "port", "max_body_size", "location", "error_page"};

	tmp = str.substr(0, str.find(':') == std::string::npos ? str.find(' ') : str.find(':'));
	_servinfo[_pos_serv]->_head_fields.count(tmp) == 1 ? throw "double field" : 0;
	for (size_t i = 0; i < 7; i++)
	{
		if (tmp == "error_page")
		{
			char *tmp;
			char *num_error;

			FieldValidCheck(&str);
			tmp = strtok((char *)str.c_str(), " ");
			num_error = tmp;
			tmp = strtok(NULL, " ");
			_servinfo[_pos_serv]->_error_pages[num_error] = std::string(tmp);

			if ((tmp = strtok(NULL, " ")))
				throw "invalid symbols";
			return ;
		}
		if (tmp == "location")
		{
			FieldValidCheck(&str);
			for (int a = 0; a < _pos_loc; a++)
			{
				if (_servinfo[_pos_serv]->_loc_path[0][a] == str)
					throw "double location path";
			}
			_servinfo[_pos_serv]->setLocPath(str);
			_servinfo[_pos_serv]->_flag_loc = true;
			return ;
		}
		if (first_level[i] == tmp)
		{
			FieldValidCheck(&str);
			tmp == "server" && !str.empty() ? throw "invalid symbols" : 0;
			_servinfo[_pos_serv]->_head_fields[tmp] = str.substr(0, str.find(' '));
			return ;
		}
	}
	if (!tmp.empty())
		throw "invalid symbols";
}

void ParseConfig::CheckDelimiter()
{
	if (_count_serv - 1 > _pos_serv)
		throw "expected delimiter: '---'";

	else if (_count_serv - 1 < _pos_serv)
		throw "extra delimiter character '---'";

	_count_serv++;
}

void ParseConfig::ParseStart(char *line)
{
	std::string str = line;

	_count_tab = 0;
	for (size_t i = 0; str[i] == '\t'; i++)
		_count_tab++;
	DelSpaceChars(&str);
	if (str[0] == '#')
		return ;
	if (str.substr(0, str.find(':') == std::string::npos ? str.find(' ') : str.find(':')) == "server")
		CheckDelimiter();
	if (str.substr(0, str.find(':') == std::string::npos ? str.find(' ') : str.find(':')) == "location")
	{
		_pos_loc++;
		_servinfo[_pos_serv]->_flag_loc = false;
	}
	if (str.substr(0, str.find(':') == std::string::npos ? str.find(' ') : str.find(':')) == "---")
	{
		_pos_serv++;
		_pos_loc = -1;
		_servinfo.push_back(new ParseConfig(_configPath));
	}
	else if (_servinfo[_pos_serv]->_flag_loc == false)
		Parse1stLevel(str);
	else
		Parse2ndLevel(str);
}

void ParseConfig::TestPrint(void)
{
	for (int j = 0; j <= _pos_serv; j++)
	{
		std::cout << "server_name: " << _servinfo[j]->_head_fields["server_name"] << std::endl;
		std::cout << "host: " << _servinfo[j]->_head_fields["host"] << std::endl;
		std::cout << "port: " << _servinfo[j]->_head_fields["port"] << std::endl;
		std::cout << "max_body_size: " << _servinfo[j]->_head_fields["max_body_size"] << std::endl << std::endl;
		std::cout << "error_page: " << _servinfo[j]->_error_pages["404"] << std::endl;
		std::cout << "error_page: " << _servinfo[j]->_error_pages["403"] << std::endl << std::endl;

		for (int i = 0; i <= _pos_loc; i++)
		{
			std::cout << "location path: " << _servinfo[j]->getValueLocPath()[i] << std::endl;
			std::cout << "	root: " << _servinfo[j]->_location[_servinfo[j]->getValueLocPath()[i]]["root"] << std::endl;
			std::cout << "	index: " << _servinfo[j]->_location[_servinfo[j]->getValueLocPath()[i]]["index"] << std::endl;

			std::cout << "	method: ";
			for (size_t a = 0; _servinfo[j]->_methods[_pos_loc].find(a) !=
				_servinfo[j]->_methods[_pos_loc].end(); a++)
				std::cout << _servinfo[j]->_methods[_pos_loc][a] << " ";
			std::cout << std::endl;

			std::cout << "	cgi_extension: " << _servinfo[j]->_location[_servinfo[j]->getValueLocPath()[i]]["cgi_extension"] << std::endl;
			std::cout << "	autoindex: " << _servinfo[j]->_location[_servinfo[j]->getValueLocPath()[i]]["autoindex"] << std::endl;

			std::cout << std::endl << "---" << std::endl << std::endl;
		}
	}
}

void ParseConfig::ReadConf(void)
{
	char *line;
	int n_line = 1;
	int fd = open(_configPath, O_RDONLY);

	try
	{
		while (get_next_line(fd, &line) > 0)
		{
			ParseStart(line);
			free(line);
			n_line++;
		}
		ParseStart(line);
		free(line);
	}
	catch(const char *ex)
	{
		std::cerr << BHI_RED << "config file error: " << RESET << ex
		<< "  (line: " << n_line << ")" << '\n';
		exit(EXIT_FAILURE);
	}
	//TestPrint();
}

void ParseConfig::ParseConf(void)
{
	_servinfo.push_back(new ParseConfig(_configPath));
	ReadConf();
}