/*********************************************************************/
/* Copyright (c) 2018, EPFL/Blue Brain Project                       */
/*                     Raphael Dumusc <raphael.dumusc@epfl.ch>       */
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

#include "MarkersUpdater.h"

#include "scene/Markers.h"

#include <limits>

namespace
{
// TUIO touch point IDs start at 0
constexpr int MOUSE_MARKER_ID = std::numeric_limits<int>::max();
}

MarkersUpdater::MarkersUpdater(Markers& markers, const QSizeF& surfaceSize)
    : _markers{markers}
    , _surfaceSize{surfaceSize}
{
}

void MarkersUpdater::update(const QTouchEvent* event)
{
    for (const auto& point : event->touchPoints())
    {
        switch (point.state())
        {
        case Qt::TouchPointPressed:
            _markers.addMarker(point.id(), _getWallPos(point.normalizedPos()));
            break;
        case Qt::TouchPointMoved:
            _markers.updateMarker(point.id(),
                                  _getWallPos(point.normalizedPos()));
            break;
        case Qt::TouchPointReleased:
            _markers.removeMarker(point.id());
            break;
        case Qt::TouchPointStationary:
            break;
        }
    }
}

void MarkersUpdater::mousePressed(const QPointF& pos)
{
    _markers.addMarker(MOUSE_MARKER_ID, pos);
}

void MarkersUpdater::mouseMoved(const QPointF& pos)
{
    _markers.updateMarker(MOUSE_MARKER_ID, pos);
}

void MarkersUpdater::mouseReleased(const QPointF& pos)
{
    Q_UNUSED(pos);
    _markers.removeMarker(MOUSE_MARKER_ID);
}

QPointF MarkersUpdater::_getWallPos(const QPointF& normalizedPos)
{
    return QPointF{normalizedPos.x() * _surfaceSize.width(),
                   normalizedPos.y() * _surfaceSize.height()};
}
