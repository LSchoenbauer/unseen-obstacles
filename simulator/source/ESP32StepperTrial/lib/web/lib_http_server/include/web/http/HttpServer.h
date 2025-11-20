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
#include <appfw/AppEventHub.h>

#include <WString.h>
#include <functional>
#include <memory>

namespace Web {
namespace Http {

using namespace AppFw;

class HttpServer;
typedef ::std::shared_ptr<HttpServer> HttpServerPtr;

/** The function pointer type for request handlers. */
typedef ::std::function<void(::std::shared_ptr<HttpRequest>)> HttpRequestHandler;
/** The function pointer type for file upload handlers. */
typedef ::std::function<
        void(::std::shared_ptr<HttpRequest>, const String& filename, size_t index, uint8_t* data, size_t len, bool fin)> HttpUploadHandler;
/** The function pointer type for request body handlers. */
typedef ::std::function<void(::std::shared_ptr<HttpRequest>, uint8_t* data, size_t len, size_t index, size_t total)> HttpRequestBodyHandler;

class AbstractRestController;
class AbstractWebSocketController;

/**
 * The HTTP Server
 */
class HttpServer : public AppEventHub {
	public:

		/**
		 * Creates a new server instance with the given file system partition and document root path.
		 * This method configures the server to listen on port '80'.
		 * @param fsDocRoot The path of the document root directory within the root file system.
		 */
		static HttpServerPtr Create(const String& fsDocRoot) {
			return HttpServer::Create(80, fsDocRoot);
		}

		/**
		 * Creates a new server instance with the given file system partition and document root path.
		 * @param port The port on which the web server is listening.
		 * @param fsDocRoot The path of the document root directory within the root file system.
		 */
		static HttpServerPtr Create(uint16_t port, const String& fsDocRoot);

		/**
		 * Specifies the location for serving static content.
		 *
		 * Note: Requires the root file system with mounted partitions.
		 *
		 * @param baseUri The base URI of static content to serve.
		 * @param fsRootDir The root directory relative to fsDocRoot applied to.
		 * @param cacheControl The HTTP cache control string.
		 * the constructor within the file system partition serving the static content.
		 */
		virtual void SetStaticContentRoot(const String& baseUri, const String& fsRootDir, const String& cacheControl =
		        Gbl::invalidString) = 0;

		/**
		 * Specifies the web page to be served for root URI (/).
		 * The method assumes a content type of 'text/html'.
		 * @param path The path of the page within the applied file system.
		 */
		void SetIndexPage(const String& path) {
			SetIndexPage(path, "text/html");
		}

		/**
		 * Specifies the web page to be served for root URI (/) and the content type of that page.
		 * @param path The path of the page within the applied file system.
		 * @param contentType The content type of the index page.
		 */
		virtual void SetIndexPage(const String& path, const String& contentType) = 0;

		/**
		 * Specifies the web page to be served if the requested resource is not available.
		 * The method assumes a content type of 'text/html'.
		 * @param path The path of the page within the applied file system.
		 */
		void SetNotFoundErrorPage(const String& path) {
			SetNotFoundErrorPage(path, "text/html");
		}

		/**
		 * Specifies the web page to be served if the requested resource is not available.
		 * @param path The path of the page within the applied file system.
		 * @param contentType The content type of the index page.
		 */
		virtual void SetNotFoundErrorPage(const String& path, const String& contentType) = 0;

		/**
		 * Attaches the given REST controller to this server instance.
		 * @param restController The controller to attach.
		 */
		virtual void AttachController(std::shared_ptr<AbstractRestController> restController) = 0;

		/**
		 * Attaches the given WebSocket controller to this server instance.
		 * @param wsController The controller to attach.
		 */
		virtual void AttachController(std::shared_ptr<AbstractWebSocketController> wsController) = 0;

		/**
		 * Registers a request handler for requests of any HTTP method targeting the given URI.
		 * @param uri The URI for which the handler is invoked.
		 * @param handler The handler function
		 * @param bodyHandler The optional function for handling the request body.
		 * @param uploadHandler The optional function for handling file uploads.
		 */
		virtual void OnRequest(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler = 0,
		        HttpUploadHandler uploadHandler = 0) = 0;
		/**
		 * Registers a request handler for HTTP GET requests targeting the given URI.
		 * @param uri The URI for which the handler is invoked.
		 * @param handler The handler function
		 * @param bodyHandler The optional function for handling the request body.
		 * @param uploadHandler The optional function for handling file uploads.
		 */
		virtual void OnGet(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler = 0,
		        HttpUploadHandler uploadHandler = 0) = 0;

		/**
		 * Registers a request handler for HTTP POST requests targeting the given URI.
		 * @param uri The URI for which the handler is invoked.
		 * @param handler The handler function
		 * @param bodyHandler The optional function for handling the request body.
		 * @param uploadHandler The optional function for handling file uploads.
		 */
		virtual void OnPost(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler = 0,
		        HttpUploadHandler uploadHandler = 0) = 0;

		/**
		 * Registers a request handler for HTTP DELETE requests targeting the given URI.
		 * @param uri The URI for which the handler is invoked.
		 * @param handler The handler function
		 * @param bodyHandler The optional function for handling the request body.
		 * @param uploadHandler The optional function for handling file uploads.
		 */
		virtual void OnDelete(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler = 0,
		        HttpUploadHandler uploadHandler = 0) = 0;

		/**
		 * Registers a request handler for HTTP PUT requests targeting the given URI.
		 * @param uri The URI for which the handler is invoked.
		 * @param handler The handler function
		 * @param bodyHandler The optional function for handling the request body.
		 * @param uploadHandler The optional function for handling file uploads.
		 */
		virtual void OnPut(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler = 0,
		        HttpUploadHandler uploadHandler = 0) = 0;

		/**
		 * Registers a request handler for HTTP PATCH requests targeting the given URI.
		 * @param uri The URI for which the handler is invoked.
		 * @param handler The handler function
		 * @param bodyHandler The optional function for handling the request body.
		 * @param uploadHandler The optional function for handling file uploads.
		 */
		virtual void OnPatch(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler = 0,
		        HttpUploadHandler uploadHandler = 0) = 0;

		/**
		 * Registers a request handler for HTTP HEAD requests targeting the given URI.
		 * @param uri The URI for which the handler is invoked.
		 * @param handler The handler function
		 * @param bodyHandler The optional function for handling the request body.
		 * @param uploadHandler The optional function for handling file uploads.
		 */
		virtual void OnHead(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler = 0,
		        HttpUploadHandler uploadHandler = 0) = 0;

		/**
		 * Registers a request handler for HTTP OPTIONS requests targeting the given URI.
		 * @param uri The URI for which the handler is invoked.
		 * @param handler The handler function
		 * @param bodyHandler The optional function for handling the request body.
		 * @param uploadHandler The optional function for handling file uploads.
		 */
		virtual void OnOptions(const String& uri, HttpRequestHandler handler, HttpRequestBodyHandler bodyHandler = 0,
		        HttpUploadHandler uploadHandler = 0) = 0;

		/**
		 * Starts the server.
		 */
		virtual void Start() = 0;

		/**
		 * Stops the server.
		 */
		virtual void Stop() = 0;

	protected:
		/** Constructor */
		HttpServer();
		/** Destructor */
		virtual ~HttpServer();

	private:

};

}
} // namespace Web::Http
