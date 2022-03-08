#ifndef TARGET_H
#define TARGET_H

class Target
{
public:
    double x;
    double y;

    double vx;
    double vy;

    KalmanFilter4D tracker;
};

#endif
