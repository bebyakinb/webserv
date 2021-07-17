#ifndef CGI_HPP
# define CGI_HPP
# include <iostream>
# include "utils.hpp"
# include "Exceptions.hpp"
# include "RequestHandler.hpp"
# include <map>
# include <unistd.h>
# include <fcntl.h>
# include <vector>
# include <sys/types.h>
# include <sys/wait.h>

//struct t_info_to_cgi;

class Cgi{
public:
	void cgi_start(t_info_to_cgi *info);
	void cgi_set_envs();
	void cgi_usage();
	void cgi_usage_2(char **filename);
	void cgi_response();
	void map_envs_to_char_array();
	std::string read_from_file(int fd);
	const std::string &getResponseBody() const;
	const std::string &getAnswerHeader() const;
private:
	t_info_to_cgi                       *_cgi_info;
	std::map<std::string, std::string>  _map_envp;
	std::string                         _response_body;
	std::string                         _answer_header;
	char                                **_env;
};

#endif //CGI_HPP
