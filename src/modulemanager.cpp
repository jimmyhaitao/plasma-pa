/*
    Copyright 2016 David Edmundson <davidedmundson@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "modulemanager.h"
#include "module.h"
#include "../config.h"

#if USE_GSETTINGS
#include "gsettingsitem.h"

#define PA_SETTINGS_PATH_MODULES "/org/freedesktop/pulseaudio/module-groups"
#endif

#if USE_GCONF
#include "gconfitem.h"
#define PA_SETTINGS_PATH_MODULES "/system/pulseaudio/modules"
#endif

#include <QTimer>

namespace QPulseAudio
{

#if USE_GCONF || USE_GSETTINGS

#if USE_GSETTINGS
class ConfigModule : public GSettingsItem
#elif USE_GCONF
class ConfigModule : public GConfItem
#endif
{
public:
    ConfigModule(const QString &configName, const QString &moduleName, QObject *parent);
    bool isEnabled() const;
    void setEnabled(bool enabled, const QVariant &args=QVariant());
private:
    QString m_moduleName;
};

ConfigModule::ConfigModule(const QString &configName, const QString &moduleName, QObject *parent) :
#if USE_GSETTINGS
    GSettingsItem(QStringLiteral(PA_SETTINGS_PATH_MODULES"/") + configName + QStringLiteral("/"), parent),
#elif USE_GCONF
    GConfItem(QStringLiteral(PA_SETTINGS_PATH_MODULES"/") + configName, parent),
#endif
    m_moduleName(moduleName)
{
}

bool ConfigModule::isEnabled() const
{
    return value(QStringLiteral("enabled")).toBool();
}

void ConfigModule::setEnabled(bool enabled, const QVariant &args)
{
    set(QStringLiteral("locked"), true);

    if (enabled) {
        set(QStringLiteral("name0"), m_moduleName);
        set(QStringLiteral("args0"), args);
        set(QStringLiteral("enabled"), true);
    } else {
        set(QStringLiteral("enabled"), false);
    }
    set(QStringLiteral("locked"), false);
}

#endif

ModuleManager::ModuleManager(QObject *parent) :
    QObject(parent)
{
#if USE_GCONF || USE_GSETTINGS
    m_combineSinks = new ConfigModule(QStringLiteral("combine"), QStringLiteral("module-combine"), this);
    m_switchOnConnect = new ConfigModule(QStringLiteral("switch-on-connect"), QStringLiteral("module-switch-on-connect"), this);
    m_deviceManager = new ConfigModule(QStringLiteral("device-manager"), QStringLiteral("module-device-manager"), this);

    connect(m_combineSinks, &ConfigModule::subtreeChanged, this, &ModuleManager::combineSinksChanged);
    connect(m_switchOnConnect, &ConfigModule::subtreeChanged, this, &ModuleManager::switchOnConnectChanged);
    connect(m_deviceManager, &ConfigModule::subtreeChanged, this, &ModuleManager::switchOnConnectChanged);
#endif

    QTimer *updateModulesTimer = new QTimer(this);
    updateModulesTimer->setInterval(500);
    updateModulesTimer->setSingleShot(true);
    connect(updateModulesTimer, &QTimer::timeout, this, &ModuleManager::updateLoadedModules);
    connect(&Context::instance()->modules(), &MapBaseQObject::added, updateModulesTimer, static_cast<void(QTimer::*)(void)>(&QTimer::start));
    connect(&Context::instance()->modules(), &MapBaseQObject::removed, updateModulesTimer, static_cast<void(QTimer::*)(void)>(&QTimer::start));
    updateLoadedModules();
}

ModuleManager::~ModuleManager()
{
};

bool ModuleManager::settingsSupported() const
{
#if USE_GCONF || USE_GSETTINGS
    return true;
#else
    return false;
#endif
}

bool ModuleManager::combineSinks() const
{
#if USE_GCONF || USE_GSETTINGS
    return m_combineSinks->isEnabled();
#else
    return false;
#endif
}

void ModuleManager::setCombineSinks(bool combineSinks)
{
#if USE_GCONF || USE_GSETTINGS
    m_combineSinks->setEnabled(combineSinks);
#else
    Q_UNUSED(combineSinks()
#endif
}

bool ModuleManager::switchOnConnect() const
{
#if USE_GCONF || USE_GSETTINGS
    //switch on connect and device-manager do the same task. Only one should be enabled

    //Note on the first run m_deviceManager will appear to be disabled even though it's actually running
    //because there is no gconf entry, however m_switchOnConnect will only exist if set by Plasma PA
    //hence only check this entry
    return m_switchOnConnect->isEnabled() ;
#else
    return false;
#endif
}

void ModuleManager::setSwitchOnConnect(bool switchOnConnect)
{
#if USE_GCONF || USE_GSETTINGS
    m_deviceManager->setEnabled(!switchOnConnect);
    m_switchOnConnect->setEnabled(switchOnConnect);
#else
    Q_UNUSED(switchOnConnect)
#endif
}

QStringList ModuleManager::loadedModules() const
{
    return m_loadedModules;
}

void ModuleManager::updateLoadedModules()
{
    m_loadedModules.clear();
    const auto modules = Context::instance()->modules().data();
    for (Module *module : modules) {
        m_loadedModules.append(module->name());
    }
    Q_EMIT loadedModulesChanged();
}

bool ModuleManager::configModuleLoaded() const
{
    return m_loadedModules.contains(configModuleName());
}

QString ModuleManager::configModuleName() const
{
#if USE_GCONF
    return QStringLiteral("module-gconf");
#elif USE_GSETTINGS
    return QStringLiteral("module-gsettings");
#else
    return QString();
#endif
}
}
