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

#include <appfw/AppEventTarget.h>
#include <web/http/HttpServer.h>

#include <WString.h>
#include <memory>

namespace Web {
namespace Http {

using namespace AppFw;

class AbstractHtmlController : public AppEventTarget {
		typedef AppEventTarget Base;
	public:

		/**
		 * Applies the URI for retrieving the HTML page of this controller.
		 *
		 * GET requests to this URI respond with the configured page.
		 *
		 * @param uri The URI for requesting the controllers HTML page.
		 */
		void SetHtmlPageUri(const String& uri);

		/**
		 * Provides the URI for retrieving the HTML page of this controller.
		 *
		 * @return The URI for requesting the controllers HTML page.
		 */
		const String& GetHtmlPageUri();

		/**
		 * Applies the path to the custom HTML page suitable for this controller.
		 * Default: Built-in HTML page of the controller.
		 *
		 * The page is served as response to GET requests to the URI
		 * defined via method 'SetHtmlPageUri'.
		 *
		 * The page is expected being 'gzip'ped.
		 *
		 * @param path The path for the customized HTML page within the local root file system.
		 * @param zipped True if the HTML page is zipped, false otherwise.
		 */
		void SetHtmlPagePath(const String& path, bool zipped = true);

		/**
		 * Provides the path to the custom HTML page suitable for this controller.
		 *
		 * @param path The path for the customized HTML page within the local root file system.
		 */
		const String& GetHtmlPagePath();

		/**
		 * Applies the credentials for authentication of the software update process.
		 * @param userName The user name of the credentials.
		 * @param passPhrase The password of the credentials.
		 * @param realm The optional authentication realm, default to unset.
		 * @param enableAuth True (default) to enable authentication, false to disable it.
		 * @return True if the credentials have been applied and the enable request succeeded, false otherwise.
		 */
		bool SetAuthenticationCredentials(const String& userName, const String& passPhrase, const String& realm =
		        Gbl::invalidString, bool enableAuth = true);

		/**
		 * Enables or disables authentication for updating the software
		 * using previously applied credentials. If no credentials have been
		 * applied, the authentication will not be enabled.
		 * @param enable True to enable authentication, false to disable it.
		 * @return True if the request has been committed, false otherwise.
		 */
		bool SetAuthenticationEnabled(bool enable);

		/**
		 * Determines whether or not authentication is enabled.
		 * Authentication may be disabled due to the corresponding configuration or invalid credentials.
		 *
		 * @return True if authentication is enabled, false otherwise.
		 */
		bool IsAuthenticationEnabled();

		/**
		 * Prints information about this HTTP controller without any header or footer.
		 */
		virtual void DumpDiagnostics();

	protected:
		/** Constructor */
		AbstractHtmlController();
		/** Destructor */
		virtual ~AbstractHtmlController();

		/**
		 * Specifies the built-in default HTML to respond if no custom HTML page
		 * was applied via 'SetHtmlPagePath()' method.
		 *
		 * The default page is expected being zipped and located in program memory.
		 *
		 * @param data The data of the built-in default page.
		 * @param size The size of the default page data in bytes.
		 */
		void SetDefaultHtmlPageData(const uint8_t* data, uint32_t size);

		/**
		 * Determines whether or not the request is authenticated.
		 * A request is authenticated if at least one of the following conditions is true:
		 * + HTTP authentication is disabled
		 * + The user name and password provided by the request matches the credentials
		 *   specified via 'SetAuthenticationCredentials' perfectly.
		 *
		 * @param request The request to validate.
		 * @return True if the request is authenticated, false otherwise.
		 */
		virtual bool IsAuthenticated(::std::shared_ptr<HttpRequest> request);

		/**
		 * Sends the specified HTML page in response to the given request to the client.
		 * The responded HTML page may be located within the root file system to allow
		 * customization of the page. If the page does not exist in RFS or the given path
		 * is invalid, the page provided as 'default data' is sent instead. The default data
		 * are expected being stored in program memory.
		 *
		 * @param request The request to which the response shall be sent.
		 * @param htmlPageRfsPath The absolute path of the HTML to send within the root file system.
		 * @param defaultPageData The data in program memory of the default page.
		 * @param defaultPageDataSize The size of the default page data in bytes.
		 * @param isZipped True if the page or page data is zipped (default), false otherwise.
		 */
		virtual void SendHtml(::std::shared_ptr<HttpRequest> request, const String& htmlPageRfsPath,
		        const uint8_t* defaultPageData, uint32_t defaultPageDataSize, bool zipped = true);

		/**
		 * Attaches this controller to the given server.
		 *
		 * This implementation registers a response handler that
		 * sends the applied HTML page (custom or built-in)
		 * IF a non-empty URI is also applied before this method is invoked.
		 */
		virtual void OnAttach(HttpServer& httpServer);

		/**
		 * Attaches REST controller to the given server by delegating the
		 * invocation to the alternative OnAttach method.
		 */
		void OnAttach(HttpServerPtr httpServer);

		/**
		 * Is invoked upon application events.
		 *
		 * @param ev The received event.
		 */
		virtual void OnEvent(std::shared_ptr<AppEvent> ev);

		/**
		 * Is invoked when a request is received, that matches the 'page URI'.
		 * This methods sends the applied HTML page as response.
		 *
		 * @param request The received request.
		 */
		virtual void OnGetPageRequest(::std::shared_ptr<HttpRequest> request);

	private:
		friend class HttpServer;

		bool mPageIsZipped;
		bool mAuthenticationRequired;

		const uint8_t* mDefaultPageData;
		uint32_t mDefaultPageDataSize;

		String mUserName;
		String mPassword;
		String mRealm;

		String mPageUri;
		String mPagePath;
};

} // namespace Http
} // namespace Web
