#ifndef TOOLS_H
#define TOOLS_H

#include "ros/ros.h"
#include <cmath>
#include <Eigen/Eigen>
#include <Eigen/Dense>
#include <random>
#include <chrono>

using namespace std;

const float PI = 3.14159265359F;

struct Point{
	float x;
	float y;
	float z;
};

struct Line{
	Point start;
	Point end;
};

struct Color{
	float r;
	float g;
	float b;
};

float randomFloat(float lower, float upper)
{
	return (lower + (static_cast<float>(rand()) / RAND_MAX) * (upper - lower));
}

double randomDouble(double lower, double upper)
{
	return (lower + (static_cast<double>(rand()) / RAND_MAX) * (upper - lower));
}

Eigen::Vector3d setVector(Point a, Point b)
{
	Eigen::Vector3d v(b.x-a.x, b.y-a.y, b.z-a.z);
	return v;
}

double gaussian_noise(double mu, double sigma)
{
    	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	
	std::default_random_engine generate(seed);
	std::normal_distribution<double> distribution(mu, sigma);
	
	return distribution(generate);
}

Point setPoint(float x, float y, float z)
{
	Point p;
	p.x = x;
	p.y = y;
	p.z = z;
	return p;
}

Color fb_Color(float r, float g, float b)
{
	Color c;
	c.r = r;
	c.b = b;
	c.g = g;
	return c;
}

#endif
