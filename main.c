#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"
#include <getopt.h>


struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL) {
        printf("Ошибка: не удалось выделить память\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

char* concat(char *s1, char *s2) {

        size_t len1 = strlen(s1);
        size_t len2 = strlen(s2);                      

        char *result = malloc(len1 + len2 + 1);

        if (!result) {
            fprintf(stderr, "malloc() failed: insufficient memory!\n");
            return NULL;
        }

        memcpy(result, s1, len1);
        memcpy(result + len1, s2, len2 + 1);    

        return result;
    }

int main(int argc, char** argv) {

    if (argc < 2)
    {
	    printf("ERROR! You need to write one argument for city.\n");
	    return 1;
    }
	
    CURL *curl;
    CURLcode res;

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, concat(concat("https://wttr.in/",argv[1]),"?0&format=j1"));
        
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed   : %s\n", curl_easy_strerror(res));
        } else {
            printf("Запрос выполнен успешно!\n");
            printf("Получено %zu байт\n", chunk.size);
	    //printf("Memory.chunk: %s\n", chunk.memory);
        }cJSON* json = cJSON_Parse(chunk.memory);
	if (json != NULL) {
	    cJSON* weather_mass = cJSON_GetObjectItemCaseSensitive(json, "weather");
	    if (weather_mass != NULL && cJSON_IsArray(weather_mass)) {
		cJSON* now_day = cJSON_GetArrayItem(weather_mass, 0);
		//printf("%s", now_day->valuestring);
		if (now_day != NULL) 
		{
		    cJSON* maxTempC = cJSON_GetObjectItemCaseSensitive(now_day, "maxtempC");
	    	    cJSON* minTempC = cJSON_GetObjectItemCaseSensitive(now_day, "mintempC");

		    cJSON* hourly = cJSON_GetObjectItemCaseSensitive(now_day, "hourly");
		    if (maxTempC != NULL  && minTempC != NULL && hourly != NULL && cJSON_IsArray(hourly)  && cJSON_IsString(maxTempC) && cJSON_IsString(minTempC))
		    {
			printf("Temperature is from %sC* to %sC*\n", minTempC->valuestring, maxTempC->valuestring);
			cJSON* hourly_0 = cJSON_GetArrayItem(hourly, 0);
			if (hourly_0 != NULL)
			{
				cJSON* windspeedKmph = cJSON_GetObjectItemCaseSensitive(hourly_0, "windspeedKmph");
				cJSON* winddir16Point = cJSON_GetObjectItemCaseSensitive(hourly_0, "winddir16Point");
				cJSON* weatherDesc = cJSON_GetObjectItemCaseSensitive(hourly_0, "weatherDesc");
				if (windspeedKmph != NULL  && winddir16Point != NULL && weatherDesc != NULL && cJSON_IsArray(weatherDesc)  && cJSON_IsString(windspeedKmph) && cJSON_IsString(winddir16Point))
				{
					printf("Wind speed is %s kmph\n", windspeedKmph->valuestring);
					printf("Wind dir 16 Point is %s\n", winddir16Point->valuestring);
					cJSON* weatherDesc_0 = cJSON_GetArrayItem(weatherDesc, 0);
					if (weatherDesc_0 != NULL)
					{
						cJSON* value = cJSON_GetObjectItemCaseSensitive(weatherDesc_0, "value");
						if (value != NULL && cJSON_IsString(value))
						{
							printf("Weather is %s\n", value->valuestring);
						}
					}
					else
					{
						fprintf(stderr, "Can't find weatherDesc[0]\n");
					}
					//printf("TODO");
				}
				else
				{
					fprintf(stderr, "Not find wind speed or wind dir or weather disciption\n");
				}
			}
			else
			{
				fprintf(stderr, "Not found hourly[0]\n");
			}
		    }
		    else
	            {
			    fprintf(stderr, "Not found maximum temperature or minimum temperature or hourly\n");
		    }	    
		}
		else
		{
			fprintf(stderr, "Not found currently weather\n");
		}
	    }
	    else
	    {
		fprintf(stderr, "Not found weather massive\n");
	    }

	    cJSON_Delete(json);
	}

        curl_easy_cleanup(curl);
        free(chunk.memory);
    } else {
        fprintf(stderr, "Не удалось инициализировать curl\n");
        return 1;
    }

    return 0;
}
