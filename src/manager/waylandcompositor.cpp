/****************************************************************************
**
** Copyright (C) 2015 Pelagicore AG
** Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company
** Contact: http://www.qt.io/ or http://www.pelagicore.com/
**
** This file is part of the Pelagicore Application Manager.
**
** $QT_BEGIN_LICENSE:GPL3-PELAGICORE$
** Commercial License Usage
** Licensees holding valid commercial Pelagicore Application Manager
** licenses may use this file in accordance with the commercial license
** agreement provided with the Software or, alternatively, in accordance
** with the terms contained in a written agreement between you and
** Pelagicore. For licensing terms and conditions, contact us at:
** http://www.pelagicore.com.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU General Public License version 3 requirements will be
** met: http://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
** SPDX-License-Identifier: GPL-3.0
**
****************************************************************************/

#include <QWaylandOutput>
#include <QWaylandShell>
#include <QWaylandQuickOutput>
#include <QWaylandWindowManagerExtension>
#include <private/qwlextendedsurface_p.h>
#include <QQuickView>

#include "windowmanager.h"
#include "applicationmanager.h"
#include "global.h"
#include "waylandcompositor.h"


void Surface::setShellSurface(QWaylandShellSurface *ss)
{
    m_shellSurface = ss;
    m_item = new SurfaceQuickItem(this);
}

void Surface::setExtendedSurface(QtWayland::ExtendedSurface *ext)
{
    m_ext = ext;
}

QWaylandShellSurface *Surface::shellSurface() const { return m_shellSurface; }

QtWayland::ExtendedSurface *Surface::extendedSurface() const { return m_ext; }

void Surface::takeFocus()
{
    m_item->takeFocus();
}

void Surface::ping() { m_shellSurface->ping(); }

qint64 Surface::processId() const { return m_surface->client()->processId(); }

QWindow *Surface::outputWindow() const
{
    if (QWaylandView *v = m_surface->throttlingView())
        return v->output()->window();
    return 0;
}

QVariantMap Surface::windowProperties() const { return m_ext ? m_ext->windowProperties() : QVariantMap(); }

void Surface::setWindowProperty(const QString &n, const QVariant &v)
{
    if (m_ext)
        m_ext->setWindowProperty(n, v);
}

void Surface::connectPong(const std::function<void ()> &cb)
{
    connect(m_shellSurface, &QWaylandShellSurface::pong, cb);
}

void Surface::connectWindowPropertyChanged(const std::function<void (const QString &, const QVariant &)> &cb)
{
    if (m_ext)
        connect(m_ext, &QtWayland::ExtendedSurface::windowPropertyChanged, cb);
}

QQuickItem *Surface::item() const
{
    return m_item;
}


SurfaceQuickItem::SurfaceQuickItem(Surface *s)
    : QWaylandQuickItem()
    , m_surface(s)
{
    setSurface(s);
    setTouchEventsEnabled(true);
    setSizeFollowsSurface(false);
}

void SurfaceQuickItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    qCDebug(LogWayland) << "sendConfigure" << m_surface << newGeometry.size() << "(PID:" << m_surface->client()->processId() << ")";
    m_surface->shellSurface()->sendConfigure(newGeometry.size().toSize(), QWaylandShellSurface::NoneEdge);

    QWaylandQuickItem::geometryChanged(newGeometry, oldGeometry);
}

Surface::Surface(QWaylandCompositor *comp, QWaylandClient *client, uint id, int version)
    : QWaylandQuickSurface(comp, client, id, version)
    , m_item(0)
    , m_shellSurface(0)
    , m_ext(0)
{
    m_surface = this;
}

WaylandCompositor::WaylandCompositor(QQuickView *view, const QString &waylandSocketName, WindowManager *manager)
    : QWaylandQuickCompositor()
    , m_manager(manager)
    , m_shell(new QWaylandShell(this))
    , m_output(new QWaylandQuickOutput(this, view))
    , m_surfExt(new QtWayland::SurfaceExtensionGlobal(this))
{
    setSocketName(waylandSocketName.toUtf8());

    view->winId();

    connect(this, &QWaylandCompositor::createSurface, this, &WaylandCompositor::doCreateSurface);
    connect(this, &QWaylandCompositor::surfaceCreated, [this](QWaylandSurface *s) {
        m_manager->waylandSurfaceCreated(static_cast<Surface *>(s));
    });

    setenv("WAYLAND_DISPLAY", qPrintable(waylandSocketName), 1);

    connect(m_shell, &QWaylandShell::createShellSurface, this, &WaylandCompositor::createShellSurface);
    connect(m_surfExt, &QtWayland::SurfaceExtensionGlobal::extendedSurfaceReady, this, &WaylandCompositor::extendedSurfaceReady);

    auto wmext = new QWaylandWindowManagerExtension(this);
    connect(wmext, &QWaylandWindowManagerExtension::openUrl, this, [](QWaylandClient *client, const QUrl &url) {
        if (ApplicationManager::instance()->fromProcessId(client->processId()))
            ApplicationManager::instance()->openUrl(url.toString());
    });

    create();
}

void WaylandCompositor::doCreateSurface(QWaylandClient *client, uint id, int version)
{
    new Surface(this, client, id, version);
}

void WaylandCompositor::createShellSurface(QWaylandSurface *surface, const QWaylandResource &resource)
{
    Surface *s = static_cast<Surface *>(surface);

    qCDebug(LogWayland) << "createShellSurface" << s << "(PID:" << s->client()->processId() << ")";
    QWaylandShellSurface *ss = new QWaylandShellSurface(m_shell, s, resource);
    s->setShellSurface(ss);

    connect(s, &QWaylandSurface::mappedChanged, this, &WaylandCompositor::waylandSurfaceMappedChanged);
}

void WaylandCompositor::extendedSurfaceReady(QtWayland::ExtendedSurface *ext, QWaylandSurface *surface)
{
    Surface *surf = static_cast<Surface *>(surface);
    surf->setExtendedSurface(ext);
}

void WaylandCompositor::waylandSurfaceMappedChanged()
{
    Surface *surface = static_cast<Surface *>(sender());
    if (surface->isMapped()) {
        m_manager->waylandSurfaceMapped(surface);
    } else {
        m_manager->waylandSurfaceUnmapped(surface);
    }
}

QWaylandSurface *WaylandCompositor::waylandSurfaceFromItem(QQuickItem *surfaceItem) const
{
    // QWaylandQuickItem::surface() will return a nullptr to WindowManager::waylandSurfaceDestroyed,
    // if the app crashed. We return our internal copy of that surface pointer instead.
    if (SurfaceQuickItem *item = qobject_cast<SurfaceQuickItem *>(surfaceItem))
        return item->m_surface->surface();
    return nullptr;
}