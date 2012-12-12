#include "ImageStitching.h"
#include <QList>
#include <QImage>
#include <QPoint>
#include <QDebug>
#include <qmath.h>
#include "Utility.h"
#include "tnt/tnt_array2d.h"
#include "tnt/jama_lu.h"
#include "tnt/jama_cholesky.h"
#include "tnt/jama_qr.h"
#include "stdlib.h"
#include "iostream"

ImageStitching::ImageStitching()
{
    _homography = Array2D<double>(3,3,0.0);

}

ImageStitching::~ImageStitching()
{

}

QImage ImageStitching::getStitchedImages(QImage *target, QList<QPoint> target_points, QImage *source, QList<QPoint> source_points)
{
    QImage new_image( Utility::blackImage(target->size()) );
    _target = target;
    _source = source;

    Q_ASSERT(target_points.count() >= 4);
    Q_ASSERT(source_points.count() >= 4);
//    target_points.clear();
//    target_points.append(QPoint(27, 231));
//    target_points.append((QPoint(195, 229)));
//    target_points.append(QPoint(170, 242));
//    target_points.append(QPoint(53, 243));

//    source_points.clear();
//    source_points.append(QPoint(37, 29));
//    source_points.append(QPoint(205, 29));
//    source_points.append(QPoint(182,41));
//    source_points.append(QPoint(62,43));

    //Shift all points from a central coordinate system to a topleft corner system.
    QPoint target_shift(target->width()/2, target->height()/2);
    QPoint source_shift(source->width()/2, source->height()/2);

    //Build our homographies
    if ( target_points.count() == source_points.count() ) {
        _A = Array2D<double>(8, 8, 0.0);
        _B = Array2D<double>(8, 1, 0.0);

        //Initialize our matrixes
        for(int i=0; i < target_points.count(); i++) {
            //First Row of A = i*2
            int x_row = i*2;
            int y_row = x_row + 1;
            QPoint tp = target_points.at(i) + target_shift;
            QPoint sp = source_points.at(i) + source_shift;
//            QPoint tp = target_points.at(i);
//            QPoint sp = source_points.at(i);

//            qDebug() << "X: " << tp.x() << "->" << sp.x() << " Y: " << tp.y() << "->" << sp.y();

            //X Row
            _A[x_row][0] = tp.x();
            _A[x_row][1] = tp.y();
            _A[x_row][2] = 1;
            _A[x_row][6] = -(sp.x() * tp.x());
            _A[x_row][7] = -(sp.x() * tp.y());

            _B[x_row][0] = sp.x();

            qDebug() << "A: " << _A[x_row][0] << "," << _A[x_row][1] << "," << _A[x_row][2] << "," << _A[x_row][3] << "," << _A[x_row][4] << "," << _A[x_row][5] << "," << _A[x_row][6] << "," << _A[x_row][7];
            qDebug() << "B: " << _B[x_row][0];
            //Y Row
            _A[y_row][3] = tp.x();
            _A[y_row][4] = tp.y();
            _A[y_row][5] = 1;
            _A[y_row][6] = -(sp.y() * tp.x());
            _A[y_row][7] = -(sp.y() * tp.y());

            _B[y_row][0] = sp.y();

            qDebug() << "A: " << _A[y_row][0] << "," << _A[y_row][1] << "," << _A[y_row][2] << "," << _A[y_row][3] << "," << _A[y_row][4] << "," << _A[y_row][5] << "," << _A[y_row][6] << "," << _A[y_row][7];
            qDebug() << "B: " << _B[y_row][0];
        }

        //Solve for our homography
        JAMA::LU<double> lu(_A);
        Array2D<double> result = lu.solve(_B);
//        JAMA::Cholesky<double> cho(_A);
//        Array1D<double> result = cho.solve(_B);

        _homography[0][0] = result[0][0];
        _homography[0][1] = result[1][0];
        _homography[0][2] = result[2][0];
        _homography[1][0] = result[3][0];
        _homography[1][1] = result[4][0];
        _homography[1][2] = result[5][0];
        _homography[2][0] = result[6][0];
        _homography[2][1] = result[7][0];
        _homography[2][2] = 1;


        //Find the inverse of our homography
//        JAMA::LU<double> h_solver(_homography);
//        Array2D<double> id(_homography.dim1(), _homography.dim2(), 0.0);
//        for (int i = 0; i < _homography.dim1(); i++)
//            id[i][i] = 1;

        // solves A * A_inv = Identity
//        _h_prime = h_solver.solve(id);

    } else {
        //Is it over constrained?

    }

    //Now perform warping with the homography
    Array2D<double> position(3, 1, 0.0);
    position[2][0] = 1.0;
    for(int y=0; y < target->height(); y++) {
        for(int x=0; x < target->width(); x++) {
            QPoint p(x,y);
            position[0][0] = (double) x;
            position[1][0] = (double) y;

            double x_prime = ((_homography[0][0] * x) + (_homography[0][1] * y) + _homography[0][2]) / ((_homography[2][0] * x) + (_homography[2][1] * y) + 1);
            double y_prime = ((_homography[1][0] * x) + (_homography[1][1] * y) + _homography[1][2]) / ((_homography[2][0] * x) + (_homography[2][1] * y) + 1);

            QPoint map_point(qRound(x_prime), qRound(y_prime));

            if ( map_point.x() < source->width() && map_point.x() >= 0 && map_point.y() < source->height() && map_point.y() >= 0) {
//                int x_max = qRound(x_prime + 0.5);
//                int x_min = qRound(x_prime - 0.5);
//                int y_max = qRound(y_prime + 0.5);
//                int y_min = qRound(y_prime - 0.5);

//                double right_diff = qAbs(x_prime - x_max);
//                double left_diff = qAbs(x_prime - x_min);

//                QRgb br = source->pixel(QPoint(x_max, y_max));
//                QRgb bl = source->pixel(QPoint(x_min, y_max));

//                int b_red_val = qRound((qRed(br) * left_diff) + (qRed(bl) * right_diff) );
//                int b_green_val = qRound((qGreen(br) * left_diff) + (qGreen(bl) * right_diff) );
//                int b_blue_val = qRound((qBlue(br) * left_diff) + (qBlue(bl) * right_diff) );

//                QRgb tr = source->pixel(QPoint(x_max, y_min));
//                QRgb tl = source->pixel(QPoint(x_min, y_min));

//                int t_red_val = qRound(qRed(tr) * left_diff + qRed(tl) * right_diff );
//                int t_green_val = qRound(qGreen(tr) * left_diff + qGreen(tl) * right_diff );
//                int t_blue_val = qRound(qBlue(tr) * left_diff + qBlue(tl) * right_diff );

//                //Now do the up and down calc
//                double up_diff = qAbs(y_prime - y_min);
//                double down_diff = qAbs(y_prime, y_max);

//                int final_red = qRound((b_red_val * up_diff) + (t_red_val * down_diff));
//                int final_green = qRound((b_green_val * up_diff) + (t_green_val * down_diff));
//                int final_blue = qRound((b_blue_val * up_diff) + (t_blue_val * down_diff));

//                new_image.setPixel(p, qRgb(final_red, final_green, final_blue));
                new_image.setPixel(p, source->pixel(map_point));
            } else {
                new_image.setPixel(p, target->pixel(p));
            }
        }
    }
    return new_image;
}

QImage ImageStitching::getPaddedImage(QImage *image)
{
    int width = image->width();
    int height = image->height();
    QSize new_size(width * 3, height * 3);

    QImage new_image( Utility::blackImage(new_size) );
    for(int y=0; y < image->height(); y++) {
        for (int x=0; x < image->width(); x++) {
            new_image.setPixel(QPoint(x+width, y+height), image->pixel(QPoint(x,y)));
        }
    }

    return new_image;
}
