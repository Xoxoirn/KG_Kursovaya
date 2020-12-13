#ifndef  MYVECTOR3D_H
#define  MYVECTOR3D_H


#include <math.h>
#include "angle.h"
#include <string>
#include <cstdlib>
#include <stdlib.h> 

//ќбычковенный 3хкомпонентный вектор
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
		coords[0] = 0;
		coords[1] = 0;
		coords[2] = 0;
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

	inline Vector3 operator - (double x)
	{
		Vector3 newV;
		newV.coords[0] = coords[0] - x;
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

	Vector3 operator += (const Vector3& a) {
		return { this->coords[0] += a.coords[0], this->coords[1] += a.coords[1], this->coords[2] += a.coords[2] };
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

	inline double ScalarProizv(const Vector3& vec)
	{
		return this->coords[0] * vec.coords[0] + this->coords[1] * vec.coords[1] + this->coords[2] * vec.coords[2];
	}

	inline Vector3 vectProisvedenie(Vector3& v)
	{
		Vector3 V;
		V.setCoords(coords[1] * v.coords[2] - coords[2] * v.coords[1],
			coords[2] * v.coords[0] - coords[0] * v.coords[2],
			coords[0] * v.coords[1] - coords[1] * v.coords[0]);
		return V;
	}

	inline const double* toArray(void)
	{
		return (coords);
	}

	inline double* ToArray(void)
	{
		return (coords);
	}

	inline void SetCoordX(double a)
	{
		this->coords[0] = a;
	}
	inline void SetCoordY(double a)
	{
		this->coords[1] = a;
	}
	inline void SetCoordZ(double a)
	{
		this->coords[2] = a;
	}
	inline double* GetLinkX()
	{
		return &coords[0];
	}
	inline double* GetLinkY()
	{
		return &coords[1];
	}
	inline double* GetLinkZ()
	{
		return &coords[2];
	}

	inline std::string ToString()
	{
		std::string str = "";
		str += std::to_string(this->coords[0]) + " ";
		str += std::to_string(this->coords[1]) + " ";
		str += std::to_string(this->coords[2]) + "\n";
		return str;
	}

	Vector3 Cross1(Vector3 vVector1, Vector3 vVector2)
	{
		Vector3 vNormal;

		// ≈сли у нас есть 2 вектора (вектор взгл€да и вертикальный вектор),
		// у нас есть плоскость, от которой мы можем вычислить угол в 90 градусов.
		// –ассчет cross'a прост, но его сложно запомнить с первого раза.
		// «начение X дл€ вектора Ч (V1.y * V2.z) Ч (V1.z * V2.y)
		vNormal.SetCoordX((vVector1.Y() * vVector2.Z()) - (vVector1.Z() * vVector2.Y()));

		// «начение Y Ч (V1.z * V2.x) Ч (V1.x * V2.z)
		vNormal.SetCoordY(((vVector1.Z() * vVector2.X()) - (vVector1.X() * vVector2.Z())));

		// «начение Z: (V1.x * V2.y) Ч (V1.y * V2.x)
		vNormal.SetCoordZ(((vVector1.X() * vVector2.Y()) - (vVector1.Y() * vVector2.X())));

		// вернем результат.
		return vNormal;
	}

};

#endif