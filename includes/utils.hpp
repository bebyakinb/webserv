#ifndef UTILS_HPP
# define UTILS_HPP
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <cstring>
# include <map>
# include <iostream>
# include <sys/stat.h>

bool		    if_file_exists(std::string file_name);
static	int		sent_len(char const *s, char c);
static	char	**free_function(char **result, int main_count);
static	char	**fill_word(char const *s, char c, char **result);
char			**ft_split(char const *s, char c);
std::string		get_cwd_string();


typedef struct t_time
{
    char    *weekday;
    char    *month;
    char    *day;
    char    *time_no_seconds;
    char    *year;
} t_time;

class Server;
class Response;

typedef struct t_info_to_cgi
{
	std::string							_body;
    std::string							_filePath;
    std::string							_cgi_path;
    std::string							_url;
    Server*								_server;
	struct s_location					*_currentLocation;
    Response							*_response;
    std::string							_answer;
	unsigned long						_bytesToSend;
	std::map<std::string,std::string>	_headers;
} t_info_to_cgi;


#endif