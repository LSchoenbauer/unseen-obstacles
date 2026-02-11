
#include "RemoteCtrl.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <appfw/AppComponent.h>
#include <web/MimeTypes.h>
#include <utils/Log.h>

using namespace Web;
using namespace Web::Http;

RemoteCtrl::RemoteCtrl() :
        Base(), mHttpServer(0), mChairController() {
}

RemoteCtrl::~RemoteCtrl() {
}

void RemoteCtrl::Init(ChairControllerPtr chairController) {
	LogDbg("init remotectrl");
	mChairController = chairController;

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

	// TODO: shape the routes: /motor/<id>/<command> all lowercase

	// Motor 1
	mHttpServer->OnRequest("/motor1Up", [this](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 1 UP clicked (core %d)", xPortGetCoreID());
		const CommandData commandData(CommandData::Command::UP,CommandData::Mover::REAR_LEFT);
		mChairController->SetCommandModeEnabled(true);
		mChairController->ApplyCommand(commandData);
		req->SendStatus(200);
	});

	mHttpServer->OnRequest("/motor1Center", [this](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 1 CENTER clicked");
		const CommandData commandData(CommandData::Command::TO_CENTER,CommandData::Mover::REAR_LEFT);
		mChairController->SetCommandModeEnabled(true);
		mChairController->ApplyCommand(commandData);
		req->SendStatus(200);
	});

	mHttpServer->OnRequest("/motor1Down", [this](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 1 DOWN clicked");
		const CommandData commandData(CommandData::Command::DOWN,CommandData::Mover::REAR_LEFT);
		mChairController->SetCommandModeEnabled(true);
		mChairController->ApplyCommand(commandData);
		req->SendStatus(200);
	});


	// Motor 2
	mHttpServer->OnRequest("/motor2Up", [this](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 2 UP clicked");
		const CommandData commandData(CommandData::Command::UP,CommandData::Mover::REAR_RIGHT);
		mChairController->SetCommandModeEnabled(true);
		mChairController->ApplyCommand(commandData);
		req->SendStatus(200);
	});

	mHttpServer->OnRequest("/motor2Center", [this](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 2 CENTER clicked");
		const CommandData commandData(CommandData::Command::TO_CENTER,CommandData::Mover::REAR_RIGHT);
		mChairController->SetCommandModeEnabled(true);
		mChairController->ApplyCommand(commandData);
		req->SendStatus(200);
	});

	mHttpServer->OnRequest("/motor2Down", [this](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 2 DOWN clicked");
		const CommandData commandData(CommandData::Command::DOWN,CommandData::Mover::REAR_RIGHT);
		mChairController->SetCommandModeEnabled(true);
		mChairController->ApplyCommand(commandData);
		req->SendStatus(200);
	});


	// Motor 3
	mHttpServer->OnRequest("/motor3Up", [this](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 3 UP clicked");
		const CommandData commandData(CommandData::Command::UP,CommandData::Mover::FRONT);
		mChairController->SetCommandModeEnabled(true);
		mChairController->ApplyCommand(commandData);
		req->SendStatus(200);
	});

	mHttpServer->OnRequest("/motor3Center", [this](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 3 CENTER clicked");
		const CommandData commandData(CommandData::Command::TO_CENTER,CommandData::Mover::FRONT);
		mChairController->SetCommandModeEnabled(true);
		mChairController->ApplyCommand(commandData);
		req->SendStatus(200);
	});

	mHttpServer->OnRequest("/motor3Down", [this](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 3 DOWN clicked");
		const CommandData commandData(CommandData::Command::DOWN,CommandData::Mover::FRONT);
		mChairController->SetCommandModeEnabled(true);
		mChairController->ApplyCommand(commandData);
		req->SendStatus(200);
	});


	// Motor 4
	mHttpServer->OnRequest("/motor4Up", [this](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 4 UP clicked");
		const CommandData commandData(CommandData::Command::UP,CommandData::Mover::ROTATION);
		mChairController->SetCommandModeEnabled(true);
		mChairController->ApplyCommand(commandData);
		req->SendStatus(200);
	});

	mHttpServer->OnRequest("/motor4Center", [this](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 4 CENTER clicked");
		const CommandData commandData(CommandData::Command::TO_CENTER,CommandData::Mover::ROTATION);
		mChairController->SetCommandModeEnabled(true);
		mChairController->ApplyCommand(commandData);
		req->SendStatus(200);
	});

	mHttpServer->OnRequest("/motor4Down", [this](std::shared_ptr<Web::Http::HttpRequest> req) {
		LogInfo("HTTP: Button Motor 4 DOWN clicked");
		const CommandData commandData(CommandData::Command::DOWN,CommandData::Mover::ROTATION);
		mChairController->SetCommandModeEnabled(true);
		mChairController->ApplyCommand(commandData);
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
