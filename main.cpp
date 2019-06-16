#include <string>

#include <curl/curl.h>
#include <json-c/json.h>

char errorBuffer[CURL_ERROR_SIZE];
bool shouldContinue = true;

int writer(char* data, size_t size, size_t nmemb, std::string* writerData)
{
    if (writerData == nullptr)
    {
        return 0;
    }

    writerData->append(data, size * nmemb);

    return size * nmemb;
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

void json_parse_array(json_object *jobj, char *key) {
    void json_parse(json_object * jobj); /*Forward Declaration*/
    enum json_type type;

    json_object *jarray = jobj; /*Simply get the array*/
    if (key) {
        jarray = json_object_object_get(jobj, key); /*Getting the array if it is a key value pair*/
    }

    int arraylen = json_object_array_length(jarray); /*Getting the length of the array*/
    printf("Array Length: %dn", arraylen);
    int i;
    json_object * jvalue;

    for (i = 0; i < arraylen; i++) {
        jvalue = json_object_array_get_idx(jarray, i); /*Getting the array element at position i*/
        type = json_object_get_type(jvalue);
        if (type == json_type_array) {
            json_parse_array(jvalue, NULL);
        }
        else if (type != json_type_object) {
            printf("value[%d]: ", i);
            //print_json_value(jvalue);
        }
        else {
            json_parse(jvalue);
        }
    }
}

void json_parse(json_object * jobj)
{

}

void parseData(const std::string& buffer)
{
    //Need to use json-c to parse the data

    json_object * jobj = json_tokener_parse(buffer.c_str());
    enum json_type type = json_object_get_type(jobj);


    if (type == json_type_object)
    {
        json_object_object_foreach(jobj, key, val) 
        {
            type = json_object_get_type(val);
            switch (type)
            {
            case json_type_null:
                printf("json_type_nulln");
                break;
            case json_type_boolean:
                printf("json_type_booleann");
                break;
            case json_type_double:
                printf("json_type_doublen");
                break;
            case json_type_int:
                printf("json_type_intn");
                break;
            case json_type_object:
                printf("json_type_objectn");
                break;
            case json_type_array:
                json_parse_array(jobj, key);
                printf("json_type_arrayn");
                break;
            case json_type_string:
                printf("json_type_stringn");
                break;
            }
        }
    }


    printf("type: %u", type);

    
}

void runOnce()
{

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

    while (shouldContinue)
    {
        if (!fetchData(conn, url, buffer))
        {
            exit(EXIT_FAILURE);
        }

        parseData(buffer);

        runOnce();
    }
    
    cleanupConn(conn);

    return 0;
}