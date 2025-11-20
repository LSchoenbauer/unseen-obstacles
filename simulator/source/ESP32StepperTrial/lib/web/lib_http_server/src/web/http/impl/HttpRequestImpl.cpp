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

#include "HttpRequestImpl.h"
#include "HttpHeaderImpl.h"
#include "HttpParameterImpl.h"
#include "HttpResponseImpl.h"

#include <rfs/RootFileSystem.h>
#include <utils/Log.h>
#include <utils/SharedGlobals.h>

namespace Web {
namespace Http {

::std::shared_ptr<HttpRequest> HttpRequestImpl::Create(AsyncWebServerRequest* nativeReq) {
	return ::std::make_shared<HttpRequestImpl>(nativeReq);
}

HttpRequestImpl::HttpRequestImpl(AsyncWebServerRequest* nativeReq) :
        Base(), Wrapper(nativeReq) {
}

HttpRequestImpl::~HttpRequestImpl() {
}

uint8_t HttpRequestImpl::GetHttpVersion() const {
	return (HasWrapped() ? GetWrapped()->version() : 0);
}

HttpRequestMethod::Enum HttpRequestImpl::GetMethod() const {
	return (HasWrapped() ? HttpRequestImpl::MapMethod(GetWrapped()->method()) : HttpRequestMethod::ANY);
}

const String& HttpRequestImpl::GetUrl() const {
	return (HasWrapped() ? GetWrapped()->url() : Gbl::invalidString);
}

const String& HttpRequestImpl::GetHost() const {
	return (HasWrapped() ? GetWrapped()->host() : Gbl::invalidString);
}

const String& HttpRequestImpl::GetContentType() const {
	return (HasWrapped() ? GetWrapped()->contentType() : Gbl::invalidString);
}

size_t HttpRequestImpl::GetContentSize() const {
	return (HasWrapped() ? GetWrapped()->contentLength() : 0);
}

bool HttpRequestImpl::IsMultipart() const {
	return (HasWrapped() ? GetWrapped()->multipart() : false);
}

bool HttpRequestImpl::Authenticate(const String& hash) {
	return (HasWrapped() ? GetWrapped()->authenticate(hash.c_str()) : false);
}

bool HttpRequestImpl::Authenticate(const String& user, const String& pw, const String& realm, bool pwIsHash) {
	return (HasWrapped() ? GetWrapped()->authenticate(user.c_str(), pw.c_str(), realm.c_str(), pwIsHash) : false);
}

void HttpRequestImpl::RequestAuthentication(const String& realm, bool digestBase) {
	if (HasWrapped()) {
		GetWrapped()->requestAuthentication(realm.c_str(), digestBase);
	}
}

String HttpRequestImpl::DecodeUrl(const String& url) const {
	return (HasWrapped() ? GetWrapped()->urlDecode(url) : Gbl::invalidString);
}

size_t HttpRequestImpl::GetHeaderCount() const {
	return (HasWrapped() ? GetWrapped()->headers() : 0);
}

bool HttpRequestImpl::HasHeader(const String& name) const {
	return (HasWrapped() ? GetWrapped()->hasHeader(name) : false);
}

::std::shared_ptr<HttpHeader> HttpRequestImpl::GetHeader(const String& name) const {
	return HttpHeaderImpl::Create((HasWrapped() ? GetWrapped()->getHeader(name) : nullptr));
}

::std::shared_ptr<HttpHeader> HttpRequestImpl::GetHeader(size_t idx) const {
	return HttpHeaderImpl::Create((HasWrapped() ? GetWrapped()->getHeader(idx) : nullptr));
}

size_t HttpRequestImpl::GetParameterCount() const {
	return (HasWrapped() ? GetWrapped()->params() : 0);
}

bool HttpRequestImpl::HasParameter(const String& name, bool postParam, bool fileParam) const {
	return (HasWrapped() ? GetWrapped()->hasParam(name, postParam, fileParam) : false);
}

::std::shared_ptr<HttpParameter> HttpRequestImpl::GetParameter(const String& name, bool postParam, bool fileParam) const {
	return HttpParameterImpl::Create((HasWrapped() ? GetWrapped()->getParam(name, postParam, fileParam) : nullptr));
}

::std::shared_ptr<HttpParameter> HttpRequestImpl::GetParameter(size_t idx) const {
	return HttpParameterImpl::Create(HasWrapped() ? GetWrapped()->getParam(idx) : nullptr);
}

size_t HttpRequestImpl::GetArgCount() const {
	return (HasWrapped() ? GetWrapped()->args() : 0);
}

bool HttpRequestImpl::HasArg(const String& name) const {
	return (HasWrapped() ? GetWrapped()->hasArg(name ? name.c_str() : 0) : 0);
}

const String& HttpRequestImpl::GetArgName(size_t idx) const {
	return (HasWrapped() ? GetWrapped()->argName(idx) : Gbl::invalidString);
}

const String& HttpRequestImpl::GetArg(size_t idx) const {
	return (HasWrapped() ? GetWrapped()->arg(idx) : Gbl::invalidString);
}

const String& HttpRequestImpl::GetArg(const String& name) const {
	return (HasWrapped() ? GetWrapped()->arg(name) : Gbl::invalidString);
}

void HttpRequestImpl::OnDisconnect(HttpDisconnectHandler fct) {
	if (HasWrapped()) {
		GetWrapped()->onDisconnect(fct);
	}
}

void HttpRequestImpl::RedirectToUrl(const String& redirectUrl) {
	if (HasWrapped()) {
		GetWrapped()->redirect(redirectUrl);
	}
}

::std::shared_ptr<HttpResponse> HttpRequestImpl::CreateResponse(int status, const String& content, const String& contentType) {
	::std::shared_ptr<HttpResponse> rsp;
	if (HasWrapped()) {
		rsp = HttpResponseImpl::Create(GetWrapped()->beginResponse(status, contentType, content));
	}
	return rsp;
}

::std::shared_ptr<HttpResponse> HttpRequestImpl::CreateResponse(const String& filePath, const String& contentType,
        bool asDownload, HttpTemplateProcessor tmplProc) {
	::std::shared_ptr<HttpResponse> rsp;
	if (HasWrapped()) {
		FS* fs = Rfs::RootFileSystem::GetInstance();
		if (fs != 0) {
			rsp = HttpResponseImpl::Create(GetWrapped()->beginResponse(*fs, filePath, contentType, asDownload, tmplProc));
		} else {
			LogError("HttpRequest::SendFile: No RFS available");
		}
	}
	return rsp;
}

::std::shared_ptr<HttpResponse> HttpRequestImpl::CreateResponse(const File& file, const String& contentType, bool asDownload,
        HttpTemplateProcessor tmplProc) {
	::std::shared_ptr<HttpResponse> rsp;
	if (HasWrapped()) {
		rsp = HttpResponseImpl::Create(GetWrapped()->beginResponse(file, file.name(), contentType, asDownload, tmplProc));
	}
	return rsp;
}

::std::shared_ptr<HttpResponse> HttpRequestImpl::CreateResponse(Stream& stream, const String& contentType, size_t contentLength,
        HttpTemplateProcessor tmplProc) {
	::std::shared_ptr<HttpResponse> rsp;
	if (HasWrapped()) {
		rsp = HttpResponseImpl::Create(GetWrapped()->beginResponse(stream, contentType, contentLength, tmplProc));
	}
	return rsp;
}

::std::shared_ptr<HttpResponse> HttpRequestImpl::CreateResponse(const String& contentType, size_t contentLength,
        HttpResponseSource responseDataCb, HttpTemplateProcessor tmplProc) {
	::std::shared_ptr<HttpResponse> rsp;
	if (HasWrapped()) {
		rsp = HttpResponseImpl::Create(GetWrapped()->beginResponse(contentType, contentLength, responseDataCb, tmplProc));
	}
	return rsp;
}

::std::shared_ptr<HttpResponse> HttpRequestImpl::CreateChunckedResponse(const String& contentType,
        HttpResponseSource responseDataCb, HttpTemplateProcessor tmplProc) {
	::std::shared_ptr<HttpResponse> rsp;
	if (HasWrapped()) {
		rsp = HttpResponseImpl::Create(GetWrapped()->beginChunkedResponse(contentType, responseDataCb, tmplProc));
	}
	return rsp;
}

::std::shared_ptr<HttpResponse> HttpRequestImpl::CreateProgmemResponse(const uint8_t* content, size_t contentLength,
        const String& contentType, int status, HttpTemplateProcessor tmplProc) {
	::std::shared_ptr<HttpResponse> rsp;
	if (HasWrapped()) {
		rsp = HttpResponseImpl::Create(GetWrapped()->beginResponse_P(status, contentType, content, contentLength, tmplProc));
	}
	return rsp;
}

::std::shared_ptr<HttpResponse> HttpRequestImpl::CreateProgmemResponse(PGM_P content, const String& contentType, int status,
        HttpTemplateProcessor tmplProc) {
	::std::shared_ptr<HttpResponse> rsp;
	if (HasWrapped()) {
		rsp = HttpResponseImpl::Create(GetWrapped()->beginResponse_P(status, contentType, content, tmplProc));
	}
	return rsp;
}

void HttpRequestImpl::SendResponse(::std::shared_ptr<HttpResponse> response) {
	if (HasWrapped() && response) {
		HttpResponseImpl* rspImpl = static_cast<HttpResponseImpl*>(response.get());
		GetWrapped()->send(rspImpl->GetWrapped());
	}
}

void HttpRequestImpl::SendStatus(int status, const String& content, const String& contentType) {
	if (HasWrapped()) {
		GetWrapped()->send(status, contentType, content);
	}
}

void HttpRequestImpl::SendContent(const String& content, const String& contentType) {
	if (HasWrapped()) {
		GetWrapped()->send(HttpStatusCode::OK, contentType, content);
	}
}

void HttpRequestImpl::SendProgmem(const uint8_t* content, size_t contentLength, const String& contentType, int status,
        HttpTemplateProcessor tmplProc) {
	if (HasWrapped()) {
		GetWrapped()->send_P(status, contentType, content, contentLength, tmplProc);
	}
}

void HttpRequestImpl::SendProgmem(PGM_P content, const String& contentType, int status, HttpTemplateProcessor tmplProc) {
	if (HasWrapped()) {
		GetWrapped()->send_P(status, contentType, content, tmplProc);
	}
}

void HttpRequestImpl::SendFile(const String& filePath, const String& contentType, bool asDownload,
        HttpTemplateProcessor tmplProc) {
	if (HasWrapped()) {
		FS* fs = Rfs::RootFileSystem::GetInstance();
		if (fs != 0) {
			GetWrapped()->send(*fs, filePath, contentType, asDownload, tmplProc);
		} else {
			LogError("HttpRequest::SendFile: No RFS available");
		}
	}
}

void HttpRequestImpl::SendFile(const File& file, const String& contentType, bool asDownload, HttpTemplateProcessor tmplProc) {
	if (HasWrapped()) {
		GetWrapped()->send(file, file.name(), contentType, asDownload, tmplProc);
	}
}

void HttpRequestImpl::SendStream(Stream& stream, const String& contentType, size_t contentLength,
        HttpTemplateProcessor tmplProc) {
	if (HasWrapped()) {
		GetWrapped()->send(stream, contentType, contentLength, tmplProc);
	}
}

void HttpRequestImpl::SendChunked(const String& contentType, HttpResponseSource responseDataCb, HttpTemplateProcessor tmplProc) {
	if (HasWrapped()) {
		GetWrapped()->sendChunked(contentType, responseDataCb, tmplProc);
	}
}

// @formatter:off
void HttpRequestImpl::DumpDiagnostics() const {
	LogInfo("");
	LogInfo("----------------------------");
	LogInfo("HTTP Request (v %d)", GetHttpVersion());
	LogInfo("[%s] %s", HttpRequestMethod::GetName(GetMethod()), GetUrl().c_str());
	LogInfo("Host:    %s", GetHost().c_str());
	LogInfo("Content: '%s' with %d bytes, multi-part: %s", GetContentType().c_str(), GetContentSize(),
			Logger::Dump(IsMultipart()));LogInfo("- %d Headers:", GetHeaderCount());
	for (size_t i = 0; i < GetHeaderCount(); ++i) {
		std::shared_ptr<HttpHeader> header = GetHeader(i);
		LogInfo("  #%02d: %s: %s", i, DumpPtrStr(header.get(), header->GetName().c_str()),
				DumpPtrStr(header, header->GetValue().c_str()));
	}
	LogInfo("- %d Parameters:", GetParameterCount());
	for (size_t i = 0; i < GetParameterCount(); ++i) {
		std::shared_ptr<HttpParameter> param = GetParameter(i);
		LogInfo("  #%02d: %s: %s (%d B), post: %s, file: %s",
				i,
				DumpPtrStr(param.get(), param->GetName().c_str()),
		        DumpPtrStr(param.get(), param->GetValue().c_str()),
		        DumpPtrInt(param.get(), param->GetSize()),
		        DumpPtrStr(param.get(), Logger::Dump(param->IsPost())),
		        DumpPtrStr(param.get(), Logger::Dump(param->IsFile())));
	}
	LogInfo("- %d Arguments:", GetArgCount());
	for (size_t i = 0; i < GetArgCount(); ++i) {
		LogInfo("  #%02d: %s: %s", i, GetArgName(i).c_str(), GetArg(i).c_str());
	}
	LogInfo("----------------------------");
}
// @formatter:on

HttpRequestMethod::Enum HttpRequestImpl::MapMethod(WebRequestMethodComposite nativeMethod) {
	// @formatter:off
	switch(nativeMethod) {
		case WebRequestMethod::HTTP_GET:     return HttpRequestMethod::GET;
		case WebRequestMethod::HTTP_POST:    return HttpRequestMethod::POST;
		case WebRequestMethod::HTTP_PUT:     return HttpRequestMethod::PUT;
		case WebRequestMethod::HTTP_PATCH:   return HttpRequestMethod::PATCH;
		case WebRequestMethod::HTTP_DELETE:  return HttpRequestMethod::DELETE;
		case WebRequestMethod::HTTP_HEAD:    return HttpRequestMethod::HEAD;
		case WebRequestMethod::HTTP_OPTIONS: return HttpRequestMethod::OPTIONS;
	}
// @formatter:on
return	HttpRequestMethod::ANY;
}

WebRequestMethodComposite HttpRequestImpl::MapMethod(HttpRequestMethod::Enum method) {
	// @formatter:off
	switch(method) {
		case HttpRequestMethod::GET: return WebRequestMethod::HTTP_GET;
		case HttpRequestMethod::POST: return WebRequestMethod::HTTP_POST;
		case HttpRequestMethod::PUT: return WebRequestMethod::HTTP_PUT;
		case HttpRequestMethod::PATCH: return WebRequestMethod::HTTP_PATCH;
		case HttpRequestMethod::DELETE: return WebRequestMethod::HTTP_DELETE;
		case HttpRequestMethod::HEAD: return WebRequestMethod::HTTP_HEAD;
		case HttpRequestMethod::OPTIONS: return WebRequestMethod::HTTP_OPTIONS;
		case HttpRequestMethod::ANY: return WebRequestMethod::HTTP_ANY;
	}
// @formatter:on
return	WebRequestMethod::HTTP_ANY;
}

}
} // namespace Web::Http
