#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>

#include <bb/system/phone/Phone>
#include <bb/system/phone/Call>
#include <bb/system/phone/CallState>
#include <bb/system/phone/CallType>

using namespace bb::cascades;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
		QObject(app) {
	// prepare the localization
	m_pTranslator = new QTranslator(this);
	m_pLocaleHandler = new LocaleHandler(this);
	if (!QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()),
			this, SLOT(onSystemLanguageChanged()))) {
		// This is an abnormal situation! Something went wrong!
		// Add own code to recover here
		qWarning() << "Recovering from a failed connect()";
	}
	// initial load
	onSystemLanguageChanged();

	// Create scene document from main.qml asset, the parent is set
	// to ensure the document gets destroyed properly at shut down.
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	// Create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();

	// Set created root object as the application scene
	app->setScene(root);
	bb::system::phone::Phone * phone = new bb::system::phone::Phone(this);
	bool success = connect(phone, SIGNAL(callUpdated(const bb::system::phone::Call&)),
			this, SLOT(onCallUpdated(const bb::system::phone::Call&)));

	if (success) {
		// Signal was successfully connected.
	} else {
		// Failed to connect to signal.
		// This is not normal in most cases and can be a critical situation
		// for your app! Make sure you know exactly why this has happened.
		// Add some code to recover from the lost connection below this line.
	}
}

void ApplicationUI::onSystemLanguageChanged() {
	QCoreApplication::instance()->removeTranslator(m_pTranslator);
	// Initiate, load and install the application translation files.
	QString locale_string = QLocale().name();
	QString file_name = QString("PhoneTest_%1").arg(locale_string);
	if (m_pTranslator->load(file_name, "app/native/qm")) {
		QCoreApplication::instance()->installTranslator(m_pTranslator);
	}
}

void ApplicationUI::onCallUpdated(const bb::system::phone::Call& call) {
	bb::system::phone::CallState::Type state = call.callState();
	qDebug() << "call updated: callId=" << call.callId() << " callState=" << state;

	switch (state) {
		case bb::system::phone::CallState::Connected:
			qDebug("[TEST] Connected");
			break;
		case bb::system::phone::CallState::Disconnected:
			qDebug("[TEST] Disconnected");
			break;
		case bb::system::phone::CallState::Incoming:
			qDebug("[TEST] Incoming");
			break;
		case bb::system::phone::CallState::Failed:
			qDebug("[TEST] Failed");
			break;
		case bb::system::phone::CallState::RemoteRinging:
			qDebug("[TEST] RemoteRinging");
			break;
		case bb::system::phone::CallState::Held:
			qDebug("[TEST] Held");
			break;
		case bb::system::phone::CallState::MultiPartyMember:
			qDebug("[TEST] MultiPartyMember");
			break;
		case bb::system::phone::CallState::Connecting:
			qDebug("[TEST] Connecting");
			break;
		case bb::system::phone::CallState::Unknown:
			qDebug("[TEST] Unknown");
			break;
		default:
			qDebug("[TEST] Error, nothing valid!");
			break;
	}
}
