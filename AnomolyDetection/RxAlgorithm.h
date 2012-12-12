#ifndef RXALGORITHM_H
#define RXALGORITHM_H

#include <QRunnable>
#include <QObject>
#include <QImage>
#include <QList>
#include <QPoint>
#include <QHash>
#include <QMap>
#include <QMultiMap>
#include <armadillo>
#include <fftw3.h>
#include "boost/math/special_functions/gamma.hpp"

using namespace std;
using namespace arma;
using namespace boost::math;

class RxAlgorithm : public QObject, public QRunnable
{
    Q_OBJECT

public:
    RxAlgorithm();
    ~RxAlgorithm();
    void run();
    QImage getImage();
//    void setParameters(QImage *image, int max_radius, int min_radius);
    void setImage(QImage *image);
    void setKernelProperties(int max_radius, int min_radius);
    void setAlgorithmProperties(double sensitivity);
    void setAnomolyProperties(int pixel_width, bool red, bool green, bool blue);

signals:
    void processingComplete();

private:
    void constructKernel();
    void constructBlurKernel(double *kernel_array, int padding, int dimension);
    void constructRxKernel(double *kernel_array, int dimension);
    QList<QPoint> getCircleTable(int radius);

    QImage *_image;
    QImage new_image;
    QImage final_image;
    QList<QPoint> _kernel;
    int _max_radius;
    int _min_radius;
    int _padding;
    double _sensitivity;
    int _anomoly_width;

    QHash<QPoint, int> *pixel_labels;
    QMultiMap<int, QPoint> _label_sets;
    QMap<int, int> label_pairings;

};

inline uint qHash(const QPoint &key)
{
    QString val = QString("%1,%2").arg(key.x(), key.y());
    return qHash(val);
}

#endif // RXALGORITHM_H
