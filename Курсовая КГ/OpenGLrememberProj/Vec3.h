#include <math.h>

#include <math.h>
#include <stdio.h>
#define PI 3.14159265

class angle
{
	double value;

	void check()
	{
		if (value < 0)
		{
			int n = abs((int)(value / (2 * PI)));
			value += (n + 1) * 2 * PI;
		}
		else
		{
			double nn = value / (2 * PI);
			int n = abs((int)(nn));
			if ((nn - n) > 0.999999)
				n++;
			value -= (n) * 2 * PI;
			//printf("%f\n", value);
			//char buf[30];
			//sprintf_s(buf, "%f\n", value);
			//OutputDebugString(buf);
		}
	}

public:
	angle normolize()
	{
		angle a = *this;
		a.check();
		return a;
	}
	double getValue()
	{
		return value;
	}
	angle operator = (angle& a)
	{
		value = a.value;
		return (*this);
	}

	angle operator = (double a)
	{
		value = a;
		//check();
		return (*this);
	}

	angle operator + (angle& a)
	{
		angle a1;
		a1 = value + a.value;
		//a1.check();
		return a1;
	}

	angle operator +(double a)
	{
		angle a1;
		a1.value = value + a;
		//a1.check();
		return a1;
	}

	angle operator - (angle& a)
	{
		angle a1;
		a1 = value - a.value;
		//a1.check();
		return a1;
	}

	angle operator -(double a)
	{
		angle a1;
		a1.value = value - a;
		//a1.check();
		return a1;
	}

	angle operator / (double& a)
	{
		angle a1;
		a1.value = value / a;
		//a1.check();
		return a1;
	}

	angle operator / (angle& a)
	{
		angle a1;
		a1.value = value / a.value;
		//a1.check();
		return a1;
	}
	operator double()
	{
		return value;
	}
};

//Обычковенный 3хкомпонентный вектор
class Vector3
{
	double coords[3];

public:

	inline double X()
	{
		return coords[0];
	}
	inline double Y()
	{
		return coords[1];
	}
	inline double Z()
	{
		return coords[2];
	}

	//перевод из сферических координат
	inline void fromSpherical(angle& eta, angle& fi, double R)
	{
		coords[0] = R * sin(eta) * cos(fi);
		coords[1] = R * sin(eta) * sin(fi);
		coords[2] = R * cos(eta);
	}

	inline void setCoords(double x, double y, double z)
	{
		coords[0] = x;
		coords[1] = y;
		coords[2] = z;
	}

	inline Vector3(angle& eta, angle& fi, double R)
	{
		fromSpherical(eta, fi, R);
	}
	inline Vector3(double x, double y, double z)
	{
		coords[0] = x;
		coords[1] = y;
		coords[2] = z;
	}
	inline Vector3()
	{

	}

	inline Vector3 operator + (Vector3& vec)
	{
		Vector3 newV;
		newV.coords[0] = coords[0] + vec.coords[0];
		newV.coords[1] = coords[1] + vec.coords[1];
		newV.coords[2] = coords[2] + vec.coords[2];
		return newV;
	}

	inline Vector3 operator - (Vector3& vec)
	{
		Vector3 newV;
		newV.coords[0] = coords[0] - vec.coords[0];
		newV.coords[1] = coords[1] - vec.coords[1];
		newV.coords[2] = coords[2] - vec.coords[2];
		return newV;
	}

	inline Vector3 operator * (double k)
	{
		Vector3 newV;
		newV.coords[0] = coords[0] * k;
		newV.coords[1] = coords[1] * k;
		newV.coords[2] = coords[2] * k;
		return newV;
	}

	inline void operator = (Vector3 vec)
	{
		coords[0] = vec.coords[0];
		coords[1] = vec.coords[1];
		coords[2] = vec.coords[2];
	}

	inline double length()
	{
		return (sqrt(coords[0] * coords[0] + coords[1] * coords[1] + coords[2] * coords[2]));
	}

	inline Vector3 normolize()
	{
		Vector3 newV;
		double l = length();
		newV.setCoords(coords[0] / l, coords[1] / l, coords[2] / l);
		return newV;
	}


	inline Vector3 vectProisvedenie(Vector3& v)
	{
		Vector3 V;
		V.setCoords(coords[1] * v.coords[2] - coords[2] * v.coords[1],
			coords[2] * v.coords[0] - coords[0] * v.coords[2],
			coords[0] * v.coords[1] - coords[1] * v.coords[0]);
		return V;
	}

	inline double ScalarProizv(const Vector3& vec)
	{
		return this->coords[0] * vec.coords[0] + this->coords[1] * vec.coords[1] + this->coords[2] * vec.coords[2];
	}

	inline const double* toArray(void)
	{
		return (coords);
	}
};
