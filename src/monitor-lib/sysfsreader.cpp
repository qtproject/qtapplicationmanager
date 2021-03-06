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

#include "sysfsreader.h"


QT_BEGIN_NAMESPACE_AM

SysFsReader::SysFsReader(const QByteArray &path, int maxRead)
    : m_path(path)
{
    m_buffer.resize(maxRead);
    m_fd.setFileName(QString::fromLocal8Bit(path));
    m_fd.open(QIODevice::ReadOnly | QIODevice::Unbuffered);
}

SysFsReader::~SysFsReader()
{ }

bool SysFsReader::isOpen() const
{
    return m_fd.isOpen();
}

QByteArray SysFsReader::fileName() const
{
    return m_path;
}

QByteArray SysFsReader::readValue() const
{
    if (!m_fd.isOpen())
        return QByteArray();
    if (!m_fd.seek(0))
        return QByteArray();

    int offset = 0;
    int read = 0;
    do {
        read = m_fd.read(m_buffer.data() + offset, m_buffer.size() - offset);
        if (read < 0)
            return QByteArray();
        else if (read < (m_buffer.size() - offset))
            m_buffer[read + offset] = 0;
        offset += read;
    } while (read > 0 && offset < m_buffer.size());
    return m_buffer;
}

QT_END_NAMESPACE_AM
