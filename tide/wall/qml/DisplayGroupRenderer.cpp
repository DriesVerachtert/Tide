/*********************************************************************/
/* Copyright (c) 2014-2018, EPFL/Blue Brain Project                  */
/*                          Raphael Dumusc <raphael.dumusc@epfl.ch>  */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/*   1. Redistributions of source code must retain the above         */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer.                                                  */
/*                                                                   */
/*   2. Redistributions in binary form must reproduce the above      */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer in the documentation and/or other materials       */
/*      provided with the distribution.                              */
/*                                                                   */
/*    THIS  SOFTWARE  IS  PROVIDED  BY  THE  ECOLE  POLYTECHNIQUE    */
/*    FEDERALE DE LAUSANNE  ''AS IS''  AND ANY EXPRESS OR IMPLIED    */
/*    WARRANTIES, INCLUDING, BUT  NOT  LIMITED  TO,  THE  IMPLIED    */
/*    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR  A PARTICULAR    */
/*    PURPOSE  ARE  DISCLAIMED.  IN  NO  EVENT  SHALL  THE  ECOLE    */
/*    POLYTECHNIQUE  FEDERALE  DE  LAUSANNE  OR  CONTRIBUTORS  BE    */
/*    LIABLE  FOR  ANY  DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,    */
/*    EXEMPLARY,  OR  CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT NOT    */
/*    LIMITED TO,  PROCUREMENT  OF  SUBSTITUTE GOODS OR SERVICES;    */
/*    LOSS OF USE, DATA, OR  PROFITS;  OR  BUSINESS INTERRUPTION)    */
/*    HOWEVER CAUSED AND  ON ANY THEORY OF LIABILITY,  WHETHER IN    */
/*    CONTRACT, STRICT LIABILITY,  OR TORT  (INCLUDING NEGLIGENCE    */
/*    OR OTHERWISE) ARISING  IN ANY WAY  OUT OF  THE USE OF  THIS    */
/*    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   */
/*                                                                   */
/* The views and conclusions contained in the software and           */
/* documentation are those of the authors and should not be          */
/* interpreted as representing official policies, either expressed   */
/* or implied, of Ecole polytechnique federale de Lausanne.          */
/*********************************************************************/

#include "DisplayGroupRenderer.h"

#include "DataProvider.h"
#include "scene/DisplayGroup.h"
#include "tools/VisibilityHelper.h"
#include "utils/geometry.h"
#include "utils/qml.h"

namespace
{
const QUrl QML_DISPLAYGROUP_URL("qrc:/qml/core/DisplayGroup.qml");
}

DisplayGroupRenderer::DisplayGroupRenderer(const WallRenderContext& context,
                                           QQuickItem& parentItem)
    : _context{context}
    , _qmlContext{new QQmlContext(context.engine.rootContext())}
    , _displayGroup{DisplayGroup::create(QSize(1, 1))}
{
    _qmlContext->setContextProperty("displaygroup", _displayGroup.get());
    _createDisplayGroupQmlItem(parentItem);
}

void DisplayGroupRenderer::setDisplayGroup(DisplayGroupPtr displayGroup)
{
    _qmlContext->setContextProperty("displaygroup", displayGroup.get());
    _updateWindowItems(*displayGroup);
    _displayGroup = std::move(displayGroup);
}

void DisplayGroupRenderer::_updateWindowItems(const DisplayGroup& displayGroup)
{
    QSet<QUuid> updatedWindows;
    const QQuickItem* parentItem = nullptr;
    const auto helper = VisibilityHelper{displayGroup, _context.screenRect,
                                         _context.isAlphaBlendingEnabled()};

    for (const auto& window : displayGroup.getWindows())
    {
        const auto& id = window->getID();

        updatedWindows.insert(id);

        if (!_windowItems.contains(id))
            _createWindowQmlItem(window);

        _windowItems[id]->update(window, helper.getVisibleArea(*window));

        // Update stacking order
        auto quickItem = _windowItems[id]->getQuickItem();
        if (parentItem)
            quickItem->stackAfter(parentItem);
        parentItem = quickItem;
    }

    _removeOldWindows(updatedWindows);
}

void DisplayGroupRenderer::_removeOldWindows(const QSet<QUuid>& updatedWindows)
{
    auto it = _windowItems.begin();
    while (it != _windowItems.end())
    {
        if (updatedWindows.contains(it.key()))
            ++it;
        else
            it = _windowItems.erase(it);
    }
}

void DisplayGroupRenderer::_createDisplayGroupQmlItem(QQuickItem& parentItem)
{
    _displayGroupItem =
        qml::makeItem(_context.engine, QML_DISPLAYGROUP_URL, _qmlContext.get());
    _displayGroupItem->setParentItem(&parentItem);
}

void DisplayGroupRenderer::_createWindowQmlItem(WindowPtr window)
{
    const auto& id = window->getID();
    auto sync = _context.provider.createSynchronizer(*window, _context.view);
    _windowItems[id].reset(
        new WindowRenderer(std::move(sync), std::move(window),
                           *_displayGroupItem, _qmlContext.get()));
}
