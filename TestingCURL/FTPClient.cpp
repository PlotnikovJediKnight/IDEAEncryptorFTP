#include "FTPClient.h"

FTPClient* FTPClient::instance_pointer = nullptr;
FTPClient* FTPClient::getInstance() {
	if (instance_pointer == nullptr) {
		instance_pointer = new FTPClient;
	}
	return instance_pointer;
}

void FTPClient::setServerInformation(const std::string& ip, const std::string& userName, const std::string& pswd, const std::string& url) {
    IP = ip;
    user_name = userName;
    password = pswd;
    URL = url;
}

size_t read_callback(char* ptr, size_t size, size_t nmemb, void* stream) {
    size_t retcode = fread(ptr, size, nmemb, (FILE*)stream);
    curl_off_t nread = (curl_off_t)retcode;
    fprintf(stderr, "*** Have read %" CURL_FORMAT_CURL_OFF_T
        " bytes from file\n", nread);
    return retcode;
}

void createTempFile(char* byte_data, int n_byte) {
    FILE* f = fopen("temp", "wb+");
    if (f) {
        fwrite(byte_data, 1, n_byte, f);
    }
    fclose(f);
}

void closeTempFile(FILE* f) {
    fclose(f);
    remove("temp");
}

void FTPClient::uploadFile(char* byte_data, int n_byte) {
    CURL* curl;
    CURLcode res;
    std::string final_URL = "ftp://" + IP + "/" + URL;
    std::string user_password = user_name + ":" + password;

    curl_global_init(CURL_GLOBAL_ALL);
    createTempFile(byte_data, n_byte);
    FILE* hd_src = fopen("temp", "rb");

    curl = curl_easy_init();
    if (curl) {

            curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
            curl_easy_setopt(curl, CURLOPT_URL, final_URL);
            curl_easy_setopt(curl, CURLOPT_READDATA, (void*)hd_src);
            curl_easy_setopt(curl, CURLOPT_INFILESIZE, (long)n_byte);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl, CURLOPT_USERPWD, user_password);

            res = curl_easy_perform(curl);

        
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        curl_easy_cleanup(curl);
    }
    closeTempFile(hd_src);
    curl_global_cleanup();
}

struct FtpFile {
    const char* filename;
    FILE* stream;
};

static size_t my_fwrite(void* buffer, size_t size, size_t nmemb, void* stream)
{
    struct FtpFile* out = (struct FtpFile*)stream;
    if (!out->stream) {
        out->stream = fopen(out->filename, "wb");
        if (!out->stream)
            return -1;
    }
    return fwrite(buffer, size, nmemb, out->stream);
}

void FTPClient::downloadFile() {
    CURL* curl;
    CURLcode res;
    std::string final_URL = "ftp://" + IP + "/" + URL;
    std::string user_password = user_name + ":" + password;
    struct FtpFile ftpfile = {
      "temp",
      NULL
    };

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl) {
        
        curl_easy_setopt(curl, CURLOPT_URL, final_URL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_USERPWD, user_password);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    }

    if (ftpfile.stream)
        fclose(ftpfile.stream);
    curl_global_cleanup();
}