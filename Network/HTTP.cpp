#include "../Engine/OS.h"

#ifdef WIN32
#	undef APIENTRY
#	undef WINGDIAPI
#endif


#include "../External/cURL/curl.h"
#include <stdexcept>
#include <cstring>
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

		static size_t streamwriter(void *ptr, size_t size, size_t nmemb, void *stream) {
			std::ostream &s=*(std::ostream*)stream;
			s.write((char*)ptr,size*nmemb);
			if(s.bad())
				return 0;
			else
				return size*nmemb;
		}

		static size_t vectorwriter(void *ptr, size_t size, size_t nmemb, void *vec) {
			std::vector<Byte> &s=*(std::vector<Byte>*)vec;
			
			auto prevsize=s.size();
			s.resize(s.size()+size*nmemb);
			std::memcpy(&s[prevsize], ptr, size*nmemb);

			return size*nmemb;
		}

		static Error translateerror(CURLcode res) {
			Error err;

			switch(res) {
			case CURLE_OK:
				break;
			case CURLE_UNSUPPORTED_PROTOCOL:
			case CURLE_URL_MALFORMAT:
				err=Error("Bad URL", Error::BadURL);
			case CURLE_COULDNT_RESOLVE_HOST:
				err=Error("Cannot resolve host name", Error::HostResolutionFailed);
			case CURLE_COULDNT_CONNECT:
				err=Error("Cannot connect to the host", Error::ConnectionFailed);
			case CURLE_REMOTE_ACCESS_DENIED:
				err=Error("Access denied", Error::AccessDenied);
			case CURLE_OUT_OF_MEMORY:
				err=Error("Out of memory", Error::OutOfMemory);
			case CURLE_LOGIN_DENIED:
				err=Error("Login error", Error::LoginError);
			case CURLE_HTTP_RETURNED_ERROR:
				err=Error("Page not found", Error::PageNotFound);
			default:
				err=Error("Unknown error", Error::Unknown);
			}

			return err;
		}

		std::string BlockingGetText(const std::string &URL) {
			CURL *curl_handle = curl_easy_init();

			if(!curl_handle)
				throw std::runtime_error("Cannot create curl handle. Initialization missing?");

			std::string s;

			curl_easy_setopt(curl_handle, CURLOPT_URL, URL.c_str());
			curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
			curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, stringwriter);
			curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&s);
			curl_easy_setopt(curl_handle, CURLOPT_FAILONERROR, 1L);

			CURLcode res=curl_easy_perform(curl_handle);

			Error err=translateerror(res);

			if(err.error)
				throw err;

			curl_easy_cleanup(curl_handle);

			return s;
		}

		int threadfncall nonblockingop(void *data) {
			Nonblocking *me=(Nonblocking*)data;
			CURLcode res=curl_easy_perform(me->curl);

			me->err=translateerror(res);
			me->isrunning=false;

			return 0;
		}

		void Nonblocking::GetText(const std::string &URL) {
			tempstr="";

			if(isrunning) 
				throw std::runtime_error("Running another task at the moment.");

			isrunning=true;
			currentoperation=Text;


			curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, stringwriter);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&tempstr);
			curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

			os::RunInNewThread(&nonblockingop, this);
		}

		void Nonblocking::GetFile(const std::string &URL, const std::string &filename) {
			tempfile.open(filename, std::ios::binary);

			if(isrunning)
				throw std::runtime_error("Running another task at the moment.");

			isrunning=true;
			currentoperation=File;

			stream(URL, tempfile);
		}

		void Nonblocking::GetStream(const std::string &URL, std::ostream &stream) {
			if(isrunning)
				throw std::runtime_error("Running another task at the moment.");

			isrunning=true;
			currentoperation=Stream;

			this->stream(URL, stream);
		}

		void Nonblocking::stream(const std::string &URL, std::ostream &stream) {

			curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &streamwriter);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&stream);
			curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

			os::RunInNewThread(&nonblockingop, this);
		}

		void Nonblocking::GetData(const std::string &URL) {
			if(isrunning) 
				throw std::runtime_error("Running another task at the moment.");

			if(currentoperation==Data) {
				gge::utils::CheckAndDelete(tempvec);
			}

			isrunning=true;
			currentoperation=Data;

			tempvec=new std::vector<Byte>;

			getdata(URL, *tempvec);
		}
		void Nonblocking::GetData(const std::string &URL, std::vector<Byte> &vec) {
			if(isrunning) 
				throw std::runtime_error("Running another task at the moment.");

			if(currentoperation==Data) {
				gge::utils::CheckAndDelete(tempvec);
			}

			isrunning=true;
			currentoperation=OwnedData;

			tempvec=&vec;

			getdata(URL, vec);
		}

		void Nonblocking::getdata(const std::string &URL, std::vector<Byte> &vec) {

			curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, vectorwriter);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&vec);
			curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

			os::RunInNewThread(&nonblockingop, this);
		}

		Nonblocking::Nonblocking() : isrunning(false),
			TextTransferComplete("Text", this),
			DataTransferComplete("Data", this),
			FileTransferComplete("File", this) 
		{
			curl=curl_easy_init();

			token=gge::Main.BeforeRenderEvent.RegisterLambda([&]() {
				if(currentoperation!=None && !isrunning) {
					if(err.error!=0) {
						TransferError(err);
					}
					else {
						switch(currentoperation) {
						case Text:
							TextTransferComplete(tempstr);
							break;
						case Data:  
							DataTransferComplete(*tempvec);
							if(!isrunning) {
								gge::utils::CheckAndDelete(tempvec);
							}
							break;
						case OwnedData:
							DataTransferComplete(*tempvec);
							tempvec=NULL;
							break;
						case File:
							tempfile.close();
							FileTransferComplete();
							break;
						case Stream:
							FileTransferComplete();
							break;
						}
					}
				}
			});

			if(!curl)
				throw std::runtime_error("Cannot create curl handle. Initialization missing?");
		}


	}


}}
