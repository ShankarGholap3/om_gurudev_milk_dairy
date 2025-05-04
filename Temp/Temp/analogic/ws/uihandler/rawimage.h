/*!
* @file     rawimage.h
* @author   Agiliad
* @brief    This file contains functions related to Raw Image
*           of Image Quality Panel.
* @date     Jan, 16 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#ifndef ANALOGIC_WS_UIHANDLER_RAWIMAGE_H_
#define ANALOGIC_WS_UIHANDLER_RAWIMAGE_H_

#include <QQuickImageProvider>
#include <analogic/ws/common/archive/localarchivebaglisthandler.h>

namespace analogic
{
namespace ws
{
/*!
 * \class   RawImage
 * \brief   This class contains variable and function related to
 *          handling Raw Image of Image Quality Panel.
 */

class RawImage : public QQuickImageProvider
{
public:
    /*!
    * @fn       RawImage
    * @param    QQuickImageProvider::Pixmap
    * @return   None
    * @brief    Constructor for class RawImage.
    */
    RawImage();

    /*!
    * @fn       requestPixmap
    * @param    const QString, QSize, const QSize
    * @return   Qpixmap
    * @brief    This function is responsible for opening Raw Image.
    */
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
};
}  // namespace ws
}  // namespace analogic

#endif  // ANALOGIC_WS_UIHANDLER_RAWIMAGE_H_
