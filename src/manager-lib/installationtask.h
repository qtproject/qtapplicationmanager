/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Copyright (C) 2019 Luxoft Sweden AB
** Copyright (C) 2018 Pelagicore AG
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtApplicationManager module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
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
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#pragma once

#include <QUrl>
#include <QStringList>
#include <QWaitCondition>
#include <QMutex>

#include <QtAppManApplication/installationreport.h>
#include <QtAppManManager/asynchronoustask.h>
#include <QtAppManManager/scopeutilities.h>

QT_BEGIN_NAMESPACE_AM

class PackageInfo;
class PackageManager;
class PackageExtractor;


class InstallationTask : public AsynchronousTask
{
    Q_OBJECT
public:
    InstallationTask(const QString &installationPath, const QString &documentPath,
                     const QUrl &sourceUrl, QObject *parent = nullptr);
    ~InstallationTask() override;

    void acknowledge();
    bool cancel() override;

signals:
    void finishedPackageExtraction();

protected:
    void execute() override;

private:
    void startInstallation() Q_DECL_NOEXCEPT_EXPR(false);
    void finishInstallation() Q_DECL_NOEXCEPT_EXPR(false);
    void checkExtractedFile(const QString &file) Q_DECL_NOEXCEPT_EXPR(false);

private:
    PackageManager *m_pm;
    QString m_installationPath;
    QString m_documentPath;
    QUrl m_sourceUrl;
    bool m_foundInfo = false;
    bool m_foundIcon = false;
    QString m_iconFileName;
    uint m_extractedFileCount = 0;
    bool m_managerApproval = false;
    QScopedPointer<PackageInfo> m_package;
    uint m_applicationUid = uint(-1);

    // changes to these 4 member variables are protected by m_mutex
    PackageExtractor *m_extractor = nullptr;
    bool m_canceled = false;
    bool m_installationAcknowledged = false;
    QWaitCondition m_installationAcknowledgeWaitCondition;

    static QMutex s_serializeFinishInstallation;

    QDir m_applicationDir;
    QDir m_extractionDir;

    ScopedDirectoryCreator m_installationDirCreator;
};

QT_END_NAMESPACE_AM
