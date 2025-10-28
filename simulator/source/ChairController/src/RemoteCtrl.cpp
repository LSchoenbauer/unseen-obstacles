
#include "RemoteCtrl.h"

#include <appfw/AppComponent.h>
#include <web/MimeTypes.h>
#include <utils/Log.h>

using namespace Web;
using namespace Web::Http;

RemoteCtrl::RemoteCtrl() :
        Base(), mHttpServer(0) {
}

RemoteCtrl::~RemoteCtrl() {
}

void RemoteCtrl::Init() {

	// HTTP server

	// root directory for public content - should be retrieved from configuration
	String docRoot("/ffs/www");

	// create the server instance
	mHttpServer = HttpServer::Create(docRoot);

	// add a simple 'ping' REST endpoint
	mHttpServer->OnRequest("/ping", [](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogDbg("HTTP: Received PING");
		req->SendContent("<!DOCTYPE html><html><body>PONG</body></html>", MimeType::HTML);
	});

	mHttpServer->OnRequest("/clickMe", [](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button clicked");
		req->SendStatus(200);
	});

	// configure static content after other handlers to avoid searching pages that are handled by others
	mHttpServer->SetIndexPage("/index.html");
	mHttpServer->SetStaticContentRoot("/", "/");

	// eventually start the server
	mHttpServer->Start();

	LogInfo("");
	LogInfo("----------------------------------------------------");

	LogInfo("RemoteCtrl initialized");
}
