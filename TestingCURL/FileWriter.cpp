#include "FileWriter.h"

FileWriter::FileWriter() : FileManipulator(L"") { }
FileWriter::FileWriter(const std::wstring& filePath) : FileManipulator(filePath) { }

char* FileWriter::getCharArray(unsigned char* enc_data, int length, int unnecessary) {
    char* adjusted = new char[length + 1];
    adjusted[0] = (char)unnecessary;
    for (int i = 1; i < length + 1; ++i) {
        adjusted[i] = (char)(enc_data[i - 1] - 128);
    }
    return adjusted;
}

void FileWriter::writeAsEncryptedByteArray(unsigned char* enc_data, int length, int unnecessary) {
    std::ofstream out(file_path, std::ios_base::binary);
    if (out.fail()) {
        throw std::runtime_error("Writing to file failed!");
    }

        char* adjusted = getCharArray(enc_data, length, unnecessary);
        out.write(adjusted, length + 1);
        delete[] adjusted;

    out.close();
}

void FileWriter::writeAsByteArray(unsigned char* enc_data, int length, int unnecessary) {
    std::ofstream out(file_path, std::ios_base::binary);
    if (out.fail()) {
        throw std::runtime_error("Writing to file failed!");
    }

        char* adjusted = new char[length];
        for (int i = 0; i < length - unnecessary; ++i) {
            adjusted[i] = (char)(enc_data[i] - 128);
        }
        out.write(adjusted, length - unnecessary);
        delete[] adjusted;

    out.close();
}