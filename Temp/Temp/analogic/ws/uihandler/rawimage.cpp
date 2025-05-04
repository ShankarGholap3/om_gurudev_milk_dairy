/*!
* @file     rawimage.cpp
* @author   Agiliad
* @brief    This file contains functions related to Raw Image
*           of Image Quality Panel.
* @date     Jan, 16 2017
*
(c) Copyright <2016-2017> Analogic Corporation. All Rights Reserved
*/

#include <qfile.h>
#include <Logger.h>
#include <analogic/ws/uihandler/rawimage.h>

namespace analogic
{
namespace ws
{
/*!
* @fn       RawImage
* @param    QQuickImageProvider::Pixmap
* @return   None
* @brief    Constructor for class RawImage.
*/
RawImage::RawImage():QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}
/*!
* @fn       requestPixmap
* @param    const QString, QSize, const QSize
* @return   Qpixmap
* @brief    This function is responsible for opening Raw Image.
*/
QPixmap RawImage::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(requestedSize);
    int width = 730;
    QPixmap pixmap;
    QFile file(id);
    if (!file.open(QFile::ReadOnly))
    {
        LOG(ERROR) << "RawImage::requestPixmap: file open failed: " << id.toStdString();
        return pixmap;
    }
    float filesize = file.size();
    float height = filesize/(width * 2);
    int tempheight = height;
    if (!(height - tempheight) == 0)
    {
        QString errMsg = "Can not parse projection file received for Image quality.";
        UILayoutManager::getUILayoutManagerInstance()->displayMsg(Errors::S_OK,
                                                                  errMsg,
                                                                  QMLEnums::QML_MESSAGE_MODEL);
        return pixmap;
    }
    if (size)
        *size = QSize(width, height);
    QMatrix rm;
    rm.rotate(90);
    QImage img(*size, QImage::Format_Grayscale8);
    int bpl = img.bytesPerLine();

    uint16_t buf;
    uchar* dst = img.bits();
    if(!dst)
    {
        LOG(FATAL) << "RawImage::requestPixmap: QImage.bits() returned NULL";
        return pixmap;
    }
    uint16_t min = 0, max = 0;
    int maxpixel_origimg =  width*height;
    uint16_t temp[maxpixel_origimg+1];
    for(int count = maxpixel_origimg; count; count--)
    {
        file.read((char*)&buf, 2);
        temp[count] = buf;
        if(count == maxpixel_origimg)
        {
            min = max = buf;
        }
        else
        {
            if(min > buf)
            {
                min = buf;
            }
            if(max < buf)
            {
                max = buf;
            }
        }
    }

    uint16_t windoWidth = max - min;\
    if(windoWidth == 0)
        LOG(ERROR) << "RawImage::requestPixmap: windoWidth calculated as 0.";

    double halfwindowwidth =(double)1/2;
    uint16_t windowcenter = min + (double)windoWidth/2;
    uint16_t * temppointer = temp + maxpixel_origimg -1;

    for(int rowIndex = 0; rowIndex < height; rowIndex++)

        for(int colIndex = 0; colIndex < bpl; colIndex++){
            if(colIndex < width)
            {
                if((*temppointer) <= (windowcenter - halfwindowwidth - (windoWidth-1)/2))
                {
                    dst[0] = 0;
                }

                else if((*temppointer) > (windowcenter -halfwindowwidth + (windoWidth-1)/2))
                {
                    dst[0] = 255;
                }

                else
                {
                    dst[0] = ((*temppointer - ((double)(windowcenter) - halfwindowwidth)) /
                              (windoWidth-1) + halfwindowwidth) * (255);
                }
               // dst[0] = (*temppointer)*(255)/(max-min) ;
                temppointer--;
            }
            else
            {
                dst[0] = 0;
            }
            dst++;
        }



    if(!pixmap.convertFromImage(img))
    {
        LOG(ERROR) << "RawImage::requestPixmap: convertFromImage() failed.";
        return pixmap;
    }

    if(!img.save("./image.png"))
    {
        LOG(ERROR) << "RawImage::requestPixmap: QImage.save() failed.";
        return pixmap;
    }

    LOG(INFO) << "RawImage::requestPixmap() succeeded.";
    pixmap = pixmap.transformed(rm);
    return pixmap;
}
}  // namespace ws
}  // namespace analogic
