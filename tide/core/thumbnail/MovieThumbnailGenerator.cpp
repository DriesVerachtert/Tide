/*********************************************************************/
/* Copyright (c) 2013-2018, EPFL/Blue Brain Project                  */
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

#include "MovieThumbnailGenerator.h"

#include "data/FFMPEGMovie.h"
#include "data/FFMPEGPicture.h"
#include "utils/stereoimage.h"

#define PREVIEW_RELATIVE_POSITION 0.5

MovieThumbnailGenerator::MovieThumbnailGenerator(const QSize& size)
    : ThumbnailGenerator{size}
{
}

QImage MovieThumbnailGenerator::generate(const QString& filename) const
{
    try
    {
        FFMPEGMovie movie{filename};
        movie.setFormat(TextureFormat::rgba);

        const auto target = PREVIEW_RELATIVE_POSITION * movie.getDuration();
        const auto picture = movie.getFrame(target);
        if (!picture)
            throw std::runtime_error("rendering movie frame failed");

        auto image = picture->toQImage(); // QImage wrapper; no copy

        if (movie.isStereo())
            image = stereoimage::extract(image, deflect::View::mono);
        // WAR: QImage::scaled doesn't make a copy if scaled size == image size
        else if (image.size().scaled(_size, _aspectRatioMode) == image.size())
            return image.copy();

        return image.scaled(_size, _aspectRatioMode);
    }
    catch (const std::runtime_error&)
    {
        return createErrorImage("movie");
    }
}
