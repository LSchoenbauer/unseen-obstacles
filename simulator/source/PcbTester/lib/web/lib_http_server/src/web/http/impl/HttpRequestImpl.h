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

#include <web/http/HttpRequest.h>
#include <utils/ObjectWrapper.h>

#include <memory>
#include <ESPAsyncWebServer.h>

namespace Web {
namespace Http {

class HttpRequestImpl : public HttpRequest, public ObjectWrapper<AsyncWebServerRequest> {
		typedef HttpRequest Base;
		typedef ObjectWrapper<AsyncWebServerRequest> Wrapper;

	public:
		static ::std::shared_ptr<HttpRequest> Create(AsyncWebServerRequest* nativeReq);

		HttpRequestImpl(AsyncWebServerRequest* nativeReq);
		virtual ~HttpRequestImpl();

		inline virtual operator bool() const final {
			return HasWrapped();
		}

		virtual uint8_t GetHttpVersion() const;
		virtual HttpRequestMethod::Enum GetMethod() const;
		virtual const String& GetUrl() const;
		virtual const String& GetHost() const;
		virtual const String& GetContentType() const;
		virtual size_t GetContentSize() const;
		virtual bool IsMultipart() const;

		virtual bool Authenticate(const String& hash);
		virtual bool Authenticate(const String& user, const String& pw, const String& realm = Gbl::invalidString,
		        bool pwIsHash = false);
		virtual void RequestAuthentication(const String& realm = Gbl::invalidString, bool digestBase = true);

		virtual String DecodeUrl(const String& url) const;

		virtual size_t GetHeaderCount() const;
		virtual bool HasHeader(const String& name) const;
		virtual ::std::shared_ptr<HttpHeader> GetHeader(const String& name) const;
		virtual ::std::shared_ptr<HttpHeader> GetHeader(size_t idx) const;

		virtual size_t GetParameterCount() const;
		virtual bool HasParameter(const String& name, bool postParam = false, bool fileParam = false) const;
		virtual ::std::shared_ptr<HttpParameter> GetParameter(const String& name, bool postParam = false,
		        bool fileParam = false) const;
		virtual ::std::shared_ptr<HttpParameter> GetParameter(size_t idx) const;

		virtual size_t GetArgCount() const;
		virtual bool HasArg(const String& name) const;
		virtual const String& GetArgName(size_t idx) const;
		virtual const String& GetArg(size_t idx) const;
		virtual const String& GetArg(const String& name) const;

		virtual void OnDisconnect(HttpDisconnectHandler fct);

		virtual void RedirectToUrl(const String& redirectUrl);

		virtual ::std::shared_ptr<HttpResponse> CreateResponse(int status, const String& content = Gbl::invalidString,
		        const String& contentType = Gbl::invalidString);
		virtual ::std::shared_ptr<HttpResponse> CreateResponse(const String& filePath, const String& contentType,
		        bool asDownload, HttpTemplateProcessor tmplProc = nullptr);
		virtual ::std::shared_ptr<HttpResponse> CreateResponse(const File& file, const String& contentType,
		        bool asDownload = false, HttpTemplateProcessor tmplProc = nullptr);
		virtual ::std::shared_ptr<HttpResponse> CreateResponse(Stream& stream, const String& contentType,
		        size_t contentLength, HttpTemplateProcessor tmplProc = nullptr);
		virtual ::std::shared_ptr<HttpResponse> CreateResponse(const String& contentType, size_t contentLength,
		        HttpResponseSource responseDataCb, HttpTemplateProcessor tmplProc = nullptr);
		virtual ::std::shared_ptr<HttpResponse> CreateChunckedResponse(const String& contentType,
		        HttpResponseSource responseDataCb, HttpTemplateProcessor tmplProc = nullptr);
		virtual ::std::shared_ptr<HttpResponse> CreateProgmemResponse(const uint8_t* content, size_t contentLength,
		        const String& contentType, int status, HttpTemplateProcessor tmplProc = nullptr);
		virtual ::std::shared_ptr<HttpResponse> CreateProgmemResponse(PGM_P content, const String& contentType,
		        int status, HttpTemplateProcessor tmplProc = nullptr);

		virtual void SendResponse(::std::shared_ptr<HttpResponse> response);
		virtual void SendStatus(int status, const String& content = String(), const String& contentType = String());
		virtual void SendContent(const String& content, const String& contentType);
		virtual void SendProgmem(const uint8_t* content, size_t contentLength, const String& contentType, int status,
		        HttpTemplateProcessor tmplProc = nullptr);
		virtual void SendProgmem(PGM_P content, const String& contentType, int status, HttpTemplateProcessor tmplProc =
		        nullptr);
		virtual void SendFile(const String& path, const String& contentType, bool asDownload = false,
		        HttpTemplateProcessor tmplProc = nullptr);
		virtual void SendFile(const File& file, const String& contentType, bool asDownload = false,
		        HttpTemplateProcessor tmplProc = nullptr);
		virtual void SendStream(Stream& stream, const String& contentType, size_t contentLength,
		        HttpTemplateProcessor tmplProc = nullptr);
		virtual void SendChunked(const String& contentType, HttpResponseSource responseDataCb,
		        HttpTemplateProcessor tmplProc = nullptr);

		virtual void DumpDiagnostics() const;

		static HttpRequestMethod::Enum MapMethod(WebRequestMethodComposite nativeMethod);
		static WebRequestMethodComposite MapMethod(HttpRequestMethod::Enum method);
	protected:

	private:

};

}
} // namespace Web::Http
