#include "../External/cURL/curl.h"
#include <stdexcept>
#include "HTTP.h"



namespace gge { namespace network {
	namespace http {


		void Initialize() {
			curl_global_init(CURL_GLOBAL_ALL);
		}

		static size_t stringwriter(void *ptr, size_t size, size_t nmemb, void *stream) {
			std::string &s=*(std::string*)stream;
			s+=std::string((char*)ptr,size*nmemb);
			return size*nmemb;
		}

		std::string BlockingGetText(const std::string &URL) {
			CURL *curl_handle = curl_easy_init();

			if(!curl_handle)
				throw std::runtime_error("Cannot create curl handle. Initialization missing?");
			curl_version_info_data * vinfo = curl_version_info( CURLVERSION_NOW );
			std::string s;

			curl_easy_setopt(curl_handle, CURLOPT_URL, URL.c_str());
			curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
			curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, stringwriter);
			curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&s);
			curl_easy_setopt(curl_handle, CURLOPT_FAILONERROR, 1L);

			CURLcode res=curl_easy_perform(curl_handle);

			switch(res) {
			case CURLE_OK:
				break;
			case CURLE_FAILED_INIT:
				throw std::runtime_error("curl initialization missing");
			case CURLE_UNSUPPORTED_PROTOCOL:
			case CURLE_URL_MALFORMAT:
				throw Error("Bad URL", Error::BadURL);
			case CURLE_COULDNT_RESOLVE_HOST:
				throw Error("Cannot resolve host name", Error::HostResolutionFailed);
			case CURLE_COULDNT_CONNECT:
				throw Error("Cannot connect to the host", Error::ConnectionFailed);
			case CURLE_REMOTE_ACCESS_DENIED:
				throw Error("Access denied", Error::AccessDenied);
			case CURLE_OUT_OF_MEMORY:
				throw Error("Out of memory", Error::OutOfMemory);
			case CURLE_LOGIN_DENIED:
				throw Error("Login error", Error::LoginError);
			case CURLE_HTTP_RETURNED_ERROR:
				throw Error("Page not found", Error::PageNotFound);
			default:
				throw Error("Unknown error", Error::Unknown);
			}


			curl_easy_cleanup(curl_handle);

			return s;
		}


	}

}}
