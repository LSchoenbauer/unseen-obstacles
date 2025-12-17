/*
 * ===============================================================================
 * (c) HTL Leonding
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Licensed under MIT License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the license.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * All trademarks used in this document are property of their respective owners.
 * ===============================================================================
 */

#pragma once

#include <web/http/HttpHeader.h>
#include <web/http/HttpParameter.h>
#include <web/http/HttpResponse.h>
#include <web/http/HttpStatusCodes.h>
#include <web/http/HttpRequestMethods.h>

#include <utils/SharedGlobals.h>
#include <WString.h>
#include <FS.h>
#include <functional>

namespace Web {
namespace Http {

/** The function pointer type for processing HTTP document template placeholders.
 *  Provides a resolved text for the given placeholder name. */
typedef ::std::function<String(const String&)> HttpTemplateProcessor;

/** The function pointer type for feeding data into an HTTP response.
 * Provides the number of bytes copied into the given buffer of given capacity and number of currently valid bytes (filled length).*/
typedef ::std::function<size_t(uint8_t*, size_t, size_t)> HttpResponseSource;

/** The function pointer type for disconnect handlers. */
typedef ::std::function<void(void)> HttpDisconnectHandler;

class HttpRequest {
	public:

		/** Determines whether or not this object refers to a valid request. */
		virtual operator bool() const = 0;

		/** Provides the HTTP version of the request. */
		virtual uint8_t GetHttpVersion() const = 0;
		/** Provides the HTTP method of the request. */
		virtual HttpRequestMethod::Enum GetMethod() const = 0;
		/** Provides the request URL. */
		virtual const String& GetUrl() const = 0;
		/** Provides the host to which this request was sent. */
		virtual const String& GetHost() const = 0;
		/** Provides the content type of the request payload. */
		virtual const String& GetContentType() const = 0;
		/** Provides the length of the request payload. */
		virtual size_t GetContentSize() const = 0;
		/** Determines whether or not this is a multi-part request. */
		virtual bool IsMultipart() const = 0;

		/**
		 * Authenticate this request against the given credential hash.
		 * The credential hash value is calculated as
		 * + 'base64(user:pw)' for basic authentication
		 * + 'user:realm:md5(user:realm:pw)' for digest based authentication.
		 *
		 * @param hash The credential hash this request need to authenticate against.
		 * @return True if the request is authenticated, false otherwise.
		 */
		virtual bool Authenticate(const String& hash) = 0;

		/**
		 * Authenticate this request against the given credentials.
		 * The credential hash value is calculated as 'base64(user:pw)'
		 *
		 * @param hash The credential hash this request need to authenticate against.
		 * @param user The user name that authenticates the request.
		 * @param pw The password or pass-phrase that authenticates the request.
		 * @param realm The realm of the authentication, not set by default.
		 * @param pwIsHash True if the password is hashed, false (default) otherwise.
		 * @return True if the request is authenticated, false otherwise.
		 */
		virtual bool Authenticate(const String& user, const String& pw, const String& realm = Gbl::invalidString, bool pwIsHash =
		        false) = 0;

		/**
		 * Requests the client to authenticate itself for the request.
		 * @param realm The realm of the authentication, not set by default.
		 * @param digestBase True (default) to request digest base authentication, false for basic authentication.
		 */
		virtual void RequestAuthentication(const String& realm = Gbl::invalidString, bool digestBase = true) = 0;

		/** Decodes the given URL. */
		virtual String DecodeUrl(const String& url) const = 0;

		/** Provides the number headers of this request. */
		virtual size_t GetHeaderCount() const = 0;
		/** Determines whether or not this request has a header with the given name. */
		virtual bool HasHeader(const String& name) const = 0;
		/** Provides the header with the given name or 0 if this header is not available. */
		virtual ::std::shared_ptr<HttpHeader> GetHeader(const String& name) const = 0;
		/** Provides the header with the given index or 0 if this header is not available. */
		virtual ::std::shared_ptr<HttpHeader> GetHeader(size_t idx) const = 0;

		/** Provides the number request parameters of this request. */
		virtual size_t GetParameterCount() const = 0;
		/** Determines whether or not this request has a parameter with the given name. */
		virtual bool HasParameter(const String& name, bool postParam = false, bool fileParam = false) const = 0;
		/** Provides the parameter with the given name or 0 if this parameter is not available. */
		virtual ::std::shared_ptr<HttpParameter> GetParameter(const String& name, bool postParam = false,
		        bool fileParam = false) const = 0;
		/** Provides the parameter with the given index or 0 if this header is not available. */
		virtual ::std::shared_ptr<HttpParameter> GetParameter(size_t idx) const = 0;

		/** Provides the number arguments of this request. */
		virtual size_t GetArgCount() const = 0;
		/** Determines whether or not this request has an argument with the given name. */
		virtual bool HasArg(const String& name) const = 0;
		/** Provides the argument name with the given index or an invalid string if this argument is not available. */
		virtual const String& GetArgName(size_t idx) const = 0;
		/** Provides the argument with the given index or an invalid string if this argument is not available. */
		virtual const String& GetArg(size_t idx) const = 0;
		/** Provides the argument with the given name or an invalid string if this argument is not available. */
		virtual const String& GetArg(const String& name) const = 0;

		/** Registers a handler that is invoked when the connection is terminated. */
		virtual void OnDisconnect(HttpDisconnectHandler fct) = 0;

		/** Redirects the request to the given URL. */
		virtual void RedirectToUrl(const String& redirectUrl) = 0;

		/** Creates a new response object based on the given attributes and HTTP status 200 (OK). */
		::std::shared_ptr<HttpResponse> CreateResponse(const String& content, const String& contentType) {
			return CreateResponse(HttpStatusCode::OK, content, contentType);
		}
		/** Creates a new response object based on the given attributes. */
		virtual ::std::shared_ptr<HttpResponse> CreateResponse(int status, const String& content = Gbl::invalidString,
		        const String& contentType = Gbl::invalidString) = 0;
		/** Creates a new response object based on the given attributes. */
		virtual ::std::shared_ptr<HttpResponse> CreateResponse(const String& filePath, const String& contentType, bool asDownload,
		        HttpTemplateProcessor tmplProc = nullptr) = 0;
		/** Creates a new response object based on the given attributes. */
		virtual ::std::shared_ptr<HttpResponse> CreateResponse(const File& file, const String& contentType, bool asDownload =
		        false, HttpTemplateProcessor tmplProc = nullptr) = 0;
		/** Creates a new response object based on the given attributes. */
		virtual ::std::shared_ptr<HttpResponse> CreateResponse(Stream& stream, const String& contentType, size_t contentLength,
		        HttpTemplateProcessor tmplProc = nullptr) = 0;
		/** Creates a new response object based on the given attributes. */
		virtual ::std::shared_ptr<HttpResponse> CreateResponse(const String& contentType, size_t contentLength,
		        HttpResponseSource responseDataCb, HttpTemplateProcessor tmplProc = nullptr) = 0;
		/** Creates a new response object based on the given attributes. */
		virtual ::std::shared_ptr<HttpResponse> CreateChunckedResponse(const String& contentType,
		        HttpResponseSource responseDataCb, HttpTemplateProcessor tmplProc = nullptr) = 0;
		/** Creates a new response object based on data stored in program memory (PROGMEM) with the given attributes with status 200 (OK). */
		::std::shared_ptr<HttpResponse> CreateProgmemResponse(const uint8_t* content, size_t contentLength,
		        const String& contentType, HttpTemplateProcessor tmplProc = nullptr) {
			return CreateProgmemResponse(content, contentLength, contentType, HttpStatusCode::OK, tmplProc);
		}
		/** Creates a new response object based on data stored in program memory (PROGMEM). */
		virtual ::std::shared_ptr<HttpResponse> CreateProgmemResponse(const uint8_t* content, size_t contentLength,
		        const String& contentType, int status, HttpTemplateProcessor tmplProc = nullptr) = 0;
		/** Creates a new response object based on data stored in program memory (PROGMEM) with the given attributes with status 200 (OK). */
		::std::shared_ptr<HttpResponse> CreateProgmemResponse(PGM_P content, const String& contentType,
		        HttpTemplateProcessor tmplProc = nullptr) {
			return CreateProgmemResponse(content, contentType, HttpStatusCode::OK, tmplProc);
		}
		/** Creates a new response object based on data stored in program memory (PROGMEM). */
		virtual ::std::shared_ptr<HttpResponse> CreateProgmemResponse(PGM_P content, const String& contentType, int status,
		        HttpTemplateProcessor tmplProc = nullptr) = 0;

		/** Sends the given response to the client. */
		virtual void SendResponse(::std::shared_ptr<HttpResponse> response) = 0;
		/** Sends the given status code and optional content text to the client. */
		virtual void SendStatus(int status, const String& content = Gbl::invalidString, const String& contentType =
		        Gbl::invalidString) = 0;
		/** Sends the given content text to the client with status 200 (OK). */
		virtual void SendContent(const String& content, const String& contentType) = 0;
		/** Sends the given content stored in program memory (PROGMEM) to the client with status 200 (OK). */
		void SendProgmem(const uint8_t* content, size_t contentLength, const String& contentType, HttpTemplateProcessor tmplProc =
		        nullptr) {
			SendProgmem(content, contentLength, contentType, HttpStatusCode::OK, tmplProc);
		}
		/** Sends the given content stored in program memory (PROGMEM) to the client using the given HTTP status code. */
		virtual void SendProgmem(const uint8_t* content, size_t contentLength, const String& contentType, int status,
		        HttpTemplateProcessor tmplProc = nullptr) = 0;
		/** Sends the given content stored in program memory (PROGMEM) to the client with status 200 (OK). */
		void SendProgmem(PGM_P content, const String& contentType, HttpTemplateProcessor tmplProc = nullptr) {
			SendProgmem(content, contentType, HttpStatusCode::OK, tmplProc);
		}
		/** Sends the given content stored in program memory (PROGMEM) to the client using the given HTTP status code. */
		virtual void SendProgmem(PGM_P content, const String& contentType, int status,
		        HttpTemplateProcessor tmplProc = nullptr) = 0;
		/** Sends the file addressed by the given path relative to document root. */
		virtual void SendFile(const String& path, const String& contentType, bool asDownload = false,
		        HttpTemplateProcessor tmplProc = nullptr) = 0;
		/** Sends the given file. */
		virtual void SendFile(const File& file, const String& contentType, bool asDownload = false,
		        HttpTemplateProcessor tmplProc = nullptr) = 0;
		/** Sends the content of the given stream. */
		virtual void SendStream(Stream& stream, const String& contentType, size_t contentLength, HttpTemplateProcessor tmplProc =
		        nullptr) = 0;
		/** Sends the content of the given in chunks provided by the response data callback function. */
		virtual void SendChunked(const String& contentType, HttpResponseSource responseDataCb, HttpTemplateProcessor tmplProc =
		        nullptr) = 0;

		/** Dumps diagnostic information of this request. */
		virtual void DumpDiagnostics() const = 0;

	protected:
		HttpRequest() {
		}
		virtual ~HttpRequest() {
		}
};

}
} // namespace Web::Http
