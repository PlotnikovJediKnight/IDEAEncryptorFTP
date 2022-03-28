#pragma once
#include "FileManipulator.h"

class FileWriter : public FileManipulator {
public:
	FileWriter();
	FileWriter(const std::wstring&);
	char* getCharArray(unsigned char*, int, int);
	void writeAsEncryptedByteArray(unsigned char*, int, int);
	void writeAsByteArray(unsigned char*, int, int);
};