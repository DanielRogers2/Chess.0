#include "net.h"
#include <iostream>
#include <string>

namespace {
    std::string data;
    size_t writeCallback(char* buffer, size_t size, size_t nmemb, void* up) {
        for (int i = 0; i<size*nmemb; i++) {
            data.push_back(buffer[i]);
        }
        return size*nmemb; //tell curl how many bytes we handled
    }
}

std::string net::getFromServer(std::string website) {
    static net instance = net();
    
    curl_easy_setopt(instance.curl, CURLOPT_URL, website.c_str());
    curl_easy_setopt(instance.curl, CURLOPT_WRITEFUNCTION, &writeCallback);
    curl_easy_perform(instance.curl);
    std::string stuff = data;
    data = "";
    return stuff;
}

std::string net::getFromServer(std::string website, std::string params) {
    return "";
}

net::net() {
    curl_global_init(CURL_GLOBAL_ALL);
    this->curl = curl_easy_init();
}

net::~net() {
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}
