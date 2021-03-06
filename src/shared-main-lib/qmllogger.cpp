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

#include <QMessageLogger>

#include "qmllogger.h"
#include "global.h"
#include "logging.h"

QT_BEGIN_NAMESPACE_AM

QmlLogger::QmlLogger(QQmlEngine *engine)
    : QObject(engine)
{
    connect(engine, &QQmlEngine::warnings, this, &QmlLogger::warnings);
}

void QmlLogger::warnings(const QList<QQmlError> &list)
{
    if (!LogQml().isWarningEnabled())
        return;

    for (const QQmlError &err : list) {
        QByteArray func;
        if (err.object())
            func = err.object()->objectName().toLocal8Bit();
        QByteArray file;
        if (err.url().scheme() == qL1S("file"))
            file = err.url().toLocalFile().toLocal8Bit();
        else
            file = err.url().toDisplayString().toLocal8Bit();

        QMessageLogger ml(file, err.line(), func, LogQml().categoryName());
        ml.warning().nospace() << qPrintable(err.description());
    }
}

QT_END_NAMESPACE_AM

#include "moc_qmllogger.cpp"
