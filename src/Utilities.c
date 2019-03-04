/*
Copyright 2019, Tarek Reyad
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <curl/curl.h>

#define MAX_BASE_URL        1024
#define MAX_DIGEST          64

static size_t WriteDataCallback(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

bool DownloadExecutable(const char *BaseURL, const char *Digest, const char *Path)
{
	if (BaseURL == NULL || Digest == NULL)
		return false;

	if (strlen(BaseURL) > MAX_BASE_URL || strlen(Digest) > MAX_DIGEST)
		return false;

	char URL[MAX_BASE_URL + MAX_DIGEST];
	if (snprintf(URL, sizeof(URL), "%s/%s", BaseURL, Digest) < 0)
		return false;

	CURL *curl;
	FILE *outputFile;
	CURLcode res = CURL_LAST;
	curl = curl_easy_init();
	if (curl)
	{
		outputFile = fopen(Path, "wb");
		curl_easy_setopt(curl, CURLOPT_URL, URL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteDataCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, outputFile);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		fclose(outputFile);
	}

	return (res == CURLE_OK);
}
