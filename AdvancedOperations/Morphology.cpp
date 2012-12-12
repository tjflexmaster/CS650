#include "Morphology.h"
#include <QImage>
#include <QList>
#include <QDebug>

Morphology::Morphology()
{
}

QImage Morphology::dilation(QImage *image, QList<QPoint> *structure)
{
    QImage new_image(image->size(), QImage::Format_ARGB32_Premultiplied);
    convolve(&new_image, image, structure, 1);
    return new_image;
}

QImage Morphology::erosion(QImage *image, QList<QPoint> *structure)
{
    QImage new_image(image->size(), QImage::Format_ARGB32_Premultiplied);
    convolve(&new_image, image, structure, structure->size());
    return new_image;
}

QImage Morphology::majority(QImage *image, QList<QPoint> *structure)
{
    QImage new_image(image->size(), QImage::Format_ARGB32_Premultiplied);
    int threshold = structure->size()/2;
    qDebug() << "Majority Threshold: " << threshold;
    convolve(&new_image, image, structure, threshold);
    return new_image;
}

QImage Morphology::opening(QImage *image, QList<QPoint> *structure)
{
    QImage fake_image(image->size(), QImage::Format_ARGB32_Premultiplied);
    convolve(&fake_image, image, structure, structure->size());
    QImage new_image(image->size(), QImage::Format_ARGB32_Premultiplied);
    convolve(&new_image, &fake_image, structure, 1);
    return new_image;
}

QImage Morphology::closing(QImage *image, QList<QPoint> *structure)
{
    QImage fake_image(image->size(), QImage::Format_ARGB32_Premultiplied);
    convolve(&fake_image, image, structure, 1);
    QImage new_image(image->size(), QImage::Format_ARGB32_Premultiplied);
    convolve(&new_image, &fake_image, structure, structure->size());
    return new_image;
}

QList<QPoint> Morphology::getSquareStruct(int side)
{
    QList<QPoint> kernel;
    int halfside;
    if ( side % 2 == 0 ) {
        halfside = side / 2;
    } else {
        halfside = (side-1) / 2;
    }
    for(int j=-halfside; j <= halfside; j++) {
        for(int i=-halfside; i <= halfside; i++) {
            kernel.append(QPoint(i,j));
        }
    }
    return kernel;
}

QList<QPoint> Morphology::getRectStruct(int width, int height)
{
    QList<QPoint> kernel;
    int half_w;
    if ( width % 2 == 0 ) {
        half_w = width / 2;
    } else {
        half_w = (width-1) / 2;
    }
    int half_h;
    if ( height % 2 == 0 ) {
        half_h = height / 2;
    } else {
        half_h = (height-1) / 2;
    }
    for(int j=-half_h; j <= half_h; j++) {
        for(int i=-half_w; i <= half_w; i++) {
            kernel.append(QPoint(i,j));
        }
    }
    return kernel;
}

QList<QPoint> Morphology::getPlusStruct(int length)
{
    QList<QPoint> kernel;

    for(int j=1; j <= length; j++) {
        kernel.append(QPoint(0, j));
        kernel.append(QPoint(0,-j));
        kernel.append(QPoint(j,0));
        kernel.append(QPoint(-j,0));
    }
    return kernel;
}

QList<QPoint> Morphology::getCustomStruct()
{
    QList<QPoint> kernel;
    kernel.append(QPoint(1,1));
    kernel.append(QPoint(1,0));
    kernel.append(QPoint(1,-1));
    kernel.append(QPoint(0,1));
    kernel.append(QPoint(0,0));
    kernel.append(QPoint(0,-1));
    kernel.append(QPoint(-1,1));
    kernel.append(QPoint(-1,0));
    kernel.append(QPoint(-1,-1));
    kernel.append(QPoint(2,1));
    kernel.append(QPoint(2,0));
    kernel.append(QPoint(2,-1));
    kernel.append(QPoint(-2,1));
    kernel.append(QPoint(-2,0));
    kernel.append(QPoint(-2,-1));
    kernel.append(QPoint(1,2));
    kernel.append(QPoint(0,2));
    kernel.append(QPoint(-1,2));
    kernel.append(QPoint(1,-2));
    kernel.append(QPoint(0,-2));
    kernel.append(QPoint(-1,-2));
    return kernel;
}

void Morphology::convolve(QImage *new_image, QImage *image, QList<QPoint> *structure, int threshold)
{
    for( int y=0; y < image->height(); y++ ) {
        for( int x=0; x < image->width(); x++ ) {
            QPoint point(x,y);
            int sum = 0;
            //Perform Convolution
            foreach( QPoint i, *structure) {
                //Only convolve real pixels
                int pixel_x = i.x() + x;
                int pixel_y = i.y() + y;
                if ( pixel_x < 0 ||
                     pixel_x >= image->width() ||
                     pixel_y < 0 ||
                     pixel_y >= image->height() ) {
                    continue;
                }
                QPoint temp(pixel_x, pixel_y);
                if ( qGray(image->pixel(temp)) == 0 )
                    sum++;
            }

            //Perform Morphology
            QRgb val;
            if ( sum >= threshold ) {
                val = qRgb(0,0,0);
            } else {
                val = qRgb(255,255,255);
            }
            new_image->setPixel(point, val);
        }
    }
}
