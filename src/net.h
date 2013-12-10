#ifndef NET_H
#define NET_H

#include <string>
#include <curl/curl.h>

class net {
public:
    static std::string getFromServer(std::string site);
    static std::string getFromServer(std::string site, std::string parameters);
    ~net();
private:
    static CURL* initialize();
    CURL* curl;
    static net* instance;
    net();
};

#endif // NET_H
