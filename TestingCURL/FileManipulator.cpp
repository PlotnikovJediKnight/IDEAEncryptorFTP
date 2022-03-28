#include "FileManipulator.h"

FileManipulator::FileManipulator() : file_path(L"") { }
FileManipulator::FileManipulator(const std::wstring& filePath) : file_path(filePath) { }

void FileManipulator::setFilePath(const std::wstring& filePath) {
    file_path = filePath;
}

std::wstring FileManipulator::getFilePath() {
    return file_path;
}