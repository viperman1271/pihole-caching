#include <chrono>
#include <string>
#include <thread>

#include <curl/curl.h>
#include <json-c/json.h>

#include "objectmodel/queryResult.h"
#include "objectmodel/queryInfo.h"

#ifdef _WINDOWS_
#include <windns.h>
#endif // _WINDOWS_

char errorBuffer[CURL_ERROR_SIZE];
bool shouldContinue = true;

int writer(char* data, size_t size, size_t nmemb, std::string* writerData)
{
    if (writerData == nullptr)
    {
        return 0;
    }

    writerData->append(data, size * nmemb);

    return static_cast<int>(size * nmemb);
}

void cleanupConn(CURL*& conn)
{
    curl_easy_cleanup(conn);
    conn = nullptr;
}

bool curlInit(CURL*& conn, const std::string& url, std::string& buffer)
{
    CURLcode code;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    conn = curl_easy_init();

    if (conn == nullptr)
    {
        fprintf(stderr, "Failed to create CURL connection\n");
        exit(EXIT_FAILURE);
    }

    code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer);
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set error buffer [%d]\n", code);
        return false;
    }

    code = curl_easy_setopt(conn, CURLOPT_URL, url.c_str());
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set URL [%s]\n", errorBuffer);
        return false;
    }

    code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L);
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set redirect option [%s]\n", errorBuffer);
        return false;
    }

    code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer);
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set writer [%s]\n", errorBuffer);
        return false;
    }

    code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, &buffer);
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set write data [%s]\n", errorBuffer);
        return false;
    }

    return true;
}

bool fetchData(CURL* conn, const std::string& url, std::string& buffer)
{
    buffer.clear();
    if (curl_easy_perform(conn) != CURLE_OK)
    {
        cleanupConn(conn);
        fprintf(stderr, "Failed to get '%s' [%s]\n", url.c_str(), errorBuffer);
        return false;
    }

    return true;
}

void runOnce(queryInfo& queryInfo)
{
    time_t currentTime;
    time(&currentTime);

    int queriedTotal = 0;
    for (const queryInfo::domainAndHitInfo& domainAndHitInfo : queryInfo.sortedDomainNames)
    {
        auto iter = queryInfo.lastForwardPerDomain.find(domainAndHitInfo.domain);
        if (iter == queryInfo.lastForwardPerDomain.end() || difftime(currentTime, iter->second) > 3600) //Assume a TTL of 1 hour
        {
            queryInfo.lastForwardPerDomain[domainAndHitInfo.domain] = currentTime;
#ifdef _WINDOWS_
            DNS_STATUS dnsQueryResult = DnsQuery(domainAndHitInfo.domain.c_str(), DNS_TYPE_A, DNS_QUERY_BYPASS_CACHE, nullptr, nullptr, nullptr);
            if (dnsQueryResult == 0)
            {
                ++queriedTotal;
            }
#elif defined(_linux_)
            res_query(argv[1], ns_c_any, ns_t_a, nsbuf, sizeof(nsbuf));
#else
#error "Not implemented"
#endif

            if (queriedTotal >= 250) // Hardcoded limit for the time being
            {
                break;
            }
        }
    }

    std::this_thread::sleep_for(std::chrono::minutes(5));
}

int main(int argc, char *argv[])
{
    CURL* conn = nullptr;
    const std::string url = "http://raspberrypi.mikefilion.org/admin/api.php?getAllQueries&auth=2c1d7da9c4245b90c24489dc9d73b61a24e2410b5373f619e0250a3e3a66d0a8";
    std::string buffer;

    if (!curlInit(conn, url, buffer))
    {
        fprintf(stderr, "Connection initializion failed\n");
        exit(EXIT_FAILURE);
    }

    queryInfo queryInfo;
    time_t lastFetchDataTime = {};
    while (shouldContinue)
    {
        time_t now;
        time(&now);

        if(difftime(now, lastFetchDataTime) > 3600) //Only once an hour will we update the data from pi-hole
        {
            time(&lastFetchDataTime);
            if (!fetchData(conn, url, buffer))
            {
                exit(EXIT_FAILURE);
            }

            queryInfo.parse(buffer);
        }

        runOnce(queryInfo);
    }
    
    cleanupConn(conn);

    return 0;
}