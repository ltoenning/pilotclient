#include "blackcore/context_runtime.h"
#include "blackmisc/nwserver.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/avaircraft.h"
#include "blackmisc/blackmiscfreefunctions.h"

#include "blackcore/context_application_impl.h"
#include "blackcore/context_network_impl.h"
#include "blackcore/context_settings_impl.h"
#include "blackcore/context_audio_impl.h"
#include "blackcore/context_simulator_impl.h"

#include "blackcore/context_application_proxy.h"
#include "blackcore/context_network_proxy.h"
#include "blackcore/context_settings_proxy.h"
#include "blackcore/context_audio_proxy.h"
#include "blackcore/context_simulator_proxy.h"

#include <QDebug>

namespace BlackCore
{
    /*
     * Constructor
     */
    CRuntime::CRuntime(const CRuntimeConfig &config, QObject *parent) :
        QObject(parent), m_init(false), m_dbusServer(nullptr), m_initDBusConnection(false),
        m_signalLogApplication(false), m_signalLogAudio(false), m_signalLogNetwork(false), m_signalLogSettings(false), m_signalLogSimulator(false),
        m_slotLogApplication(false), m_slotLogAudio(false), m_slotLogNetwork(false), m_slotLogSettings(false), m_slotLogSimulator(false),
        m_dbusConnection(QDBusConnection("default")),
        m_contextApplication(nullptr), m_contextAudio(nullptr), m_contextNetwork(nullptr), m_contextSettings(nullptr), m_contextSimulator(nullptr)
    {
        this->init(config);
    }

    /*
     * Signal logging
     */
    void CRuntime::signalLog(bool enabled)
    {
        this->signalLogForApplication(enabled);
        this->signalLogForAudio(enabled);
        this->signalLogForNetwork(enabled);
        this->signalLogForSettings(enabled);
        this->signalLogForSimulator(enabled);
    }

    /*
     * Signal logging
     */
    void CRuntime::slotLog(bool enabled)
    {
        this->slotLogForApplication(enabled);
        this->slotLogForAudio(enabled);
        this->slotLogForNetwork(enabled);
        this->slotLogForSettings(enabled);
        this->slotLogForSimulator(enabled);
    }

    /*
     * Enable signal logging
     */
    bool CRuntime::signalLogForApplication(bool enabled)
    {
        if (enabled == this->m_signalLogApplication) return enabled;
        if (!this->getIContextApplication())
        {
            this->m_signalLogApplication = false;
            return false;
        }
        this->m_signalLogApplication = enabled;
        if (enabled)
        {
            QMetaObject::Connection con;
            con = QObject::connect(this->getIContextApplication(), &IContextApplication::widgetGuiStarting,
            [this]() { QStringList l; l << "widgetGuiStarting"; this->logSignal(this->getIContextApplication(), l);});
            this->m_logSignalConnections.insert("application", con);
            con = QObject::connect(this->getIContextApplication(), &IContextApplication::widgetGuiTerminating,
            [this]() { QStringList l; l << "widgetGuiTerminating"; this->logSignal(this->getIContextApplication(), l);});
            this->m_logSignalConnections.insert("application", con);
            con = QObject::connect(this->getIContextApplication(), &IContextApplication::statusMessage,
            [this](const BlackMisc::CStatusMessage & msg) { QStringList l; l << "statusMessage" << msg.toQString() ; this->logSignal(this->getIContextApplication(), l);});
            this->m_logSignalConnections.insert("application", con);
            con = QObject::connect(this->getIContextApplication(), &IContextApplication::statusMessages,
            [this](const BlackMisc::CStatusMessageList & msgs) { QStringList l; l << "statusMessages" << msgs.toQString(); this->logSignal(this->getIContextApplication(), l);});
            this->m_logSignalConnections.insert("application", con);
        }
        else
        {
            this->disconnectLogSignals("application");
        }
        return enabled;
    }

    bool CRuntime::signalLogForAudio(bool enabled)
    {
        if (enabled == this->m_signalLogAudio) return enabled;
        if (!this->getIContextNetwork())
        {
            this->m_signalLogAudio = false;
            return false;
        }
        this->m_signalLogAudio = enabled;
        if (enabled)
        {
            QMetaObject::Connection con;
            con = QObject::connect(this->getIContextAudio(), &IContextAudio::audioTestCompleted,
            [this]() { QStringList l; l << "audioTestCompleted"; this->logSignal(this->getIContextAudio(), l);});
            this->m_logSignalConnections.insert("audio", con);
        }
        else
        {
            this->disconnectLogSignals("audio");
        }
        return enabled;
    }

    /*
     * Enable signal logging
     */
    bool CRuntime::signalLogForNetwork(bool enabled)
    {
        if (enabled == this->m_signalLogNetwork) return enabled;
        if (!this->getIContextNetwork())
        {
            this->m_signalLogNetwork = false;
            return false;
        }
        this->m_signalLogNetwork = enabled;
        if (enabled)
        {
            QMetaObject::Connection con;
            con = QObject::connect(this->getIContextNetwork(), &IContextNetwork::changedAircraftsInRange,
            [this]() { QStringList l; l << "changedAircraftsInRange"; this->logSignal(this->getIContextNetwork(), l);});
            this->m_logSignalConnections.insert("network", con);
            con = QObject::connect(this->getIContextNetwork(), &IContextNetwork::changedAtcStationsBooked,
            [this]() { QStringList l; l << "changedAtcStationsBooked"; this->logSignal(this->getIContextNetwork(), l);});
            this->m_logSignalConnections.insert("network", con);
            con = QObject::connect(this->getIContextNetwork(), &IContextNetwork::changedAtcStationsOnline,
            [this]() { QStringList l; l << "changedAtcStationsOnline"; this->logSignal(this->getIContextNetwork(), l);});
            this->m_logSignalConnections.insert("network", con);
            con = QObject::connect(this->getIContextNetwork(), &IContextNetwork::changedAircraftSituation,
            [this](const BlackMisc::Aviation::CCallsign & callsign, const BlackMisc::Aviation::CAircraftSituation & situation) { QStringList l; l << "changedAircraftSituation" << callsign.toQString() << situation.toQString(); this->logSignal(this->getIContextNetwork(), l);});
            this->m_logSignalConnections.insert("network", con);
            con = QObject::connect(this->getIContextNetwork(), &IContextNetwork::connectionStatusChanged,
            [this](uint from, uint to) { QStringList l; l << "connectionStatusChanged" << QString::number(from) << QString::number(to); this->logSignal(this->getIContextNetwork(), l);});
            this->m_logSignalConnections.insert("network", con);
        }
        else
        {
            this->disconnectLogSignals("network");
        }
        return enabled;
    }

    /*
     * Enable signal logging
     */
    bool CRuntime::signalLogForSettings(bool enabled)
    {
        if (enabled == this->m_signalLogSettings) return enabled;
        if (!this->getIContextSettings())
        {
            this->m_signalLogSettings = false;
            return false;
        }
        this->m_signalLogSettings = enabled;
        if (enabled)
        {
            QMetaObject::Connection con;
            con = QObject::connect(this->getIContextSettings(), &IContextSettings::changedSettings,
            [this]() { QStringList l; l << "changedSettings"; this->logSignal(this->getIContextSettings(), l);});
            this->m_logSignalConnections.insert("settings", con);
        }
        else
        {
            this->disconnectLogSignals("settings");
        }
        return enabled;
    }

    /*
     * Enable signal logging
     */
    bool CRuntime::signalLogForSimulator(bool enabled)
    {
        if (enabled == this->m_signalLogSimulator) return enabled;
        if (!this->getIContextSimulator())
        {
            this->m_signalLogSimulator = false;
            return false;
        }
        this->m_signalLogSimulator = enabled;
        if (enabled)
        {
            QMetaObject::Connection con;
            con = QObject::connect(this->getIContextSimulator(), &IContextSimulator::connectionChanged,
            [this]() { QStringList l; l << "connectionChanged"; this->logSignal(this->getIContextSimulator(), l);});
            this->m_logSignalConnections.insert("simulator", con);
        }
        else
        {
            this->disconnectLogSignals("simulator");
        }
        return enabled;
    }

    void CRuntime::logSignal(QObject *sender, const QStringList &values)
    {
        QString s = (sender) ? sender->metaObject()->className() : "";
        qDebug() << "signal" << s << values;
    }

    void CRuntime::logSlot(const char *func, const QString &p1, const QString &p2, const QString &p3, const QString &p4) const
    {
        if (p1.isEmpty()) qDebug() << func; return;
        if (p2.isEmpty()) qDebug() << func << p1; return;
        if (p3.isEmpty()) qDebug() << func << p1 << p2; return;
        if (p4.isEmpty()) qDebug() << func << p1 << p2 << p3; return;
        qDebug() << func << p1 << p2 << p3 << p4; return;
    }

    /*
     * Init runtime
     */
    void CRuntime::init(const CRuntimeConfig &config)
    {
        if (m_init) return;
        BlackMisc::registerMetadata();
        BlackMisc::initResources();

        // upfront reading of settings, as DBus server alread relies on
        // settings
        CContextSettings *settings = nullptr;
        QString dbusServerAddress;
        if (config.hasLocalSettings())
        {
            settings = new CContextSettings(config.getModeSettings(), this);
            if (settings) settings->read();
            dbusServerAddress = settings->getNetworkSettings().getDBusServerAddress();
        }

        // contexts
        switch (config.getModeSettings())
        {
        case CRuntimeConfig::Local:
            this->m_contextSettings = settings;
            break;
        case CRuntimeConfig::LocalInDbusServer:
            this->initDBusServer(dbusServerAddress);
            this->m_contextSettings = settings->registerWithDBus(this->m_dbusServer);
            break;
        case CRuntimeConfig::Remote:
            this->initDBusConnection();
            this->m_contextSettings = new BlackCore::CContextSettingsProxy(BlackCore::CDBusServer::ServiceName, this->m_dbusConnection, config.getModeSettings(), this);
            break;
        default:
            break;
        }

        switch (config.getModeApplication())
        {
        case CRuntimeConfig::Local:
            this->m_contextApplication = new CContextApplication(config.getModeApplication(), this);
            break;
        case CRuntimeConfig::LocalInDbusServer:
            this->initDBusServer(dbusServerAddress);
            this->m_contextApplication = (new CContextApplication(config.getModeApplication(), this))->registerWithDBus(this->m_dbusServer);
            break;
        case CRuntimeConfig::Remote:
            this->initDBusConnection();
            this->m_contextApplication = new BlackCore::CContextApplicationProxy(BlackCore::CDBusServer::ServiceName, this->m_dbusConnection, config.getModeApplication(), this);
            break;
        default:
            break;
        }

        switch (config.getModeAudio())
        {
        case CRuntimeConfig::Local:
            this->m_contextAudio = new CContextAudio(config.getModeAudio(), this);
            break;
        case CRuntimeConfig::LocalInDbusServer:
            this->initDBusServer(dbusServerAddress);
            this->m_contextAudio = (new CContextAudio(config.getModeAudio(), this))->registerWithDBus(this->m_dbusServer);
            break;
        case CRuntimeConfig::Remote:
            this->initDBusConnection();
            this->m_contextAudio = new BlackCore::CContextAudioProxy(BlackCore::CDBusServer::ServiceName, this->m_dbusConnection, config.getModeAudio(), this);
            break;
        default:
            break;
        }

        switch (config.getModeNetwork())
        {
        case CRuntimeConfig::Local:
            this->m_contextNetwork = new CContextNetwork(config.getModeNetwork(), this);
            break;
        case CRuntimeConfig::LocalInDbusServer:
            this->initDBusServer(dbusServerAddress);
            this->m_contextNetwork = (new CContextNetwork(config.getModeNetwork(), this))->registerWithDBus(this->m_dbusServer);
            break;
        case CRuntimeConfig::Remote:
            this->initDBusConnection();
            this->m_contextNetwork = new BlackCore::CContextNetworkProxy(BlackCore::CDBusServer::ServiceName, this->m_dbusConnection, config.getModeNetwork(), this);
            break;
        default:
            break;
        }

        switch (config.getModeSimulator())
        {
        case CRuntimeConfig::Local:
            this->m_contextSimulator = new CContextSimulator(config.getModeSimulator(), this);
            break;
        case CRuntimeConfig::LocalInDbusServer:
            this->initDBusServer(dbusServerAddress);
            this->m_contextSimulator = (new CContextSimulator(config.getModeSimulator(), this))->registerWithDBus(this->m_dbusServer);
            break;
        case CRuntimeConfig::Remote:
            this->initDBusConnection();
            this->m_contextSimulator = new BlackCore::CContextSimulatorProxy(BlackCore::CDBusServer::ServiceName, this->m_dbusConnection, config.getModeSimulator(), this);
            break;
        default:
            break;
        }

        // post inits, wiring things among context (e.g. signal slots)
        this->initPostSetup();

        // flag
        m_init = true;
    }

    void CRuntime::initDBusServer(const QString &dBusAddress)
    {
        if (this->m_dbusServer) return;
        Q_ASSERT(!dBusAddress.isEmpty());
        this->m_dbusServer = new CDBusServer(dBusAddress, this);
    }

    void CRuntime::initPostSetup()
    {
        if (this->m_contextSettings && this->m_contextAudio && this->m_contextSettings->usingLocalObjects())
        {
            // only, when both contexts exists and only if settings originate locally
            connect(this->m_contextSettings, &IContextSettings::changedSettings,
                    this->getCContextAudio(), &CContextAudio::settingsChanged);
        }

        if (this->m_contextSimulator && this->m_contextSimulator->usingLocalObjects() && this->m_contextNetwork)
        {
            if (this->getCContextSimulator()->m_simulator)
            {
                connect(this->m_contextNetwork, SIGNAL(aircraftSituationUpdate(BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftSituation)),
                        this->getCContextSimulator()->m_simulator, SLOT(addAircraftSituation(BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftSituation)));
            }
        }
    }

    void CRuntime::gracefulShutdown()
    {
        if (!this->m_init) return;
        this->m_init = false;

        // disable all logging and all signals towards runtime
        this->signalLog(false);
        disconnect(this);

        // unregister all from DBus
        this->m_dbusServer->unregisterAllObjects();

        // handle contexts
        if (this->getIContextApplication())
        {
            disconnect(this->getIContextApplication());
            this->getIContextApplication()->setOutputRedirectionLevel(IContextApplication::RedirectNone);
            this->getIContextApplication()->setStreamingForRedirectedOutputLevel(IContextApplication::RedirectNone);
            CContextApplicationBase::resetOutputRedirection();
        }

        if (this->getIContextSimulator())
        {
            // TODO: disconnect from simulator
            disconnect(this->getIContextSimulator());
        }

        // log off from network, if connected
        if (this->getIContextNetwork())
        {
            disconnect(this->getIContextNetwork());
            this->getIContextNetwork()->disconnectFromNetwork();
        }

        if (this->getIContextAudio()) disconnect(this->getIContextAudio());
        if (this->getIContextSettings()) disconnect(this->getIContextAudio());

        // mark contexts as invalid
        // they will be deleted by the parent object (this runtime)
        this->m_contextApplication = nullptr;
        this->m_contextAudio = nullptr;
        this->m_contextNetwork = nullptr;
        this->m_contextSettings = nullptr;
        this->m_contextSimulator = nullptr;
    }

    void CRuntime::disconnectLogSignals(const QString &name)
    {
        if (!this->m_logSignalConnections.contains(name)) return;
        QMultiMap<QString, QMetaObject::Connection>::const_iterator i = this->m_logSignalConnections.constFind(name);
        while (i != this->m_logSignalConnections.end() && i.key() == name)
        {
            disconnect(i.value());
            ++i;
        }
        this->m_logSignalConnections.remove(name);
    }

    void CRuntime::initDBusConnection()
    {
        if (this->m_initDBusConnection) return;
        this->m_dbusConnection = QDBusConnection::sessionBus();
    }

    IContextNetwork *CRuntime::getIContextNetwork()
    {
        return this->m_contextNetwork;
    }

    const IContextNetwork *CRuntime::getIContextNetwork() const
    {
        return this->m_contextNetwork;
    }

    IContextAudio *CRuntime::getIContextAudio()
    {
        return this->m_contextAudio;
    }

    const IContextAudio *CRuntime::getIContextAudio() const
    {
        return this->m_contextAudio;
    }

    IContextSettings *CRuntime::getIContextSettings()
    {
        return this->m_contextSettings;
    }

    const IContextSettings *CRuntime::getIContextSettings() const
    {
        return this->m_contextSettings;
    }

    const IContextApplication *CRuntime::getIContextApplication() const
    {
        return this->m_contextApplication;
    }

    IContextApplication *CRuntime::getIContextApplication()
    {
        return this->m_contextApplication;
    }

    const IContextSimulator *CRuntime::getIContextSimulator() const
    {
        return this->m_contextSimulator;
    }

    IContextSimulator *CRuntime::getIContextSimulator()
    {
        return this->m_contextSimulator;
    }

    CContextAudio *CRuntime::getCContextAudio()
    {
        return static_cast<CContextAudio *>(this->m_contextAudio);
    }

    CContextAudio *CRuntime::getCContextAudio() const
    {
        return static_cast<CContextAudio *>(this->m_contextAudio);
    }

    CContextApplication *CRuntime::getCContextApplication()
    {
        return static_cast<CContextApplication *>(this->m_contextApplication);
    }

    CContextApplication *CRuntime::getCContextApplication() const
    {
        return static_cast<CContextApplication *>(this->m_contextApplication);
    }

    CContextSimulator *CRuntime::getCContextSimulator()
    {
        return static_cast<CContextSimulator *>(this->m_contextSimulator);
    }

    CContextSimulator *CRuntime::getCContextSimulator() const
    {
        return static_cast<CContextSimulator *>(this->m_contextSimulator);
    }

    bool CRuntimeConfig::requiresDBusSever() const
    {
        return (this->m_application == LocalInDbusServer ||
                this->m_audio == LocalInDbusServer ||
                this->m_network == LocalInDbusServer ||
                this->m_settings == LocalInDbusServer ||
                this->m_simulator == LocalInDbusServer);
    }

    bool CRuntimeConfig::requiresDBusConnection() const
    {
        return (this->m_application == Remote ||
                this->m_audio == Remote ||
                this->m_network == Remote ||
                this->m_settings == Remote ||
                this->m_simulator == Remote);
    }

    const CRuntimeConfig &CRuntimeConfig::forCore()
    {
        static CRuntimeConfig cfg = CRuntimeConfig(CRuntimeConfig(CRuntimeConfig::LocalInDbusServer));
        return cfg;
    }

    const CRuntimeConfig &CRuntimeConfig::local()
    {
        static CRuntimeConfig cfg = CRuntimeConfig(CRuntimeConfig(CRuntimeConfig::Local));
        return cfg;
    }

    const CRuntimeConfig &CRuntimeConfig::remote()
    {
        static CRuntimeConfig cfg = CRuntimeConfig(CRuntimeConfig(CRuntimeConfig::Remote));
        return cfg;
    }
}
