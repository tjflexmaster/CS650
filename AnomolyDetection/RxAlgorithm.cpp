#include "RxAlgorithm.h"
#include <QImage>
#include <armadillo>
#include <iostream>
#include <qmath.h>
#include <QHash>
#include <QMap>
#include <QMultiMap>
#include <fftw3.h>
#include "boost/math/special_functions/gamma.hpp"

RxAlgorithm::RxAlgorithm()
{
    pixel_labels = new QHash<QPoint, int>();
    _sensitivity = 0.80;
}

RxAlgorithm::~RxAlgorithm()
{
    delete _image;
    delete pixel_labels;
}

void RxAlgorithm::run()
{
    pixel_labels->clear();
    cout << "Processing Started" << endl;
    QRgb black = qRgb(0,0,0);
    new_image = QImage(_image->size(), QImage::Format_ARGB32);
    final_image = *_image;
    QList<QPoint> outlier_pixels;

    //We need to know what the padding will be so we can make all arrays the correct size
    _padding = _max_radius;

    int height = _image->height() + _padding;
    int width = _image->width() + _padding;
    int new_dim = max(height,width);
    int n_dim_counter = 1;
    while(new_dim > qPow(2, n_dim_counter)) {
        n_dim_counter++;
    }
    new_dim = qPow(2,n_dim_counter);

    int N = new_dim * new_dim;
//    int filter_N = qPow((_max_radius * 2) + 1, 2) - qPow((_min_radius * 2) - 1, 2);
    fftw_plan planR, planG, planB, planK;
    fftw_complex *outR, *outG, *outB, *outK, *convR, *convG, *convB;
    double *inR, *inG, *inB, *inK, *resultR, *resultG, *resultB;

    //Allocate arrays.
    inR = (double*) fftw_malloc(sizeof(double) * N);
    inG = (double*) fftw_malloc(sizeof(double) * N);
    inB = (double*) fftw_malloc(sizeof(double) * N);
    inK = (double*) fftw_malloc(sizeof(double) * N);

    outR = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    outG = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    outB = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    outK = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

    convR = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    convG = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    convB = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

    resultR = (double*) fftw_malloc(sizeof(double) * N);
    resultG = (double*) fftw_malloc(sizeof(double) * N);
    resultB = (double*) fftw_malloc(sizeof(double) * N);

    mat C( _image->height() * _image->width(), 3);
    QList<mat > Z_list;
    int counter = 0;
    //Perform RX algorithm only on non border pixels
    for( int y=0; y < new_dim; y++ ) {
        for ( int x=0; x < new_dim; x++ ) {
            int index = (y * new_dim) + x;
            if ( x <_image->width() &&
                 y < _image->height() ) {

                QPoint p(x, y);
                QRgb main_rgb = _image->pixel(p);
                int red = qRed(main_rgb);
                int green = qGreen(main_rgb);
                int blue = qBlue(main_rgb);
                C(counter, 0) = red;
                C(counter, 1) = green;
                C(counter, 2) = blue;
                counter++;

                //Fill in FFTW in arrays with pixel channel color
                inR[index] = red;
                inG[index] = green;
                inB[index] = blue;
                inK[index] = 0; //Initialize the kernel
            } else {
                //Padding around the entire image.
                inR[index] = 0;
                inG[index] = 0;
                inB[index] = 0;
                inK[index] = 0;
            }
        }
    }

    //Create Image Kernel  //this modifies the corners
//    constructBlurKernel(inK, _padding, new_dim);
    constructRxKernel(inK, new_dim);

    //Forward plans.
    planR = fftw_plan_dft_r2c_2d(new_dim, new_dim, inR, outR, FFTW_ESTIMATE);
    planG = fftw_plan_dft_r2c_2d(new_dim, new_dim, inG, outG, FFTW_ESTIMATE);
    planB = fftw_plan_dft_r2c_2d(new_dim, new_dim, inB, outB, FFTW_ESTIMATE);
    planK = fftw_plan_dft_r2c_2d(new_dim, new_dim, inK, outK, FFTW_ESTIMATE);

    //Forward FFT.
    fftw_execute(planR);
    fftw_execute(planG);
    fftw_execute(planB);
    fftw_execute(planK);

    //Multiply channels with kernel
    double scale = 1.0;
    for( int i=0; i<N; i++ ) {
        convR[i][0] = ((outR[i][0] * outK[i][0]) - (outR[i][1] * outK[i][1])) * scale;
        convR[i][1] = ((outR[i][0] * outK[i][1]) + (outR[i][1] * outK[i][0])) * scale;
        convG[i][0] = ((outG[i][0] * outK[i][0]) - (outG[i][1] * outK[i][1])) * scale;
        convG[i][1] = ((outG[i][0] * outK[i][1]) + (outG[i][1] * outK[i][0])) * scale;
        convB[i][0] = ((outB[i][0] * outK[i][0]) - (outB[i][1] * outK[i][1])) * scale;
        convB[i][1] = ((outB[i][0] * outK[i][1]) + (outB[i][1] * outK[i][0])) * scale;

    }

    //Backward plans.
    planR = fftw_plan_dft_c2r_2d(new_dim, new_dim, convR, resultR, FFTW_ESTIMATE);
    planG = fftw_plan_dft_c2r_2d(new_dim, new_dim, convG, resultG, FFTW_ESTIMATE);
    planB = fftw_plan_dft_c2r_2d(new_dim, new_dim, convB, resultB, FFTW_ESTIMATE);

    //Backward fft
    fftw_execute(planR);
    fftw_execute(planG);
    fftw_execute(planB);

    //Show results
    for( int y=0; y < new_dim; y++ ) {
        for ( int x=0; x < new_dim; x++ ) {
            int index = (y * new_dim) + x;
            if ( x < _image->width() &&
                 y < _image->height() ) {

                mat Z(3,1);
                Z(0,0) = inR[index] - resultR[index]/N;
                Z(1,0) = inG[index] - resultG[index]/N;
                Z(2,0) = inB[index] - resultB[index]/N;
                Z_list.append(Z);

//                QPoint p(x, y);
//                QRgb final_color = qRgb(resultR[index] / N, resultG[index] / N, resultB[index] / N);
//                new_image.setPixel(p, final_color);
            }
        }
    }

    mat S = cov(C); //Covariance Matrix
    mat S_inv = inv(S);
    cout << "Covariance Found " << S_inv << endl;

    //Calculate the mahalanobis distance
    int i = 0;
    foreach(mat Z, Z_list) {
        int x = i % _image->width();
        int y = i/_image->width();
        QPoint p(x, y);
        mat val = Z.t() * S_inv * Z;
        double m_dist = qSqrt(val.at(0,0));

        double prob_of_inc = gamma_p(3/2, m_dist/2);

        if ( prob_of_inc > _sensitivity ) {
            new_image.setPixel(p, qRgb(255,255,255));
        } else {
            new_image.setPixel(p, qRgb(0,0,0));
        }

        i++;
    }

    cout << "Connecting Components..." << endl;

    QRgb bg_color = qRgb(0,0,0);

    QList<QPoint> neighbors;
    neighbors.append(QPoint(-1,0)); //w
    neighbors.append(QPoint(0,-1)); //n

    int label_count = 0;

    //First Pass
    for(int y=0; y < new_image.height(); y++) {
        for ( int x=0; x < new_image.width(); x++) {
            QPoint p(x, y);

            QRgb p_color = new_image.pixel(p);

            //Are we background?
            if ( p_color == bg_color ) {
                continue;
            }

            QList<int> neighbor_labels;
            int min_label = label_count + 1;
            foreach(QPoint q, neighbors) {
                QPoint neighbor = p+q;
                if ( new_image.valid(neighbor) ) {
                    QRgb color = new_image.pixel(neighbor);
                    if ( color == p_color ) {
                        int label = pixel_labels->value(neighbor);
                        int label_min = label_pairings.value(label, label);
                        neighbor_labels.append(label);
                        if ( label_min < min_label ) {
                            min_label = label_min;
                        }
                    }
                }
            }

            //If no matching neighbors then use the min and update the count
            if ( neighbor_labels.isEmpty() ) {
                label_count++;
                label_pairings.insert(label_count, label_count);
            } else if ( neighbor_labels.count() > 1 ) {
                foreach(int l, neighbor_labels) {
                    int neighbor_min = label_pairings.value(l, label_count);
                    if ( min_label < neighbor_min ) {
                        //If our neighbors are connected then we should store the minimum label they
                        //are connected too.
                        label_pairings.insert(l, min_label);
                    }
                }
            }

            //Set the label
            pixel_labels->insert(p, min_label);

        }
    }

    QList<int> used_labels;
    for(int y=0; y < new_image.height(); y++) {
        for ( int x=0; x < new_image.width(); x++) {

            QPoint p(x, y);
            if ( pixel_labels->contains(p) ) {
                int label = pixel_labels->value(p);
                int min_label = label_pairings.value(label, label);
                while(label != min_label) {
                    label = min_label;
                    min_label =  label_pairings.value(label, label);
                }

                int label_index = 0;
                if ( !used_labels.contains(min_label) ) {
                    used_labels.append(min_label);
                    label_index = used_labels.count() - 1;
                } else {
                    label_index = used_labels.indexOf(min_label);
                }

                _label_sets.insert(label_index, p);
                //mark pixel
                new_image.setPixel(p, _image->pixel(p));
            }
        }
    }

    cout << "Processing Components..." << endl;

    QRgb white = qRgb(0,255,0);
    int key = 0;
    bool loop = true;
    while ( loop ) {

        QList<QPoint> component_points = _label_sets.values(key);
        //Initialize BR values
        int max_x = -1, max_y = -1;
        int min_x = 9999, min_y = 9999;

        foreach(QPoint p, component_points) {
            //Initialize Bounding Rect Values
            if ( p.x() > max_x )
                max_x = p.x();

            if ( p.y() > max_y )
                max_y = p.y();

            if ( p.x() < min_x )
                min_x = p.x();

            if ( p.y() < min_y )
                min_y = p.y();

        } //end foreach

        int xdiff = max_x - min_x;
        int ydiff = max_y - min_y;

        if ( xdiff > _anomoly_width || ydiff > _anomoly_width ) {
            QPoint center_point(max_x - (xdiff/2), max_y - (ydiff/2));

            QList<QPoint> circle_points = getCircleTable( (max(xdiff, ydiff) / 2) + 10 );
            foreach(QPoint t, circle_points) {
                final_image.setPixel(center_point + t, white);
            }
        }

        key++;
        if ( !_label_sets.contains(key) ) {
            loop = false;
        }

    } //end while

    cout << "Processing Complete" << endl;
    emit processingComplete();
}

void RxAlgorithm::constructKernel()
{
    //Construct the RX convolution kernel
    for ( int j=0; j < 2*_max_radius; j++ ) {
        for ( int i=0; i < 2*_max_radius; i++ ) {
            if ( (j > _max_radius - _min_radius && j < _max_radius + _min_radius) ||
                (i > _max_radius - _min_radius && i < _max_radius + _min_radius) ) {
                continue;
            }
            _kernel.append(QPoint(i - _max_radius, j - _max_radius));
        }
    }
}

QImage RxAlgorithm::getImage()
{
    return final_image;
}

void RxAlgorithm::setImage(QImage *image)
{
    _image = image;
}

void RxAlgorithm::setKernelProperties(int max_radius = 53, int min_radius = 13)
{
    if ( max_radius < min_radius )
        max_radius = min_radius;

    _max_radius = max_radius;
    _min_radius = min_radius;

    //Use odd values for the radius
    if ( _max_radius % 2 == 0 ) {
        _max_radius++;
    }

    if ( _min_radius % 2 == 0 ) {
        _min_radius++;
    }
}

void RxAlgorithm::setAlgorithmProperties(double sensitivity)
{
    _sensitivity = sensitivity;
}

void RxAlgorithm::setAnomolyProperties(int pixel_width, bool red, bool green, bool blue)
{
    _anomoly_width = pixel_width;
}

void RxAlgorithm::constructBlurKernel(double *kernel_array, int padding, int dimension)
{
    int blur_size = (padding * 2) + 1;


    for(int j=0; j<blur_size; j++) {
        for(int i=0; i<blur_size; i++) {
            int val_x = i - padding;
            int val_y = j - padding;

            if ( val_x < 0 ) {
                val_x = dimension + val_x;
            }

            if ( val_y < 0 ) {
                val_y = dimension + val_y;
            }

            int index = (val_y * dimension) + val_x;
            kernel_array[index] = 1.0/qPow(blur_size,2);
        }
    }
}

void RxAlgorithm::constructRxKernel(double *kernel_array, int dimension)
{
    //Create the kernel array
    int kernel_size = (2 * _max_radius) + 1;
    for(int j=0; j < kernel_size; j++) {
        for(int i=0; i < kernel_size; i++) {
            int val_x = i - _max_radius;
            int val_y = j - _max_radius;

            bool set_value = false;
            //Only update values that are in the desired regions
            if ( val_x <= -_min_radius ||
                 val_x >= _min_radius ) {

                set_value = true;
            } else {
                if ( val_y <= -_min_radius || val_y >= _min_radius ) {
                    set_value = true;
                }
            }

            if ( !set_value ) {
                continue;
            }

            if ( val_x < 0 ) {
                val_x = dimension + val_x;
            }

            if ( val_y < 0 ) {
                val_y = dimension + val_y;
            }

            int index = (val_y * dimension) + val_x;
            kernel_array[index] = 1.0/qPow(kernel_size,2);

        }
    }
}

QList<QPoint> RxAlgorithm::getCircleTable(int radius)
{
    QList<QPoint> kernel;
    int f = 1 - radius;
    int ddF_x = 1;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    kernel.append(QPoint(0, radius));
    kernel.append(QPoint(0, -radius));
    kernel.append(QPoint(radius, 0));
    kernel.append(QPoint(-radius, 0));

    while ( x < y ) {
        if ( f >= 0 ) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }

        x++;
        ddF_x += 2;
        f += ddF_x;
        kernel.append(QPoint(x, y));
        kernel.append(QPoint(-x, y));
        kernel.append(QPoint(x, -y));
        kernel.append(QPoint(-x, -y));
        kernel.append(QPoint(y, x));
        kernel.append(QPoint(-y, x));
        kernel.append(QPoint(y, -x));
        kernel.append(QPoint(-y, -x));
    }

    return kernel;
}
