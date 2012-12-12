#include "GaussianCalc.h"
#include "Utility.h"
#include <qmath.h>
#include <QDebug>

GaussianCalc::GaussianCalc(qreal sigma, qreal x, qreal m)
{
    _sigma = sigma;
    _x = x;
    _m = m;
}

//void GaussianCalc::run()
//{
//    qreal prob = 0;
//    qreal _a = 1 / _sigma * qSqrt( 2 * Utility::PI );
//    qreal _c = 2 * qPow(_sigma, 2);

//    qreal _b = qPow((_x - _m), 2) / _c;
//    //Run Gaussian
//    prob = _a * qExp( -(_b));

//    qDebug() << "Thread prob: " << prob;

//}
