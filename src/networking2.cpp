#include <curl/curl.h>

char *loadFromUrl(char *url, int *outBytesNum=NULL, char *postStr=NULL, char *headerStr1=NULL, char *headerStr2=NULL);

size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
/// FUNCTIONS ^

struct CURLMemoryStruct {
  char *memory;
  size_t size;
};

char *loadFromUrl(char *url, int *outBytesNum, char *postStr, char *headerStr1, char *headerStr2) {
	CURL *curlHandle;
	CURLMemoryStruct chunk;
	chunk.memory = (char *)malloc(1);  
	chunk.size = 0;

	curlHandle = curl_easy_init();
	if (!curlHandle) {
		logf("Curl failed to init\n");
		if (outBytesNum) *outBytesNum = 0;
		return NULL;
	}

	curl_easy_setopt(curlHandle, CURLOPT_URL, url);
	curl_easy_setopt(curlHandle, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
	curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &chunk);
	curl_easy_setopt(curlHandle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	if (postStr) curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, postStr);
	if (headerStr1) {
		struct curl_slist *headerChunk = NULL;
		headerChunk = curl_slist_append(headerChunk, "Accept:");
		headerChunk = curl_slist_append(headerChunk, headerStr1);
		if (headerStr2) headerChunk = curl_slist_append(headerChunk, headerStr2);
		curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, headerChunk);
	}

	CURLcode res = curl_easy_perform(curlHandle);
	if (res != CURLE_OK) {
		logf("CURL error: %s\n", curl_easy_strerror(res));
		if (outBytesNum) *outBytesNum = 0;
		return NULL;
	}

	curl_easy_cleanup(curlHandle);

	if (outBytesNum) *outBytesNum = chunk.size;
	return chunk.memory;
}

size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct CURLMemoryStruct *mem = (struct CURLMemoryStruct *)userp;

  char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
    logf("error: not enough memory\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}
