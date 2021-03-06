/*********************************************************************/
/* Copyright (c) 2016-2017, EPFL/Blue Brain Project                  */
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

#ifndef TEXTURENODERGBA_H
#define TEXTURENODERGBA_H

#include "TextureNode.h"

#include <QOpenGLBuffer>
#include <QSGSimpleTextureNode>
#include <memory>

class QQuickWindow;

/**
 * A node with a double buffered texture.
 *
 * Initially it displays an empty black texture (id 0). Users can upload data
 * asynchronously to the texture and call swap() on the next frame rendering to
 * display the results.
 *
 * The texture can be either static or dynamic:
 * * In the dynamic case, two PBOs are used for real-time texture updates.
 * * In the static case, a single PBO is used for the initial texture upload and
 *   then released in the first call to swap() so that no memory is wasted.
 */
class TextureNodeRGBA : public QSGSimpleTextureNode, public TextureNode
{
public:
    /**
     * Create a textured rectangle for rendering RGBA images on the GPU.
     * @param window a reference to the quick window for generating textures.
     * @param dynamic true if the texture is going to be updated more than once.
     */
    TextureNodeRGBA(QQuickWindow& window, bool dynamic);

    /** @sa QSGOpaqueTextureMaterial::setMipmapFiltering */
    void setMipmapFiltering(QSGTexture::Filtering filtering);

    QRectF getCoord() const final { return rect(); }
    void setCoord(const QRectF& coord) final { setRect(coord); }
    void uploadTexture(const Image& image) final;
    void swap() final;

private:
    QQuickWindow& _window;
    bool _dynamicTexture = false;

    std::unique_ptr<QSGTexture> _texture;
    std::unique_ptr<QOpenGLBuffer> _pbo;

    QSize _nextTextureSize;
    uint _glImageFormat = 0;
};

#endif
