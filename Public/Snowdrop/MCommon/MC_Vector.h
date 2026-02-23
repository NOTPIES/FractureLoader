#pragma once

#ifndef MC_VECTOR_H
#define MC_VECTOR_H

#include <cmath>

// 2D VECTOR
template <class Type>
class MC_Vector2
{
public:
	// CONSTRUCTORS
	MC_Vector2() : x(0), y(0)
	{
	}

	MC_Vector2(Type aX, Type aZ)
	{
		myX = aX;
		myZ = aZ;
	}

	// ASSIGNMENT & OPERATORS
	MC_Vector2<Type>& operator+=(const MC_Vector2<Type>& v)
	{
		myX += v.myX;
		myZ += v.myZ;
		return *this;
	}

	MC_Vector2<Type>& operator-=(const MC_Vector2<Type>& v)
	{
		myX -= v.myX;
		myZ -= v.myZ;
		return *this;
	}

	MC_Vector2<Type>& operator*=(Type s)
	{
		myX *= s;
		myZ *= s;
		return *this;
	}

	MC_Vector2<Type>& operator/=(Type s)
	{
		myX /= s;
		myZ /= s;
		return *this;
	}

	MC_Vector2<Type> operator+(const MC_Vector2<Type>& v) const { return MC_Vector2<Type>(myX + v.myX, myZ + v.myZ); }
	MC_Vector2<Type> operator-(const MC_Vector2<Type>& v) const { return MC_Vector2<Type>(myX - v.myX, myZ - v.myZ); }
	MC_Vector2<Type> operator*(Type s) const { return MC_Vector2<Type>(myX * s, myZ * s); }
	MC_Vector2<Type> operator/(Type s) const { return MC_Vector2<Type>(myX / s, myZ / s); }

	bool operator==(const MC_Vector2<Type>& v) const { return (myX == v.myX && myZ == v.myZ); }
	bool operator!=(const MC_Vector2<Type>& v) const { return !(*this == v); }

	// BASIC MATH
	Type Length() const { return static_cast<Type>(std::sqrt(static_cast<Type>(myX * myX + myZ * myZ))); }
	Type Length2() const { return (myX * myX + myZ * myZ); }

	MC_Vector2<Type>& Normalize()
	{
		Type len = Length();
		if (len > 0) { *this /= len; }
		return *this;
	}

	Type Dot(const MC_Vector2<Type>& v) const { return v.myX * myX + v.myZ * myZ; }

	// MEMBERS
	union
	{
		Type myX;
		Type myU;
		Type myWidth;
		Type x;
	};

	union
	{
		Type myZ;
		Type myY;
		Type myV;
		Type myHeight;
		Type y;
		Type z;
	};
};

// 3D VECTOR
template <class Type>
class MC_Vector3
{
public:
	// CONSTRUCTORS
	MC_Vector3() : x(0), y(0), z(0)
	{
	}

	MC_Vector3(Type aX, Type aY, Type aZ)
	{
		myX = aX;
		myY = aY;
		myZ = aZ;
	}

	// ASSIGNMENT & OPERATORS
	MC_Vector3<Type>& operator+=(const MC_Vector3<Type>& v)
	{
		myX += v.myX;
		myY += v.myY;
		myZ += v.myZ;
		return *this;
	}

	MC_Vector3<Type>& operator-=(const MC_Vector3<Type>& v)
	{
		myX -= v.myX;
		myY -= v.myY;
		myZ -= v.myZ;
		return *this;
	}

	MC_Vector3<Type>& operator*=(Type s)
	{
		myX *= s;
		myY *= s;
		myZ *= s;
		return *this;
	}

	MC_Vector3<Type>& operator/=(Type s)
	{
		Type inv = static_cast<Type>(1.0) / s;
		myX *= inv;
		myY *= inv;
		myZ *= inv;
		return *this;
	}

	MC_Vector3<Type> operator+(const MC_Vector3<Type>& v) const
	{
		return MC_Vector3<Type>(myX + v.myX, myY + v.myY, myZ + v.myZ);
	}

	MC_Vector3<Type> operator-(const MC_Vector3<Type>& v) const
	{
		return MC_Vector3<Type>(myX - v.myX, myY - v.myY, myZ - v.myZ);
	}

	MC_Vector3<Type> operator*(Type s) const { return MC_Vector3<Type>(myX * s, myY * s, myZ * s); }
	MC_Vector3<Type> operator/(Type s) const { return MC_Vector3<Type>(myX / s, myY / s, myZ / s); }

	bool operator==(const MC_Vector3<Type>& v) const { return (myX == v.myX) && (myY == v.myY) && (myZ == v.myZ); }
	bool operator!=(const MC_Vector3<Type>& v) const { return !(*this == v); }

	// BASIC MATH
	Type Length() const { return static_cast<Type>(std::sqrt(myX * myX + myY * myY + myZ * myZ)); }
	Type Length2() const { return (myX * myX + myY * myY + myZ * myZ); }

	MC_Vector3<Type>& Normalize()
	{
		Type len = Length();
		if (len > 0) { *this /= len; }
		return *this;
	}

	MC_Vector3<Type> Cross(const MC_Vector3<Type>& v) const
	{
		return MC_Vector3<Type>(
			myY * v.myZ - myZ * v.myY,
			myZ * v.myX - myX * v.myZ,
			myX * v.myY - myY * v.myX);
	}

	Type Dot(const MC_Vector3<Type>& v) const { return v.myX * myX + v.myY * myY + v.myZ * myZ; }

	// ACCESS
	const Type operator[](int index) const { return (&x)[index]; }
	Type& operator[](int index) { return (&x)[index]; }

	// MEMBERS
	union
	{
		Type myX;
		Type myR;
		Type x;
		Type r;
	};

	union
	{
		Type myY;
		Type myG;
		Type y;
		Type g;
	};

	union
	{
		Type myZ;
		Type myB;
		Type z;
		Type b;
	};
};

// 4D VECTOR
template <class Type>
class MC_Vector4
{
public:
	// CONSTRUCTORS
	MC_Vector4() : x(0), y(0), z(0), w(0)
	{
	}

	MC_Vector4(Type aX, Type aY, Type aZ, Type aW = Type(1))
	{
		myX = aX;
		myY = aY;
		myZ = aZ;
		myW = aW;
	}

	// OPERATORS
	MC_Vector4<Type>& operator+=(const MC_Vector4<Type>& v)
	{
		myX += v.myX;
		myY += v.myY;
		myZ += v.myZ;
		myW += v.myW;
		return *this;
	}

	MC_Vector4<Type>& operator-=(const MC_Vector4<Type>& v)
	{
		myX -= v.myX;
		myY += v.myY;
		myZ -= v.myZ;
		myW -= v.myW;
		return *this;
	}

	MC_Vector4<Type>& operator*=(Type s)
	{
		myX *= s;
		myY *= s;
		myZ *= s;
		myW *= s;
		return *this;
	}

	MC_Vector4<Type>& operator/=(Type s)
	{
		myX /= s;
		myY /= s;
		myZ /= s;
		myW /= s;
		return *this;
	}

	// BASIC MATH
	Type Length() const { return static_cast<Type>(std::sqrt(myX * myX + myY * myY + myZ * myZ + myW * myW)); }
	Type Length2() const { return (myX * myX + myY * myY + myZ * myZ + myW * myW); }

	MC_Vector4<Type>& Normalize()
	{
		Type len = Length();
		if (len > 0) { *this /= len; }
		return *this;
	}

	Type Dot(const MC_Vector4<Type>& v) const { return v.myX * myX + v.myY * myY + v.myZ * myZ + v.myW * myW; }

	// ACCESS
	const Type operator[](int index) const { return (&x)[index]; }
	Type& operator[](int index) { return (&x)[index]; }

	// MEMBERS
	union
	{
		Type myX;
		Type myR;
		Type myLeft;
		Type x;
		Type r;
	};

	union
	{
		Type myY;
		Type myG;
		Type myTop;
		Type y;
		Type g;
	};

	union
	{
		Type myZ;
		Type myB;
		Type myRight;
		Type z;
		Type b;
	};

	union
	{
		Type myW;
		Type myA;
		Type myBottom;
		Type w;
		Type a;
	};
};

// TYPEDEFS
using MC_Vector2i = MC_Vector2<int>;
using MC_Vector2ui = MC_Vector2<unsigned int>;
using MC_Vector2d = MC_Vector2<double>;
using MC_Vector2f = MC_Vector2<float>;
using MC_Vector2s = MC_Vector2<short>;
using Point2i = MC_Vector2i;
using Point2ui = MC_Vector2ui;
using Point2d = MC_Vector2d;
using Point2f = MC_Vector2f;
using TexCoord = MC_Vector2f;

using MC_Vector3i = MC_Vector3<int>;
using MC_Vector3d = MC_Vector3<double>;
using MC_Vector3f = MC_Vector3<float>;
using MC_Vector3uc = MC_Vector3<unsigned char>;
using MC_Vector3s = MC_Vector3<short>;
using Point3i = MC_Vector3i;
using Point3d = MC_Vector3d;
using Point3f = MC_Vector3f;
using Vertex = MC_Vector3f;
using Normal = MC_Vector3f;
using Color = MC_Vector3f;
using MC_Rgb = MC_Vector3uc;

using MC_Vector4i = MC_Vector4<int>;
using MC_Vector4d = MC_Vector4<double>;
using MC_Vector4f = MC_Vector4<float>;
using MC_Vector4fA =


__declspec(align(16)) MC_Vector4<float>;
using MC_Vector4uc = MC_Vector4<unsigned char>;
using MC_Vector4s = MC_Vector4<short>;
using Point4i = MC_Vector4i;
using Point4d = MC_Vector4d;
using Point4f = MC_Vector4f;
using ColorRGBA = MC_Vector4f;
using MC_Rgba = MC_Vector4uc;

#endif
