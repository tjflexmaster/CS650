#ifndef ARMADILLOEXAMPLES_H
#define ARMADILLOEXAMPLES_H

#include <QRunnable>
#include <armadillo>

using namespace std;
using namespace arma;

class ArmadilloExamples : public QRunnable
{
public:
    ArmadilloExamples();
    void run();

private:
    void covariance();
    void inverse();
    void multiplication();
};

#endif // ARMADILLOEXAMPLES_H
