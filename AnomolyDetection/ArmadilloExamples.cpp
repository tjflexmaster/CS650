#include "ArmadilloExamples.h"
#include <armadillo>

ArmadilloExamples::ArmadilloExamples()
{
}

void ArmadilloExamples::run()
{
    covariance();
    inverse();
    multiplication();
}

void ArmadilloExamples::covariance()
{
    mat X = randu<mat>(4,5);
    mat Y = randu<mat>(4,5);

    mat C = cov(X,Y);
    cout << C << endl;
}

void ArmadilloExamples::inverse()
{
    mat A = randu<mat>(5,5);
    mat B = inv(A);

    cout << B << endl;
}

void ArmadilloExamples::multiplication()
{
    mat X = randu<mat>(1,4);
    mat Z = X * X.t();
    cout << Z << endl;
}
