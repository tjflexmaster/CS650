#ifndef GAUSSIANCALC_H
#define GAUSSIANCALC_H

#include <QRunnable>
#include <QObject>
#include <QDebug>

class GaussianCalc : public QRunnable
{

public:
    GaussianCalc(qreal sigma, qreal x, qreal m);
    void run()
    {
        qDebug() << "Hello from thread";
    }
    
    
public slots:

private:
    qreal _sigma;
    qreal _x;
    qreal _m;
    
};

#endif // GAUSSIANCALC_H
