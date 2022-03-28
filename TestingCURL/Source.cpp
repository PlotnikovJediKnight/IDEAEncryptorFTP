#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include "IDEAEncryptor.h"
#include "FileReader.h"
#include "FileWriter.h"
#include "FTPClient.h"
#define RUSSIAN_LETTERS 1251


void printMenu() {
    std::cout << "==============IDEA ENCRYPTOR v.1.0===============" << std::endl;
    std::cout << "====================Main Menu====================" << std::endl;
    std::cout << "0. Print main menu" << std::endl;
    std::cout << "1. Encrypt file" << std::endl;
    std::cout << "2. Decrypt file" << std::endl;
    std::cout << "3. Exit program" << std::endl;
}

int fillUserKey(word16* ukey) {
    long long entered = -1;
    long long word_count = 0;
    std::string input;
    for (int i = 0; i < 8; i++) {
        try {
            std::cout << "Enter #" << i + 1 << " two-byte part of 128 bit IDEA key:" << std::endl;
            std::getline(std::cin, input);
            entered = std::stoi(input);

            if (entered > 65535 || entered < 0) {
                throw std::invalid_argument("Wrong argument!");
            }

            *ukey++ = entered & 0xFFFF;
            word16* pointer = (ukey - 1);
            *pointer = *pointer >> 8 | *pointer << 8;
            word_count++;
        }
        catch (std::invalid_argument& inv) {
            std::cerr << inv.what() << std::endl;
        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    return word_count;
}

bool chooseFileDialog(wchar_t* fileName) {
    OPENFILENAME ofn;

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = fileName;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 256;
    ofn.lpstrFilter = L"Все файлы для шифрования\0*.*\0";
    ofn.nFilterIndex = 0;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    if (GetOpenFileName(&ofn) == TRUE) {
        return true;
    }
    else {
        return false;
    }
}

bool saveFileDialog(const wchar_t* desc, wchar_t* fileName) {
    OPENFILENAME ofn;

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = fileName;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 256;
    ofn.lpstrFilter = desc;
    ofn.nFilterIndex = 0;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    if (GetSaveFileName(&ofn) == TRUE) {
        return true;
    }
    else {
        return false;
    }

}

int getYesOrNo(string question) {
    std::string input;
    int command = -1;
    std::cout << question << std::endl;

    try {
        std::getline(std::cin, input);
        command = std::stoi(input);
    }
    catch (std::invalid_argument& inv) {
        std::cerr << inv.what() << std::endl;
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    if (command == 1) { return 1; }
    else { return 0; }
}

std::wstring fileNameConversion(std::wstring msg, wchar_t fileName[256]) {
    _setmode(_fileno(stdout), _O_U16TEXT);
    std::wstring fileNameWSTRING{ fileName };
    std::wcout << msg << fileNameWSTRING << std::endl;
    _setmode(_fileno(stdout), _O_TEXT);
    return fileNameWSTRING;
}

void initiateLogDialog(FILE** log_pointer, const wchar_t* desc) {
    wchar_t logName[256];
    if (getYesOrNo((string)"Would you like to create an encryption log-file? (0/1)")) {
        if (saveFileDialog(desc, logName)) {
            *log_pointer = _wfopen(logName, L"w+");
            if (*log_pointer == nullptr)
                std::cerr << "Couldn't open a log-file!" << std::endl;
        }
        else {
            std::cerr << "The log-file for encryption was not chosen!" << std::endl;
        }
    }
}

void send_to_ftp_server(FileWriter& fw, byteptr correctly_formed_byte_array, int length, int unnecessary) {
    SetConsoleCP(RUSSIAN_LETTERS);
    SetConsoleOutputCP(RUSSIAN_LETTERS);

    std::string ip;
    std::cout << "Please, enter FTP-server IP:" << std::endl;
    std::getline(std::cin, ip);

    std::string user_name;
    std::cout << "Please, enter username:" << std::endl;
    std::getline(std::cin, user_name);

    std::string password;
    std::cout << "Please, enter password:" << std::endl;
    std::getline(std::cin, password);

    std::string URL;
    std::cout << "Please, enter URL:" << std::endl;
    std::getline(std::cin, URL);

    FTPClient::getInstance()->setServerInformation(ip, user_name, password, URL);
    char *pointer = fw.getCharArray(correctly_formed_byte_array, length, unnecessary);
    FTPClient::getInstance()->uploadFile(pointer, length + 1);
    delete[] pointer;
}

void download_from_ftp_server() {
    SetConsoleCP(RUSSIAN_LETTERS);
    SetConsoleOutputCP(RUSSIAN_LETTERS);

    std::string ip;
    std::cout << "Please, enter FTP-server IP:" << std::endl;
    std::getline(std::cin, ip);

    std::string user_name;
    std::cout << "Please, enter username:" << std::endl;
    std::getline(std::cin, user_name);

    std::string password;
    std::cout << "Please, enter password:" << std::endl;
    std::getline(std::cin, password);

    std::string URL;
    std::cout << "Please, enter URL:" << std::endl;
    std::getline(std::cin, URL);

    FTPClient::getInstance()->setServerInformation(ip, user_name, password, URL);
    FTPClient::getInstance()->downloadFile();
}

void doEncryptionDialog() {
    word16 userkey[8];
    wchar_t fileName[256]; std::wstring fileNameWSTRING;
    wchar_t outpName[256]; std::wstring outpNameWSTRING;
    FileReader freader;
    FileWriter fwriter;
    int length, unnecessary;
    byteptr correctly_formed_byte_array;
    bool ftp = false;
    FILE* log_pointer = nullptr;

    if (fillUserKey(userkey) != 8) { std::cerr << "128-bit IDEA key was incorrectly entered!" << std::endl; }
    else {
        if (chooseFileDialog(fileName) != true) { std::cerr << "The file for encryption was not chosen!" << std::endl; }
        else {
            fileNameWSTRING = fileNameConversion(L"Chosen file is: ", fileName);
            ftp = getYesOrNo((string)"Would you like to: 0) Save encrypted file locally 1) Send encrypted file to the FTP-server? (0/1)");
            
            initiateLogDialog(&log_pointer, L"Log-файл с шифрованием\0*.txt\0");

            try {
                freader.setFilePath(fileNameWSTRING);
                correctly_formed_byte_array = freader.readAsByteArray(&length, &unnecessary);
            }
            catch (std::runtime_error& r) {
                std::cerr << r.what() << std::endl;
                return;
            }

            IDEAEncryptor::getInstance()->idea_key((byteptr)userkey);
            IDEAEncryptor::getInstance()->idea_enc((byteptr)correctly_formed_byte_array, length / 8, log_pointer);

            if (ftp)
            {
                send_to_ftp_server(fwriter, correctly_formed_byte_array, length, unnecessary);
            }
            else
            {
                if (!saveFileDialog(L"Зашифрованные файлы\0*.enc\0", outpName)) { std::cerr << "The file for encrypted message was not chosen!" << std::endl; }
                else {
                    try {
                        outpNameWSTRING = fileNameConversion(L"Encrypted message will be stored in: ", outpName);
                        fwriter.setFilePath(outpNameWSTRING);
                        fwriter.writeAsEncryptedByteArray(correctly_formed_byte_array, length, unnecessary);
                    }
                    catch (std::runtime_error& r) {
                        std::cerr << r.what() << std::endl;
                        return;
                    }
                }
            }

            if (log_pointer != nullptr) fclose(log_pointer);
            delete[] correctly_formed_byte_array;
        }
    }
}

void doDeencryptionDialog() {
    word16 userkey[8];
    wchar_t fileName[256]; std::wstring fileNameWSTRING;
    wchar_t outpName[256]; std::wstring outpNameWSTRING;
    FileReader freader;
    FileWriter fwriter;
    int length, unnecessary;
    byteptr correctly_formed_byte_array = nullptr;
    bool ftp = false;
    FILE* log_pointer = nullptr;

    if (fillUserKey(userkey) != 8) { std::cerr << "128-bit IDEA key was incorrectly entered!" << std::endl; }
    else {
        ftp = getYesOrNo((string)"Would you like to: 0) Choose encrypted file locally 1) Download encrypted file from the FTP-server? (0/1)");


        if (ftp)
        {
            download_from_ftp_server();
            fileNameWSTRING = L"temp";
        }

        else
        {
            if (chooseFileDialog(fileName) != true) { std::cerr << "The file for encryption was not chosen!" << std::endl; }
            else {
                fileNameWSTRING = fileNameConversion(L"Chosen file is: ", fileName);
            }
        }

        try {
            freader.setFilePath(fileNameWSTRING);
            correctly_formed_byte_array = freader.readAsEncryptedByteArray(&length, &unnecessary);
        }
        catch (std::runtime_error& r) {
            std::cerr << r.what() << std::endl;
            return;
        }

        initiateLogDialog(&log_pointer, L"Log-файл с шифрованием\0*.txt\0");

        IDEAEncryptor::getInstance()->idea_key((byteptr)userkey);
        IDEAEncryptor::getInstance()->idea_dec((byteptr)correctly_formed_byte_array, length / 8, log_pointer);

        if (!saveFileDialog(L"Зашифрованные файлы\0*.enc\0", outpName)) { std::cerr << "The file for encrypted message was not chosen!" << std::endl; }
        else {
            try {
                outpNameWSTRING = fileNameConversion(L"Encrypted message will be stored in: ", outpName);
                fwriter.setFilePath(outpNameWSTRING);
                fwriter.writeAsByteArray(correctly_formed_byte_array, length, unnecessary);
            }
            catch (std::runtime_error& r) {
                std::cerr << r.what() << std::endl;
            }
        }

        if (ftp) remove("temp");
        if (log_pointer != nullptr) fclose(log_pointer);
        delete[] correctly_formed_byte_array;
    }
}

int main(void) {
    std::string input;
    int command = -1;
    printMenu();

    while (true) {
        std::cout << "Enter your command:" << std::endl;
       
        try {
            std::getline(std::cin, input);
            command = std::stoi(input);

            switch (command) {
            case 0: {
                printMenu();
                break;
            }
            case 1: {
                doEncryptionDialog();
                break;
            }
            case 2: {
                doDeencryptionDialog();
                break;
            }
            case 3: {
                exit(0);
                break;
            }
            default:
                throw std::invalid_argument("Wrong argument!");
            }
        }
        catch (std::invalid_argument& inv) {
            std::cerr << inv.what() << std::endl;
        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    return 0;
}