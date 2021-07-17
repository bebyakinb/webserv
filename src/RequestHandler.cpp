//
// Created by enoelia on 08.06.2021.
//

#include "RequestHandler.hpp"

RequestHandler::RequestHandler(Server *server) : _server(server){
	_response = new Response();
	_method = 42;
	_flagChuked = 0;
	// _flagParsed = 0;
	_wrongMethods = 0;
	_badContentSize = 0;
	_wrongHTTPVersion = 0;
	_badRequest = 0;
	_servAnswer = {
			{ERR400, "HTTP/1.1 400 Bad Request\n"},
			{ERR403, "HTTP/1.1 403 Forbidden\n"},
			{ERR404, "HTTP/1.1 404 Not Found\n"},
			{ERR405, "HTTP/1.1 405 Method Not Allowed\n"},
			{ERR408, "HTTP/1.1 408 Request Timeout\n"},
			{ERR505, "HTTP/1.1 505 HTTP Version Not Supported\n"}
			};
	_defaultErrors = {
			{ERR400,"<!DOCTYPE html>\n<html><title>400</title><body>Error 400 Bad Request</body></html>\n"},
			{ERR403,"<!DOCTYPE html>\n<html><title>403</title><body>Error 403 Forbidden</body></html>\n"},
			{ERR404,"<!DOCTYPE html>\n<html><title>404</title><body>Error 404 Not Found</body></html>\n"},
			{ERR405,"<!DOCTYPE html>\n<html><title>405</title><body>Error 405 Method Not Allowed</body></html>\n"},
			{ERR408,"<!DOCTYPE html>\n<html><title>408</title><body>Error 408 Request Timeout</body></html>\n"},
			{ERR505,"<!DOCTYPE html>\n<html><title>505</title><body>Error 505 HTTP Version Not Supported</body></html>\n"}
	};
}

RequestHandler::RequestHandler(const RequestHandler &other){
	*this = other;
}

RequestHandler::~RequestHandler() {
	delete _response;
}

RequestHandler&	RequestHandler::operator=( const RequestHandler &other){
	if (this != &other) {
		;
	}
	return (*this);
}

int					RequestHandler::getMethod() const {
	return _method;
}

void				RequestHandler::setMethod(int method) {
	_method = method;
}

Server*				RequestHandler::getServer() const {
	return _server;
}

void				RequestHandler::setServer(Server *server) {
	_server = server;
}

const std::string&	RequestHandler::getRawRequest() const {
	return _rawRequest;
}

void				RequestHandler::setRawRequest(const std::string &rawRequest) {
	_rawRequest = rawRequest;
}

const std::string &RequestHandler::getAnswer() const {
	return _answer;
}

unsigned long RequestHandler::getBytesToSend() const {
	return _bytesToSend;
}

void RequestHandler::pushBody(std::string strBody)
{
	this->_strsBody[this->_strsBody.size() - 1].push_back(strBody);
}


void				RequestHandler::testPrint()
{
	std::cout << std::endl;

	std::cout << "_wrongMethods: "<< _wrongMethods << std::endl;
	std::cout << "_badContentSize: " << _badContentSize << std::endl;
	std::cout << "_wrongHTTPVersion: " << _wrongHTTPVersion << std::endl;
	std::cout << "_badRequest: " << _badRequest << std::endl;

	std::cout << std::endl;

	std::cout << "_method: " << _method << std::endl;
	std::cout << "_url: " << _url << std::endl;

	std::cout << std::endl;

	std::cout << "Host: " << _headers["Host"] << std::endl;
	std::cout << "Content-Type: " << _headers["Content-Type"] << std::endl;
	std::cout << "Content-Length: " << _headers["Content-Length"] << std::endl;
	std::cout << "Transfer-Encoding: " << _headers["Transfer-Encoding"] << std::endl;
	std::cout << "test: " << _headers["lol"] << std::endl;

	std::cout << std::endl;

	std::cout << "_body: " << std::endl << std::endl << _body << std::endl;
}

int					RequestHandler::checkNewPartOfRequest(char *partOfRequest, int size){
	_rawRequest.append(partOfRequest, size);
	int status = parseRequest();
	if (status == 0) {//парсинг запроса на готовность к обработке(наличие \n\r\n\r) + заполнение полей
		return 0;
//	} else if (status == -1){
//		if (_badRequest || _badContentSize){
//			responseError(ERR400);
//		} else if (_wrongHTTPVersion){
//			responseError(ERR505);
//		} else if (_wrongMethods){
//			responseError(ERR405);
//		} else {
//			responseError(ERR400);
//		}
	} else {
		prepareResponse();
		_rawRequest = "";
	}
	return 1;
}

int					RequestHandler::checkFirstStr(std::cmatch result, std::regex rex)
{
	std::string first_str;

	if (_rawRequest.find("\r\n") != std::string::npos)
	{
		first_str = _rawRequest.substr(0, _rawRequest.find("\r\n") + 2);
		if (std::regex_match(first_str.c_str(), result, rex))
		{
			if (result[1] == "GET")
				_method = 0;
			else if (result[1] == "POST")
				_method = 1;
			else if (result[1] == "DELETE")
				_method = 2;
			else if (result[1] == "PUT")
				_method = 3;
			else
			{
				_wrongMethods = 1;
				return (-1);
			}
			_url = result[2];
			if (result[3] != "HTTP/1.1")
			{
				_wrongHTTPVersion = 1;
				return (-1);
			}
			_flagParsed = 1;
		}
		else if (!first_str.empty())
		{
			_badRequest = 1;
			return (-1);
		}
	}
	return (0);
}

int					RequestHandler::checkDoubleFields(std::cmatch result)
{
	char *tmp = (char *)result[1].str().c_str();

	if (islower(tmp[0]))
		tmp[0] = tmp[0] - 32;
	for (size_t i = 1; i < result[1].length(); i++)
	{
		if (!islower(tmp[i]))
			tmp[i] = tmp[i] + 32;
	}
	if (_headers.find(tmp) != _headers.end())
		return (-1);
	return (0);
}

int					RequestHandler::checkHeaders(std::cmatch result, std::regex rex)
{
	std::string oneHeader;
	std::string rawRequestTmp = _rawRequest.substr(0, _rawRequest.find("\r\n\r\n") + 4);

	rawRequestTmp.erase(0, rawRequestTmp.find("\r\n") + 2);
	while (rawRequestTmp != "\r\n")
	{
		oneHeader = rawRequestTmp.substr(0, rawRequestTmp.find("\r\n") + 2);
		if (!std::regex_match(oneHeader.c_str(), result, rex) || checkDoubleFields(result))
		{
			_badRequest = 1;
			return (-1);
		}
		_headers[result[1]] = result[2];
		rawRequestTmp.erase(0, rawRequestTmp.find("\r\n") + 2);
	}
	return (0);
}

int					RequestHandler::checksAfterParse(std::cmatch result, std::regex rex)
{
    if ((_headers.find("Content-Length") != _headers.end() ||
         _headers.find("Transfer-Encoding") != _headers.end()) &&
        _rawRequest.substr(_rawRequest.find("\r\n\r\n") + 4).empty() &&
        std::atoi(_headers["Content-Length"].c_str()) != 0 && _method != 1)
        return (0);
    if (((_headers.find("Content-Length") == _headers.end() &&
          _headers.find("Transfer-Encoding") == _headers.end() &&
          !_rawRequest.substr(_rawRequest.find("\r\n\r\n") + 4).empty()) ||
         (_headers.find("Transfer-Encoding") != _headers.end() &&
          _headers["Transfer-Encoding"] != "chunked")) && _method != 1)
	{
		_badRequest = 1;
		return (-1);
	}
	if (_headers.find("Transfer-Encoding") != _headers.end() && _method != 0)
	{
		int count = 0;
		std::string oneStrBody;
		std::regex rexCheckEnd("(0\r\n\r\n)");
		std::string tmpBody = _rawRequest.substr(_rawRequest.find("\r\n\r\n") + 4);

		if (!std::regex_search(tmpBody.c_str(), result, rexCheckEnd))
		{
			_badRequest = 1;
			return (-1);
		}
		while (tmpBody != "\r\n")
		{
			oneStrBody = tmpBody.substr(0, tmpBody.find("\r\n") + 2);
			if (!std::regex_match(oneStrBody.c_str(), result, rex))
			{
				_badRequest = 1;
				return (-1);
			}
			tmpBody.erase(0, tmpBody.find("\r\n") + 2);
			pushBody(result[1]);
			count++;
		}
		for (size_t i = 0; i < count - 1; i += 2)
		{
			if (strtol(_strsBody[0][i].c_str(), NULL, 16) != _strsBody[0][i + 1].size())
			{
				_badRequest = 1;
				return (-1);
			}
		}
		_body = _rawRequest.substr(_rawRequest.find("\r\n\r\n") + 4);
	}
	else if (_headers.find("Content-Length") != _headers.end() && _method != 0)
	{
		if ((/*strtol(_headers["Content-Length"].c_str(), NULL, 10) > _server->getMaxBodySize() || */
					strtol(_headers["Content-Length"].c_str(), NULL, 10) == 0L ||
					strtol(_headers["Content-Length"].c_str(), NULL, 10) == ULONG_MAX ||
					strtol(_headers["Content-Length"].c_str(), NULL, 10) < 0) &&
			_headers["Content-Length"] != "0")
		{
			_badContentSize = 1;
			return (-1);
		}
		if (_rawRequest.substr(_rawRequest.find("\r\n\r\n") + 4).size() <
			std::atoi(_headers["Content-Length"].c_str()))
		{
			_body = _rawRequest.substr(_rawRequest.find("\r\n\r\n") + 4);
			return (0);
		}
		else
			_body = _rawRequest.substr(_rawRequest.find("\r\n\r\n") + 4, std::atoi(_headers["Content-Length"].c_str()));
	}
	return (1);
}

int					RequestHandler::parseRequest()
{
	std::cmatch result;
	std::regex rex_first_str("(^[\r\n]+|[A-Z]+)(?: )"
							 "([\\w\\:\\.\\/-]+)"
							 "(?: )"
							 "(HTTP/+[\\d]+\\.[\\d]+)"
							 "(?:\r\n)");

	std::regex rex_headers("([\\w-]+)"
						   "(?:\\:)"
						   "(?:[ ]{1}|)"
						   "(.+)"
						   "(?:[ ]{1}|)"
						   "(?:\r\n)");

	std::regex rex_body("([.]*[^\r\n]+)"
						"(?:\r\n)");

	for (size_t i = 0; _rawRequest[0] == '\r' && _rawRequest[1] == '\n';)
		_rawRequest.erase(0, 2);
	if (_rawRequest.find("\r\n\r\n") != std::string::npos)
	{
		if (checkFirstStr(result, rex_first_str) == -1)
			return (-1);
		if (checkHeaders(result, rex_headers) == -1)
			return (-1);
		return (checksAfterParse(result, rex_body));
	}
	return (0);


	// //<Заглушка>
	// _method = GET;
	// _url = "/index3.html";
	// _headers.insert( std::pair<std::string, std::string>("Content-Length","555"));
	// //</Заглушка>
}


void				RequestHandler::prepareResponse(){
	if (_method == GET && _url.back() != '/' && _url.find('.') == std::string::npos)
		_url += "/";

	setUpPathFromUrl(std::string::npos);
	struct stat buff;
	if (_method == GET && _currentLocation->methods[GET] ) {
		if (stat(_filePath.c_str(), &buff) == -1){
			responseError(ERR404);//нет такого пути или файла
		} else if (S_ISDIR(buff.st_mode)){
			if (!_currentLocation->index.empty()){
				_filePath += _currentLocation->index;
				if (stat((_filePath).c_str(), &buff) == -1) {
					responseError(ERR404);
				} else {
					responseToGetRequest();
				}
			} else if (_currentLocation->autoindex == 1) {
				autoindex_execution();
			} else if (stat((_filePath + "index.html").c_str(), &buff) != -1) {
				responseToGetRequest();
			} else
				responseError(ERR404);
		} else if (S_ISREG(buff.st_mode)){
			responseToGetRequest();
		}
	}  else if (_method == POST && _currentLocation->methods[POST] ) {
	    //в методе post мне нужно, чтобы только url - все после location
        std::string tmp_path;
        std::string tmp_url;
        std::string name;
        std::size_t found;
        if (_filePath.back() == '/') {
            found = _filePath.find_last_of('/');
            tmp_path = _filePath.substr(0, found);
            _filePath = tmp_path;
            found = _url.find_last_of('/');
            tmp_url = _url.substr(0, found);
            _url = tmp_url;
        }
        if (!_currentLocation->cgi_path.empty()) {
            if (stat(_currentLocation->root.c_str(), &buff) == -1)
                responseError(ERR404);
            else if (S_ISDIR(buff.st_mode))
                responseToPostRequest();
            else
                responseError(ERR404);
        } else {
            if (_url == "" || _url == "/")
                responseError(ERR400);
            if (stat((_currentLocation->root).c_str(), &buff) == -1)
                responseError(ERR404);
            else if (S_ISDIR(buff.st_mode)) {
                responseToPostRequest();
            } else {
                responseError(ERR404);
            }

//            found = _filePath.find_last_of('/');
//            tmp_path = _filePath.substr(0, found + 1);
//            name = _filePath.substr(found + 1);
//            _filePath = tmp_path;
//            if (name == _url || _url == "")
//                responseError(ERR400);
//            else
//            {
//                found = _filePath.find_last_of('/');
//                tmp_path = _filePath.substr(0, found + 1);
//                _url = _filePath.substr(found + 1);
//                _filePath = tmp_path;
//                if (stat((_filePath + _url).c_str(), &buff) == -1) {
//                    responseError(ERR404);
//                } else if (S_ISDIR(buff.st_mode)) {
//                    _url = _url + name;
//                    responseToPostRequest();
//                } else {
//                    responseError(ERR404);
//                }
//            }
        }
    }
	else if (_method == DELETE && _currentLocation->methods[DELETE] )
	{
		std::size_t found = _filePath.find_last_of("/");
		_url = _filePath.substr(found+1);
		if (stat(_filePath.c_str(), &buff) == -1)
			responseError(ERR404);//нет такого пути или файла
		else
		{
			if (_url.empty() || !(_url != "/"))
				responseError(ERR400);
			else
				responseToDeleteRequest();//Compare to original NGINX
		}
	}
	else {
		responseError(ERR405);
	}
	_answer = _response->receiveAnswer();
	_bytesToSend = _answer.length();
}
int 				RequestHandler::setUpPathFromUrl(size_t lastSlashUrlPos){
	size_t newLastSlashUrlPos;
	if ((newLastSlashUrlPos = _url.find_last_of('/', lastSlashUrlPos - 1)) == std::string::npos){
		return (0);
	}
	//какие бывают кейсы?
	// url = "/" location = "/" - OK
	// url = "/index.html" location = "/' - OK
	// url = "www/index.html" location = "/www"
	// url = "www/index.html" location = "/www/"
	// url = "/www/" location = "/www/"
	// URL либо должен указывать на файл либо оканчиваться "/"!!!
	for (std::vector<t_location*>::iterator it = _server->getLocations().begin(); it != _server->getLocations().end(); ++it) {
		std::cout <<"sub str + 1" << _url.substr(0, newLastSlashUrlPos + 1) << std::endl;
		std::cout <<"sub str" << _url.substr(0, newLastSlashUrlPos) << std::endl;
		std::cout <<"url" << (*it)->url << std::endl;
		if (((*it)->url == _url.substr(0, newLastSlashUrlPos + 1)) || ((*it)->url == _url.substr(0, newLastSlashUrlPos))){
			_filePath = (*it)->root + _url.substr(newLastSlashUrlPos);
			_currentLocation = *it;
			return (1);
		}
	}
	return (setUpPathFromUrl(newLastSlashUrlPos));
}

void				RequestHandler::responseError(int errNum){
	std::stringstream buffer;
	std::ifstream file(_server->getErrorPaths()[errNum].c_str());
	std::string serverAnswer = _servAnswer[errNum];
	if (serverAnswer.empty())
		throw Exceptions::NoSuchErrorException();
	if (_server->getErrorPaths()[errNum].empty() || !file ) {
		buffer << _defaultErrors[errNum];
	} else {
		buffer << file.rdbuf();
	}
	_response->setUpBody(buffer);
	_response->setUpHeaders("html");
	_response->setServerAnswer(serverAnswer);
}

void	RequestHandler::responseToGetRequest(){
	std::stringstream buffer;
	std::ifstream file(_filePath.c_str());

	if (!file) {
		responseError(ERR404);
	} else {
		buffer << file.rdbuf();
		_response->setServerAnswer("HTTP/1.1 200 OK\n");
		_response->setUpBody(buffer);
		_response->setUpHeaders(_filePath.substr(_filePath.find_last_of('.') + 1));
	}
}


void	RequestHandler::cgi_handler()
{
	t_info_to_cgi *info;
	info = new t_info_to_cgi;

	info->_answer = _answer;
	info->_body = _body;
	info->_bytesToSend = _bytesToSend;
	info->_currentLocation = _currentLocation;
	info->_filePath = _filePath;
	info->_url = _url;
	info->_cgi_path = _currentLocation->cgi_path;
	info->_response = _response;
	info->_server = _server;
	info->_headers = _headers;
   
   	if (!info->_body.empty()){
       Cgi cgi_obj;
       cgi_obj.cgi_start(info);
       responseAll("HTTP/1.1 200 Ok", cgi_obj.getResponseBody(), _filePath.substr(_filePath.find_last_of('.') + 1));
  	}
   	else
       responseAll("HTTP/1.1 204 No Content", "", _filePath.substr(_filePath.find_last_of('.') + 1));
}

void	RequestHandler::responseToPostRequest()
{
	std::string filename = _currentLocation->root + _url;

	if ((_currentLocation->cgi_path).empty())
	{
		if (if_file_exists(filename) == true)
		{
			std::ofstream file(filename, std::ios::out | std::ios::in | std::ios::trunc);
			if (file.is_open())
			{
                file << _body;
				responseAll("HTTP/1.1 204 No Content", "", "html");
				file.close();
			}
			else
				responseError(ERR403);
		}
		else
		{
			std::ofstream file(filename, std::ios::out | std::ios::in | std::ios::trunc);
			if (file.is_open())
			{
				file << _body;
				responseAll("HTTP/1.1 201 Created", _body, _filePath.substr(_filePath.find_last_of('.') + 1));
				file.close();
			}
		}
	}
	else {
        cgi_handler();
    }
}

void	RequestHandler::responseToDeleteRequest()
{
	std::string         status_message;
	std::stringstream   buffer;

	_body = "";
	std::ifstream file(_filePath);
	if (file.good())
	{
		buffer << file.rdbuf();
		std::string delete_body(buffer.str());

		if (remove(_filePath.c_str()) == 0) {
			if (delete_body.empty() == true)
				status_message = "HTTP/1.1 204 No content";
			else {
				status_message = "HTTP/1.1 200 Ok";
				_body = delete_body;
			}
		} else
			status_message = "HTTP/1.1 202 Accepted";
	}
	else
		status_message = "HTTP/1.1 403 Forbidden";
	responseAll(status_message, "", "html");
}

typedef struct _file
{
	std::string name;
	std::string time;
	std::string size;
	bool        if_dir;
}_file;

void	RequestHandler::autoindex_execution()
{
	std::string current_dir;
	std::string location;
	std::list<std::string> files;
	std::list<std::string> times;
	std::list<std::string> sizes;
	std::list<_file *> _files;

	current_dir = _filePath;
	if (current_dir.back() != '/')
		current_dir = current_dir + "/";
	location = current_dir;
	std::cout << "current location: " << location << std::endl;


	DIR *d;
	struct dirent   *dir;
	struct stat     filestat;
	char no_seconds[10];
	char year[10];
	std::string tmp;
	t_time   my_time;
	char **array;
	std::string time;
	std::string name;
	char file_path[1024];
	_file *file_insatnce;

	d = opendir(location.c_str());
	files.clear();
	sizes.clear();
	times.clear();
	if (d)
	{
		dir = readdir(d);
		while ((dir = readdir(d)) != NULL)
		{
			if(dir->d_name[0] == '.')
				continue;
			file_insatnce = new _file;
			tmp = location + dir->d_name;
			strcpy(file_path, tmp.c_str());
			stat(file_path, &filestat);
			//free(file_path);
			name = dir->d_name;
			if (filestat.st_mode == 16877 & dir->d_name[0] != '.')
				name = name + "/";
			//Спросить у Бориса, как это сделать нормально
			array = ft_split(ctime(&filestat.st_mtime), ' ');
			strcpy(no_seconds, array[3]);
			strcpy(year, array[4]);

			no_seconds[5] = '\0';
			year[4] = '\0';
			my_time.weekday = array[0];
			my_time.month = array[1];
			my_time.day = array[2];
			my_time.time_no_seconds = no_seconds;
			my_time.year = year;

			time = my_time.day;
			if (my_time.day[1] == '\0')
				time = "0" + time;
			time = time + "-";
			time = time + my_time.month;
			time = time + "-";
			time = time + my_time.year;
			time = time + " ";
			time = time + my_time.time_no_seconds;

			if (filestat.st_mode == 16877){
				tmp = "-";
				file_insatnce->if_dir = true;}
			else{
				tmp = std::to_string(filestat.st_size);
				file_insatnce->if_dir = false;}
			file_insatnce->size = tmp;
			file_insatnce->time = time;
			file_insatnce->name = name;
			_files.push_back(file_insatnce);
			for (int i = 0; array[i] != NULL;)
				free(array[i++]);
			free(array);
		}
		closedir(d);
		//if closedir() == -1, throw Exception
	}
	//else Exception

	std::string response;
	std::string line;

	response = "";
	response = response + "<html>\n"
			   + "<head><title>Index of " + _url + "</title></head>\n"
			   + "<body bgcolor=\"white\">\n"
			   + "<h1>Index of " + _url + "</h1><hr><pre>";
	for (std::list<_file *>::iterator it = _files.begin(); it != _files.end(); )
	{
		if ((*it)->if_dir == true) {
			line = "<a href=\"";
			line = line + (*it)->name;
			line = line + "\">";
			line = line + (*it)->name;
			line = line + "</a>";
			int space = ((*it)->name).size();
			for (int space = ((*it)->name).size(); space != 70; space++)
				line = line + " ";
			line = line + (*it)->time;
			for (space = 0; space != 30 - ((*it)->size).size(); space++)
				line = line + " ";
			line = line + (*it)->size;
			it++;
			line += "\n";
			response = response + line;
		}
		else
			it++;
	}
	for (std::list<_file *>::iterator it = _files.begin(); it != _files.end(); )
	{
		if ((*it)->if_dir == false) {
			line = "<a href=\"";
			line = line + (*it)->name;
			line = line + "\">";
			line = line + (*it)->name;
			line = line + "</a>";
			int space = ((*it)->name).size();
			for (int space = ((*it)->name).size(); space != 70; space++)
				line = line + " ";
			line = line + (*it)->time;
			for (space = 0; space != 30 - ((*it)->size).size(); space++)
				line = line + " ";
			line = line + (*it)->size;
			it++;
			line += "\n";
			response = response + line;
		}
		else
			it++;
	}
	for (std::list<_file *>::iterator it = _files.begin(); it != _files.end(); )
	{
		delete(*it++);
	}
	response = response + "</pre><hr></body>\n"
						  "</html>";
	responseAll("HTTP/1.1 200 OK" ,response, "html");
}

void				RequestHandler::responseAll(std::string first_str, std::string body, const std::string &extension){
	std::stringstream buffer;

	buffer << body;
	_response->setUpBody(buffer);
	_response->setUpHeaders(extension);
	_response->setServerAnswer(first_str);
}
