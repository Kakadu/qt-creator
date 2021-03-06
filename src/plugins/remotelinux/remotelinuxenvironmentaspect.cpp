/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "remotelinuxenvironmentaspect.h"

#include "remotelinuxenvironmentaspectwidget.h"

namespace RemoteLinux {

// --------------------------------------------------------------------
// RemoteLinuxEnvironmentAspect:
// --------------------------------------------------------------------

RemoteLinuxEnvironmentAspect::RemoteLinuxEnvironmentAspect(ProjectExplorer::RunConfiguration *rc) :
    ProjectExplorer::EnvironmentAspect(rc)
{ }

RemoteLinuxEnvironmentAspect *RemoteLinuxEnvironmentAspect::create(ProjectExplorer::RunConfiguration *parent) const
{
    return new RemoteLinuxEnvironmentAspect(parent);
}

ProjectExplorer::RunConfigWidget *RemoteLinuxEnvironmentAspect::createConfigurationWidget()
{
    return new RemoteLinuxEnvironmentAspectWidget(this);
}

QList<int> RemoteLinuxEnvironmentAspect::possibleBaseEnvironments() const
{
    return QList<int>() << static_cast<int>(RemoteBaseEnvironment)
                        << static_cast<int>(CleanBaseEnvironment);
}

QString RemoteLinuxEnvironmentAspect::baseEnvironmentDisplayName(int base) const
{
    if (base == static_cast<int>(CleanBaseEnvironment))
        return tr("Clean Environment");
    else  if (base == static_cast<int>(RemoteBaseEnvironment))
        return tr("System Environment");
    return QString();
}

Utils::Environment RemoteLinuxEnvironmentAspect::baseEnvironment() const
{
    Utils::Environment env;
    if (baseEnvironmentBase() == static_cast<int>(RemoteBaseEnvironment))
        env = m_remoteEnvironment;
    const QString displayKey = QLatin1String("DISPLAY");
    if (!env.hasKey(displayKey))
        env.appendOrSet(displayKey, QLatin1String(":0.0"));
    return env;
}

Utils::Environment RemoteLinuxEnvironmentAspect::remoteEnvironment() const
{
    return m_remoteEnvironment;
}

void RemoteLinuxEnvironmentAspect::setRemoteEnvironment(const Utils::Environment &env)
{
    if (env != m_remoteEnvironment) {
        m_remoteEnvironment = env;
        if (baseEnvironmentBase() == static_cast<int>(RemoteBaseEnvironment))
            emit environmentChanged();
    }
}

QString RemoteLinuxEnvironmentAspect::userEnvironmentChangesAsString() const
{
    QString env;
    QString placeHolder = QLatin1String("%1=%2 ");
    foreach (const Utils::EnvironmentItem &item, userEnvironmentChanges())
        env.append(placeHolder.arg(item.name, item.value));
    return env.mid(0, env.size() - 1);
}

} // namespace RemoteLinux

