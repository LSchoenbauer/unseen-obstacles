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

#include <web/http/HttpServer.h>
#include <web/http/HttpRequestMethods.h>
#include <rfs/RootFileSystem.h>
#include <utils/SharedGlobals.h>

#include <ESPAsyncWebServer.h>
#include <memory>

namespace Web {
namespace Http {

/** The function pointer type for processing functionality using RFS and paths relative to document root. */
typedef ::std::function<void(fs::FS&, const String&)> FileProcessorFn;

class AbstractRestController;
class AbstractWebSocketController;

/**
 * The HTTP Server implementation
 */
class HttpServerImpl : public HttpServer {
		typedef HttpServer Base;
	public:
		static HttpServerPtr Create(uint16_t port, const String& fsDocRoot);

		HttpServerImpl(uint16_t port, const String& fsDocRoot);
		virtual ~HttpServerImpl();

		virtual void SetStaticContentRoot(const String& baseUri, const String& fsRootDir, const String& cacheControl =
		        Gbl::invalidString) override;
		virtual void SetIndexPage(const String& path, const String& contentType) override;
		virtual void SetNotFoundErrorPage(const String& path, const String& contentType) override;

		virtual void AttachController(std::shared_ptr<AbstractRestController> restController) override;
		virtual void AttachController(std::shared_ptr<AbstractWebSocketController> wsController) override;

		virtual void OnRequest(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler =
		        nullptr, HttpUploadHandler uploadHandler = nullptr) override;
		virtual void OnGet(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler = nullptr,
		        HttpUploadHandler uploadHandler = nullptr) override;
		virtual void OnPost(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler = nullptr,
		        HttpUploadHandler uploadHandler = nullptr) override;
		virtual void OnDelete(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler =
		        nullptr, HttpUploadHandler uploadHandler = nullptr) override;
		virtual void OnPut(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler = nullptr,
		        HttpUploadHandler uploadHandler = nullptr) override;
		virtual void OnPatch(const String& uri, HttpRequestHandler handler,
		        HttpRequestBodyHandler bodyHandler = nullptr, HttpUploadHandler uploadHandler = nullptr) override;
		virtual void OnHead(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler = nullptr,
		        HttpUploadHandler uploadHandler = nullptr) override;
		virtual void OnOptions(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler =
		        nullptr, HttpUploadHandler uploadHandler = nullptr) override;

		virtual void Start();
		virtual void Stop();

	protected:
		/** Registers the given handler for processing requests to the given URI. */
		void OnRequest(const String& uri, HttpRequestMethod::Enum httpMethod, HttpRequestHandler handler,
		        HttpRequestBodyHandler bodyHandler, HttpUploadHandler uploadHandler);
		/** Processes the given processor function which requires file system and a path relative to document root. */
		void ProcessFile(const String& relPath, FileProcessorFn processorFn);

	private:
		AsyncWebServer mNativeServer;
		String mDocRoot;
};

}
} // namespace Web::Http
