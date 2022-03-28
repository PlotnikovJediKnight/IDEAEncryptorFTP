#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#ifdef WIN32
	#include <io.h>
#else
	#include <unistd.h>
#endif

class FTPClient {
private:
	static FTPClient* instance_pointer;
	std::string IP;
	std::string user_name;
	std::string password;
	std::string URL;
public:
	static FTPClient* getInstance();
	void setServerInformation(const std::string&, const std::string&, const std::string&, const std::string&);
	void uploadFile(char*, int);
	void downloadFile();
};
