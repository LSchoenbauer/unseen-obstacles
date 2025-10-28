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

#include "HttpServerImpl.h"
#include "HttpRequestImpl.h"
#include <web/http/AbstractRestController.h>
#include <web/http/AbstractWebSocketController.h>
#include <utils/Log.h>

namespace Web {
namespace Http {

HttpServerPtr HttpServerImpl::Create(uint16_t port, const String& fsDocRoot) {
	return ::std::make_shared<HttpServerImpl>(port, fsDocRoot);
}

HttpServerImpl::HttpServerImpl(uint16_t port, const String& fsDocRoot) :
        Base(), mNativeServer(port), mDocRoot(fsDocRoot) {
	if (fsDocRoot.endsWith("/")) {
		mDocRoot.substring(0, mDocRoot.length() - 1);
	}
}

HttpServerImpl::~HttpServerImpl() {
}

void HttpServerImpl::SetStaticContentRoot(const String& baseUri, const String& fsRootDir, const String& cacheControl) {
	if (baseUri && fsRootDir) {
		ProcessFile(fsRootDir, [=](fs::FS& fs, const String& absPath) {
			mNativeServer.serveStatic(baseUri.c_str(), fs, absPath.c_str(), cacheControl ? cacheControl.c_str() : 0);
			LogInfo("Configured static content for URI %s to be served from %s", baseUri.c_str(), absPath.c_str());
		}
		);
	} else {
		LogWarn("Static content repository configuration failed");
	}
}

void HttpServerImpl::SetIndexPage(const String& path, const String& contentType) {
	if (path && contentType) {
		ProcessFile(path, [&](fs::FS& fs, const String& absPath) {
			mNativeServer.on("/", HTTP_GET, [=, &fs](AsyncWebServerRequest* request) {
				request->send(fs, absPath, contentType, false);
			});
		}
		);
		LogInfo("Registered index page: %s (%s)", path.c_str(), contentType.c_str());
	} else {
		LogWarn("Index page registration failed");
	}
}

void HttpServerImpl::SetNotFoundErrorPage(const String& path, const String& contentType) {
	if (path && contentType) {
		ProcessFile(path, [&](fs::FS& fs, const String& absPath) {
			mNativeServer.onNotFound([=, &fs](AsyncWebServerRequest* request) {
				request->send(fs, absPath, contentType, false);
			});
		}
		);
		LogInfo("Registered 404-Error page: %s (%s)", path.c_str(), contentType.c_str());
	} else {
		LogWarn("404-Error page registration failed");
	}
}

void HttpServerImpl::AttachController(std::shared_ptr<AbstractRestController> restController) {
	if (restController) {
		AttachTarget(restController);
		restController->OnAttach(*this);
	}
}

void HttpServerImpl::AttachController(std::shared_ptr<AbstractWebSocketController> wsController) {
	if (wsController) {
		AttachTarget(wsController);
		mNativeServer.addHandler(&(wsController->GetNativeWebSocket()));
		wsController->OnAttach(*this);
	}
}

void HttpServerImpl::OnRequest(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler,
        HttpUploadHandler uploadHandler) {
	OnRequest(uri, HttpRequestMethod::ANY, handler, bodyHandler, uploadHandler);
}

void HttpServerImpl::OnGet(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler,
        HttpUploadHandler uploadHandler) {
	OnRequest(uri, HttpRequestMethod::GET, handler, bodyHandler, uploadHandler);
}

void HttpServerImpl::OnPost(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler,
        HttpUploadHandler uploadHandler) {
	OnRequest(uri, HttpRequestMethod::POST, handler, bodyHandler, uploadHandler);
}

void HttpServerImpl::OnDelete(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler,
        HttpUploadHandler uploadHandler) {
	OnRequest(uri, HttpRequestMethod::DELETE, handler, bodyHandler, uploadHandler);
}

void HttpServerImpl::OnPut(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler,
        HttpUploadHandler uploadHandler) {
	OnRequest(uri, HttpRequestMethod::PUT, handler, bodyHandler, uploadHandler);
}

void HttpServerImpl::OnPatch(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler,
        HttpUploadHandler uploadHandler) {
	OnRequest(uri, HttpRequestMethod::PATCH, handler, bodyHandler, uploadHandler);
}

void HttpServerImpl::OnHead(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler,
        HttpUploadHandler uploadHandler) {
	OnRequest(uri, HttpRequestMethod::HEAD, handler, bodyHandler, uploadHandler);
}

void HttpServerImpl::OnOptions(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler,
        HttpUploadHandler uploadHandler) {
	OnRequest(uri, HttpRequestMethod::OPTIONS, handler, bodyHandler, uploadHandler);
}

void HttpServerImpl::OnRequest(const String& uri, HttpRequestMethod::Enum httpMethod, HttpRequestHandler handler,
        HttpRequestBodyHandler bodyHandler, HttpUploadHandler uploadHandler) {
	if (uri) {
		AsyncCallbackWebHandler* nativeHandler = new AsyncCallbackWebHandler();
		nativeHandler->setUri(uri);
		if (httpMethod != HttpRequestMethod::ANY) {
			nativeHandler->setMethod(HttpRequestImpl::MapMethod(httpMethod));
		}
		if (handler) {
			nativeHandler->onRequest([handler](AsyncWebServerRequest* req) {
				handler(HttpRequestImpl::Create(req));
			});
		}
		if (bodyHandler) {
			nativeHandler->onBody(
			        [bodyHandler](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t index, size_t total) {
				        bodyHandler(HttpRequestImpl::Create(req), data, len, index, total);
			        });
		}
		if (uploadHandler) {
			nativeHandler->onUpload(
			        [uploadHandler](AsyncWebServerRequest* req, const String& filename, size_t index, uint8_t* data,
			                size_t len, bool fin) {
				        uploadHandler(HttpRequestImpl::Create(req), filename, index, data, len, fin);
			        });
		}
		mNativeServer.addHandler(nativeHandler);
		LogInfo("Registered %s handler for URI %s", HttpRequestMethod::GetName(httpMethod), uri.c_str());
	} else {
		LogWarn("Invalid URI: No handler was registered");
	}
}

void HttpServerImpl::Start() {
	mNativeServer.begin();
	LogInfo("Started");
}

void HttpServerImpl::Stop() {
	mNativeServer.end();
	LogInfo("Stopped");
}

void HttpServerImpl::ProcessFile(const String& relPath, FileProcessorFn processorFn) {
	FS* fs = Rfs::RootFileSystem::GetInstance();
	if (fs != 0) {
		String absPath(mDocRoot);
		absPath.concat(relPath);
		LogDbg("Serving file '%s'", absPath.c_str());
		processorFn(*fs, absPath);
	} else {
		LogError("Serving file content failed: RFS not available");
	}
}

}
} // namespace Web::Http
