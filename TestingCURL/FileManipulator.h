#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <exception>

class FileManipulator {
protected:
	std::wstring file_path;
public:
	FileManipulator();
	FileManipulator(const std::wstring&);
	void setFilePath(const std::wstring&);
	std::wstring getFilePath();
};
