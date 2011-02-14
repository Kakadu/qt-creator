/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** No Commercial Usage
**
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/

#ifndef S60DEVICERUNCONFIGURATIONWIDGET_H
#define S60DEVICERUNCONFIGURATIONWIDGET_H

#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

namespace Utils {
    class DetailsWidget;
    class DebuggerLanguageChooser;
}

namespace Qt4ProjectManager {
namespace Internal {

class S60DeviceRunConfiguration;

class S60DeviceRunConfigurationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit S60DeviceRunConfigurationWidget(S60DeviceRunConfiguration *runConfiguration,
                                             QWidget *parent = 0);
private slots:
    void argumentsEdited(const QString &text);
    void runConfigurationEnabledChange(bool enabled);
    void useCppDebuggerToggled(bool);
    void useQmlDebuggerToggled(bool);
    void qmlDebugServerPortChanged(uint);

private:
    S60DeviceRunConfiguration *m_runConfiguration;
    Utils::DetailsWidget *m_detailsWidget;
    Utils::DebuggerLanguageChooser *m_debuggerLanguageChooser;
    QLineEdit *m_argumentsLineEdit;
};

} // namespace Internal
} // namespace Qt4ProjectManager

#endif // S60DEVICERUNCONFIGURATIONWIDGET_H
