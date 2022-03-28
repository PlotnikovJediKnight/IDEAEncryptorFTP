#include "FileReader.h"

FileReader::FileReader() : FileManipulator(L"") { }
FileReader::FileReader(const std::wstring& filePath) : FileManipulator(filePath) { }

unsigned char* FileReader::readAsByteArray(int* const lengthPointer, int* const madeUpBytesPointer) {
    std::ifstream is(file_path, std::ios_base::binary);
    if (is.fail()) {
        throw std::runtime_error("Openning initial file failed!");
    }

    is.seekg(0, is.end);
    int length = is.tellg();
    int notfull_length = length;
    is.seekg(0, is.beg);

    if (length % 8 != 0) {
        *madeUpBytesPointer = (8 - length % 8);
        length += *madeUpBytesPointer;
    }
    else {
        *madeUpBytesPointer = 0;
    }

    char* buffer = new char[length];

        std::cout << "Attempting to read " << notfull_length << " characters... " << std::endl;
        is.read(buffer, notfull_length);


        if (is)
            std::cout << "All characters have been read successfully." << std::endl;
        else
            std::cerr << "Error: only " << is.gcount() << " could be read." << std::endl;

        is.close();

        unsigned char* toReturn = new unsigned char[length];

        for (int i = 0; i < notfull_length; i++) {
            toReturn[i] = (unsigned char)(buffer[i] + 128);
        }

        int buf = *madeUpBytesPointer;
        for (int i = length - 1; buf > 0; buf--, i--) {
            toReturn[i] = 0xFF;
        }
        *lengthPointer = length;

    delete[] buffer;
    return toReturn;
}

unsigned char* FileReader::readAsEncryptedByteArray(int* const lengthPointer, int* const madeUpBytesPointer) {
    std::ifstream is(file_path, std::ios_base::binary);
    if (is.fail()) {
        throw std::runtime_error("Openning encrypted file failed!");
    }

    is.seekg(0, is.end);
    int length = is.tellg();
    is.seekg(0, is.beg);

    int unnecessary = is.get();
    *madeUpBytesPointer = unnecessary;

    char* buffer = new char[length - 1];
    if ((length - 1) % 8 != 0) throw std::runtime_error("This file was probably not encrypted by IDEA. Abort.");

        std::cout << "Attempting to read " << length - 1 << " characters... " << std::endl;
        is.read(buffer, length - 1);


        if (is)
            std::cout << "All characters have been read successfully." << std::endl;
        else
            std::cerr << "Error: only " << is.gcount() << " could be read." << std::endl;

        is.close();

        unsigned char* toReturn = new unsigned char[length - 1];

        for (int i = 0; i < length - 1; i++) {
            toReturn[i] = (buffer[i] + 128) & 0xFF;
        }

        *lengthPointer = length - 1;

    delete[] buffer;
    return toReturn;
}