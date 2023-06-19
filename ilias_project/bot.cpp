#include <curl/curl.h>
#include <iostream>
#include <string>
#include "IRC.hpp"
#include <sstream>
std::string weath(int c)
{
    std::string tmp;
    switch(c)
    {
        case (0):
            tmp += "Clear sky\r\n";
            break;
        case (1):
            tmp += "Mainly clear\r\n";
            break;
        case (2):
            tmp += "Partly cloudy\r\n";
            break;
        case (3):
            tmp += "Overcast\r\n";
            break;
        case (45):
            tmp += "Fog\r\n";
            break;
        case (48):
            tmp += "Depositing rime fog\r\n";
            break;
        case (51):
            tmp += "Drizzle light\r\n";
            break;
        case (53):
            tmp += "Drizzle moderate\r\n";
            break;
        case (55):
            tmp += "Drizzle dense intensity\r\n";
            break;
        case (56):
            tmp += "Freezing drizzle light\r\n";
            break;
        case (57):
            tmp += "Freezing dense intensity\r\n";
            break;
        case (61):
            tmp += "Rain slight\r\n";
            break;
        case (63):
            tmp += "Rain moderate\r\n";
            break;
        case (65):
            tmp += "Rain heavy intensity\r\n";
            break;
        case (66):
            tmp += "Freezing Rain light\r\n";
            break;
        case (67):
            tmp += "Freezing Rain heavy intensity\r\n";
            break;
        case (71):
            tmp += "Snow fall slight\r\n";
            break;
        case (73):
            tmp += "Snow fall moderate\r\n";
            break;
        case (75):
            tmp += "Snow fall heavy intensity\r\n";
            break;
        case (77):
            tmp += "Snow grains\r\n";
            break;
        case (80):
            tmp += "Rain showers slight\r\n";
            break;
        case (81):
            tmp += "Rain showers moderate\r\n";
            break;
        case (82):
            tmp += "Rain showers violent\r\n";
            break;
        case (85):
            tmp += "Snow showers slight\r\n";
            break;
        case (86):
            tmp += "Rain showers heavy\r\n";
            break;
    }
    return tmp;
}
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t totalSize = size * nmemb;
    response->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

std::string api_res(std::string str)
{
    curl_global_init(CURL_GLOBAL_ALL);
    std::string tmp;
    CURL *curl = curl_easy_init();
    if (curl)
    {
        std::string url = "https://api.open-meteo.com/v1/forecast?latitude=32.88&longitude=-6.91&current_weather=true&hourly=temperature_2m,relativehumidity_2m,windspeed_10m";
        curl_easy_setopt(curl, CURLOPT_URL,url.c_str());
        std::string res1;
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl,CURLOPT_WRITEDATA, &res1);
        CURLcode res = curl_easy_perform(curl);
        if(res == CURLE_OK)
        {
            size_t pos;
            size_t pose;
            if (str == "temperature")
            {
                pos = res1.find("{\"temperature\":");
                pose = res1.find(",",pos);
                tmp += "Temperature @ khouribga is : ";
                tmp += res1.substr(pos + 15 ,pose - (pos + 15));
                tmp += "°C\r\n";
            }
            else if(str == "weather")
            {
                std::stringstream ss;
                int op;
                pos = res1.find("\"weathercode\":");
                pose = res1.find(",",pos);
                ss << res1.substr(pos + 14 ,pose - (pos + 14));
                ss >> op;
                tmp += "Weather @ khouribga is : ";
                tmp+= weath(op);
            }
            else
                throw std::runtime_error("BOT_ERROR: invalid choice\r\n");
            curl_easy_cleanup(curl);
        }
        else 
            throw std::runtime_error("BOT_ERROR\r\n");
    curl_global_cleanup();
    }
    else
        throw std::runtime_error("BOT_ERROR\r\n");
    return tmp;
}