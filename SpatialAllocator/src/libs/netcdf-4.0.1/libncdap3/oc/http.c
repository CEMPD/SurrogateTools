/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
 See the COPYRIGHT file for more information. */

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "ocinternal.h"
#include "ocdebug.h"
#include "http.h"
#include "rc.h"

static size_t WriteFileCallback(void*, size_t, size_t, void*);
static size_t WriteMemoryCallback(void*, size_t, size_t, void*);

struct Fetchdata {
	FILE* stream;
	size_t size;
};

int
file_fetch_url(CURL* curl, char* url, FILE* stream, size_t* sizep)
{
	int stat = OC_NOERR;
	CURLcode cstat = CURLE_OK;
	struct Fetchdata fetchdata;


	/* These four conditionals look for value in four globals set when the
	 * .dodsrc file was read.
	 */
	if (dods_verify) {
		if (set_verify(curl) != OC_NOERR)
			goto fail;
	}
	if (dods_compress) {
		if (set_compression(curl) != OC_NOERR)
			goto fail;
	}
	if (pstructProxy) {
		if (set_proxy(curl, pstructProxy) != OC_NOERR)
			goto fail;
	}
	if (cook) {
		if (set_cookies(curl, cook) != OC_NOERR)
			goto fail;
	}

	if (credentials_in_url(url)) {
		char *result_url = NULL;
		if (extract_credentials(url, &userName, &password, &result_url) != OC_NOERR)
			goto fail;
		url = result_url;
	}

	if (userName && password) {
		if (set_user_password(curl, userName, password) != OC_NOERR)
			goto fail;
	}

	/* Set the URL */
	cstat = curl_easy_setopt(curl, CURLOPT_URL, (void*)url);
	if (cstat != CURLE_OK)
		goto fail;

	/* send all data to this function  */
	cstat = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFileCallback);
	if (cstat != CURLE_OK)
		goto fail;

	/* we pass our file to the callback function */
	cstat = curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&fetchdata);
	if (cstat != CURLE_OK)
		goto fail;

	fetchdata.stream = stream;
	fetchdata.size = 0;
	cstat = curl_easy_perform(curl);
	if (cstat != CURLE_OK)
		goto fail;

	if (stat == OC_NOERR) {
		/* return the file size*/
		if (sizep != NULL)
			*sizep = fetchdata.size;
	}
	return THROW(stat);

	fail: oclog(LOGERR, "curl error: %s", curl_easy_strerror(cstat));
	return THROW(OC_ECURL);
}

int
fetch_url(CURL* curl, char* url, OCbytes* buf)
{
	int stat = OC_NOERR;
	CURLcode cstat = CURLE_OK;
	size_t len;

	/* These conditionals look for value in four globals set when the
	 * .dodsrc file was read.
	 */
	if (dods_verify) {
		if (set_verify(curl) != OC_NOERR)
			goto fail;
	}
	if (dods_compress) {
		if (set_compression(curl) != OC_NOERR)
			goto fail;
	}
	if (pstructProxy) {
		if (set_proxy(curl, pstructProxy) != OC_NOERR)
			goto fail;
	}
	if (cook) {
		if (set_cookies(curl, cook) != OC_NOERR)
			goto fail;
	}

	if (credentials_in_url(url)) {
		char *result_url = NULL;
		if (extract_credentials(url, &userName, &password, &result_url) != OC_NOERR)
			goto fail;
		url = result_url;
	}

	if (userName && password) {
		if (set_user_password(curl, userName, password) != OC_NOERR)
			goto fail;
	}

	/* Set the URL */
	cstat = curl_easy_setopt(curl, CURLOPT_URL, (void*)url);
	if (cstat != CURLE_OK)
		goto fail;

	/* send all data to this function  */
	cstat = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	if (cstat != CURLE_OK)
		goto fail;

	/* we pass our file to the callback function */
	cstat = curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)buf);
	if (cstat != CURLE_OK)
		goto fail;

	cstat = curl_easy_perform(curl);
	if (cstat != CURLE_OK)
		goto fail;

	/* Null terminate the buffer*/
	len = ocblength(buf);
	ocbappend(buf, '\0');
	ocbsetlength(buf, len); /* dont count null in buffer size*/

	return THROW(stat);

fail: oclog(LOGERR, "curl error: %s", curl_easy_strerror(cstat));
	return THROW(OC_ECURL);
}

static size_t
WriteFileCallback(void* ptr, size_t size, size_t nmemb,	void* data)
{
	size_t count;
	struct Fetchdata* fetchdata;
	fetchdata = (struct Fetchdata*) data;
	count = fwrite(ptr, size, nmemb, fetchdata->stream);
	if (count > 0) {
		fetchdata->size += (count * size);
	}
	return count;
}

static size_t
WriteMemoryCallback(void *ptr, size_t size, size_t nmemb,	void *data)
{
	size_t realsize = size * nmemb;
	OCbytes* buf = (OCbytes*) data;
	ocbappendn(buf, ptr, realsize);
	return realsize;
}

#ifdef IGNORE
static void
assembleurl(DAPURL* durl, OCbytes* buf, int what)
{
	encodeurltext(durl->url,buf);
	if(what & WITHPROJ) {
		ocbcat(buf,"?");
		encodeurltext(durl->projection,buf);
	}
	if(what & WITHSEL) encodeurltext(durl->selection,buf);

}

static char mustencode="";
static char hexchars[16] = {
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'a', 'b',
	'c', 'd', 'e', 'f',
};

static void
encodeurltext(char* text, OCbytes* buf)
{
	/* Encode the URL to handle illegal characters */
	len = strlen(url);
	encoded = ocmalloc(len*4+1); /* should never be larger than this*/
	if(encoded==NULL) return;
	p = url; q = encoded;
	while((c=*p++)) {
		if(strchr(mustencode,c) != NULL) {
			char tmp[8];
			int hex1, hex2;
			hex1 = (c & 0x0F);
			hex2 = (c & 0xF0) >> 4;
			tmp[0] = '0'; tmp[1] = 'x';
			tmp[2] = hexchars[hex2]; tmp[3] = hexchars[hex1];
			tmp[4] = '\0';
			ocbcat(buf,tmp);
		} else *q++ = (char)c;
	}

}

#endif

int
curlopen(CURL** curlp)
{
	int stat = OC_NOERR;
	CURLcode cstat;
	CURL* curl;
	/* initialize curl*/
	curl = curl_easy_init();
	if (curl == NULL)
		stat = OC_ECURL;
	else {
		cstat = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
		if (cstat != CURLE_OK)
			stat = OC_ECURL;
		/* some servers don't like requests that are made without a user-agent */
		cstat = curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		if (cstat != CURLE_OK)
			stat = OC_ECURL;
	}
	if (curlp)
		*curlp = curl;
	return THROW(stat);
}

void
curlclose(CURL* curl)
{
	if (curl != NULL)
		curl_easy_cleanup(curl);
}
