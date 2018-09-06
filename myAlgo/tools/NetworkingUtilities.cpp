//
//  Created by Joshua Karbi on 2018-09-01.
//

#include "NetworkingUtilities.hpp"
#include <stdlib.h>
#include <iostream>
#include <vector>

namespace
{
    struct MemoryStruct
    {
        char *memory;
        size_t size;
    };
    
    static size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
    {
        size_t realsize = size * nmemb;
        struct MemoryStruct *mem = (struct MemoryStruct *)userp;
        
        mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
        
        if(mem->memory == NULL) {
            /* out of memory! */
            printf("not enough memory (realloc returned NULL)\n");
            return 0;
        }
        
        memcpy(&(mem->memory[mem->size]), contents, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;
        
        return realsize;
    }
}

namespace tools
{
    std::string simplePost(const std::string& url,
                           const std::string& params,
                           const std::vector<std::string>& headers)
    {
        CURL* curl;
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();
        struct MemoryStruct chunk;
        chunk.memory = (char *)malloc(1);
        chunk.size = 0;
        struct curl_slist *header_list = NULL;
        
        if (curl)
        {
            
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
            struct curl_slist *header_list = NULL;
            
            for (size_t i = 0; i < headers.size(); i++)
            {
                header_list = curl_slist_append(header_list, headers[i].c_str());
            }
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
            
            curl_easy_perform(curl);
            
            curl_easy_cleanup(curl);
        }
        
        curl_slist_free_all(header_list); /* free the list again */
        curl_global_cleanup();
        
        std::string result(chunk.memory, chunk.size);
        std::cout << result << std::endl;
        return result;
    }
}