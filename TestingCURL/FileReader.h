#pragma once
#include "FileManipulator.h"

class FileReader : public FileManipulator {
public:
	FileReader();
	FileReader(const std::wstring&);
	unsigned char* readAsByteArray(int* const, int* const);
	unsigned char* readAsEncryptedByteArray(int* const, int* const);
};