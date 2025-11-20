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

#include <web/http/HttpStatusCodes.h>
#include <web/http/AbstractHtmlController.h>
#include <web/MimeTypes.h>
#include <rfs/RootFileSystem.h>
#include <utils/Log.h>

namespace Web {
namespace Http {

AbstractHtmlController::AbstractHtmlController() :
        Base(), mPageIsZipped(true), mAuthenticationRequired(false), mDefaultPageData(0), mDefaultPageDataSize(0), mUserName(0),
        mPassword(0), mRealm(0), mPageUri(0), mPagePath(0) {
}

AbstractHtmlController::~AbstractHtmlController() {
}

const String& AbstractHtmlController::GetHtmlPageUri() {
	return mPageUri;
}

void AbstractHtmlController::SetHtmlPageUri(const String& uri) {
	if (uri) {
		mPageUri = uri;
	}
}

void AbstractHtmlController::SetHtmlPagePath(const String& path, bool zipped) {
	if (path) {
		mPagePath = path;
		mPageIsZipped = zipped;
	}
}

const String& AbstractHtmlController::GetHtmlPagePath() {
	return mPagePath;
}

bool AbstractHtmlController::SetAuthenticationCredentials(const String& userName, const String& passPhrase, const String& realm,
        bool enableAuth) {
	mUserName = userName;
	mPassword = passPhrase;
	mRealm = realm;
	return SetAuthenticationEnabled(enableAuth);
}

bool AbstractHtmlController::SetAuthenticationEnabled(bool enable) {
	mAuthenticationRequired = enable && mUserName && !mUserName.isEmpty() && mPassword && !mPassword.isEmpty();
	return mAuthenticationRequired == enable;
}

bool AbstractHtmlController::IsAuthenticationEnabled() {
	return mAuthenticationRequired;
}

void AbstractHtmlController::SetDefaultHtmlPageData(const uint8_t* data, uint32_t size) {
	mDefaultPageData = data;
	mDefaultPageDataSize = size;
}

bool AbstractHtmlController::IsAuthenticated(::std::shared_ptr<HttpRequest> request) {
	bool isAuthenticated = !mAuthenticationRequired;
	if (!isAuthenticated) {
		isAuthenticated = request->Authenticate(mUserName, mPassword);
		if (!isAuthenticated) {
			request->RequestAuthentication();
		}
	}
	return isAuthenticated;
}

void AbstractHtmlController::OnAttach(HttpServerPtr httpServer) {
	if (httpServer) {
		OnAttach(*httpServer);
	}
}

void AbstractHtmlController::OnAttach(HttpServer& httpServer) {
	if (mPageUri) {
		httpServer.OnGet(mPageUri, [&](::std::shared_ptr<HttpRequest> request) {
			OnGetPageRequest(request);
		});
	}
}

void AbstractHtmlController::OnEvent(std::shared_ptr<AppEvent> ev) {
	// empty by intension
}

void AbstractHtmlController::OnGetPageRequest(::std::shared_ptr<HttpRequest> request) {
	SendHtml(request, mPagePath, mDefaultPageData, mDefaultPageDataSize, true);
}

void AbstractHtmlController::SendHtml(::std::shared_ptr<HttpRequest> request, const String& htmlPageRfsPath,
        const uint8_t* defaultPageData, uint32_t defaultPageDataSize, bool zipped) {
	if (IsAuthenticated(request)) {
		LogDbg("On GET update request");
		::std::shared_ptr<HttpResponse> response;
		if (htmlPageRfsPath && Rfs::RootFileSystem::GetInstance()->Exists(htmlPageRfsPath)) {
			// we have an existing custom HTML page -> use it
			response = request->CreateResponse(htmlPageRfsPath, MimeType::HTML, false);
		} else {
			// fall-back to built-in UI
			zipped = true;
			response = request->CreateProgmemResponse(defaultPageData, defaultPageDataSize, MimeType::HTML);
		}
		if (zipped) {
			response->AddHeader("Content-Encoding", "gzip");
		}
		request->SendResponse(response);
	}
}

void AbstractHtmlController::DumpDiagnostics() {
	LogInfo("Authentication: Enabled: %s, required: %s, realm: %s, user: %s, PW: %s", Logger::Dump(IsAuthenticationEnabled()),
	        Logger::Dump(mAuthenticationRequired), Logger::Dump(mRealm), Logger::Dump(mUserName), Logger::Dump(mPassword));
	LogInfo("RFS path for HTML page:   %s", mPagePath ? mPagePath.c_str() : "INVALID");
	LogInfo("GET  URI for HTML page:   %s", mPageUri ? mPageUri.c_str() : "INVALID");
}

} // namespace Http
} // namespace Web
