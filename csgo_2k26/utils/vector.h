#pragma once
#pragma once
#include <cmath>
#include <cfloat>

inline float BitsToFloat(unsigned int i) {
	return *reinterpret_cast<float*>(&i);
}

#define FLOAT32_NAN_BITS     (unsigned long)0x7FC00000
#define FLOAT32_NAN          BitsToFloat( FLOAT32_NAN_BITS )
#define VEC_T_NAN FLOAT32_NAN


class vec2
{
public:
	float x{ 0.0f };
	float y{ 0.0f };

public:
	vec2()
	{
		x = 0.0f;
		y = 0.0f;
	}

	vec2(const float x, const float y)
	{
		this->x = x;
		this->y = y;
	}

	vec2(float* const v)
	{
		x = v[0];
		y = v[1];
	}

	vec2(const float* const v)
	{
		x = v[0];
		y = v[1];
	}

	vec2(const vec2& v)
	{
		x = v.x;
		y = v.y;
	}

public:
	float& operator[](const int i)
	{
		return ((float*)this)[i];
	}

	float operator[](const int i) const
	{
		return ((float*)this)[i];
	}

public:
	vec2& operator=(const vec2& v)
	{
		x = v.x;
		y = v.y;

		return *this;
	}

	vec2& operator+=(const vec2& v)
	{
		x += v.x;
		y += v.y;

		return *this;
	}

	vec2& operator-=(const vec2& v)
	{
		x -= v.x;
		y -= v.y;

		return *this;
	}

	vec2& operator*=(const vec2& v)
	{
		x *= v.x;
		y *= v.y;

		return *this;
	}

	vec2& operator/=(const vec2& v)
	{
		x /= v.x;
		y /= v.y;

		return *this;
	}

	vec2& operator+=(const float v)
	{
		x += v;
		y += v;

		return *this;
	}

	vec2& operator-=(const float v)
	{
		x -= v;
		y -= v;

		return *this;
	}

	vec2& operator*=(const float v)
	{
		x *= v;
		y *= v;

		return *this;
	}

	vec2& operator/=(const float v)
	{
		x /= v;
		y /= v;

		return *this;
	}

	vec2 operator+(const vec2& v) const
	{
		return { x + v.x, y + v.y };
	}

	vec2 operator-(const vec2& v) const
	{
		return { x - v.x, y - v.y };
	}

	vec2 operator*(const vec2& v) const
	{
		return { x * v.x, y * v.y };
	}

	vec2 operator/(const vec2& v) const
	{
		return { x / v.x, y / v.y };
	}

	vec2 operator+(const float v) const
	{
		return { x + v, y + v };
	}

	vec2 operator-(const float v) const
	{
		return { x - v, y - v };
	}

	vec2 operator*(const float v) const
	{
		return { x * v, y * v };
	}

	vec2 operator/(const float v) const
	{
		return { x / v, y / v };
	}

public:
	void set(const float x = 0.0f, const float y = 0.0f)
	{
		this->x = x;
		this->y = y;
	}

	bool isZero() const
	{
		constexpr float epsilon{ 0.01f };

		return x > -epsilon && x < epsilon && y > -epsilon && y < epsilon;
	}

public:
	float length() const
	{
		return sqrtf(x * x + y * y);
	}

	float lengthSqr() const
	{
		return x * x + y * y;
	}

	float distTo(const vec2& v) const
	{
		return (*this - v).length();
	}

	float distToSqr(const vec2& v) const
	{
		return (*this - v).lengthSqr();
	}

	float dot(const vec2& v) const
	{
		return x * v.x + y * v.y;
	}
};

class Vector
{
public:
	float x{ 0.0f };
	float y{ 0.0f };
	float z{ 0.0f };

public:
	Vector()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	Vector(const float x, const float y, const float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector(float* const v)
	{
		x = v[0];
		y = v[1];
		z = v[2];
	}

	Vector(const float* const v)
	{
		x = v[0];
		y = v[1];
		z = v[2];
	}

	Vector(const Vector& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

public:
	float& operator[](const int i)
	{
		return ((float*)this)[i];
	}

	float operator[](const int i) const
	{
		return ((float*)this)[i];
	}

public:
	Vector& operator=(const Vector& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;

		return *this;
	}

	Vector& operator+=(const Vector& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}

	Vector& operator-=(const Vector& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}

	Vector& operator*=(const Vector& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;

		return *this;
	}

	Vector& operator/=(const Vector& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;

		return *this;
	}

	Vector& operator+=(const float v)
	{
		x += v;
		y += v;
		z += v;

		return *this;
	}

	Vector& operator-=(const float v)
	{
		x -= v;
		y -= v;
		z -= v;

		return *this;
	}

	Vector& operator*=(const float v)
	{
		x *= v;
		y *= v;
		z *= v;

		return *this;
	}

	Vector& operator/=(const float v)
	{
		x /= v;
		y /= v;
		z /= v;

		return *this;
	}

	Vector operator+(const Vector& v) const
	{
		return { x + v.x, y + v.y, z + v.z };
	}

	Vector operator-(const Vector& v) const
	{
		return { x - v.x, y - v.y, z - v.z };
	}

	Vector operator*(const Vector& v) const
	{
		return { x * v.x, y * v.y, z * v.z };
	}

	Vector operator/(const Vector& v) const
	{
		return { x / v.x, y / v.y, z / v.z };
	}

	Vector operator+(const float v) const
	{
		return { x + v, y + v, z + v };
	}

	Vector operator-(const float v) const
	{
		return { x - v, y - v, z - v };
	}

	Vector operator*(const float v) const
	{
		return { x * v, y * v, z * v };
	}

	Vector operator/(const float v) const
	{
		return { x / v, y / v, z / v };
	}

public:
	void set(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	bool isZero() const
	{
		constexpr float epsilon{ 0.01f };

		return x > -epsilon && x < epsilon && y > -epsilon && y < epsilon && z > -epsilon && z < epsilon;
	}

public:
	float length() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	float lengthSqr() const
	{
		return x * x + y * y + z * z;
	}

	float normalize()
	{
		const float len{ length() };
		const float len_normal{ 1.0f / (FLT_EPSILON + len) };

		x = x * len_normal;
		y = y * len_normal;
		z = z * len_normal;

		return len;
	}

	Vector normalized()
	{
		normalize();

		return *this;
	}

	float length2D() const
	{
		return sqrtf(x * x + y * y);
	}

	float length2DSqr() const
	{
		return x * x + y * y;
	}

	float distTo(const Vector& v) const
	{
		return (*this - v).length();
	}

	float distToSqr(const Vector& v) const
	{
		return (*this - v).lengthSqr();
	}

	float dot(const Vector& v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}
public:
	Vector cross(const Vector& v) const
	{
		return { y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x };
	}

	Vector scale(const float scale)
	{
		return { x * scale, y * scale, z * scale };
	}

	Vector normalizeAngles()
	{
		auto normalizeAngle = [&](float& ang) -> void
			{
				float f = (ang * (1.f / 360.f)) + 0.5f;
				int i = (int)f;
				float fi = (float)i;
				f = (f < 0.f && f != fi) ? fi - 1.f : fi;
				ang -= f * 360.f;
			};

		normalizeAngle(x);
		normalizeAngle(y);
		normalizeAngle(z);

		return *this;
	}
};

class Vector4D {
public:
	float x{};
	float y{};
	float z{};
	float w{};
};

struct matrix3x4_t {
	matrix3x4_t() = default;

	matrix3x4_t(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23) {
		m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
	}

	void Init(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector& vecOrigin) {
		m_flMatVal[0][0] = xAxis.x; m_flMatVal[0][1] = yAxis.x; m_flMatVal[0][2] = zAxis.x; m_flMatVal[0][3] = vecOrigin.x;
		m_flMatVal[1][0] = xAxis.y; m_flMatVal[1][1] = yAxis.y; m_flMatVal[1][2] = zAxis.y; m_flMatVal[1][3] = vecOrigin.y;
		m_flMatVal[2][0] = xAxis.z; m_flMatVal[2][1] = yAxis.z; m_flMatVal[2][2] = zAxis.z; m_flMatVal[2][3] = vecOrigin.z;
	}

	matrix3x4_t(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector& vecOrigin) {
		Init(xAxis, yAxis, zAxis, vecOrigin);
	}

	inline void Invalidate(void) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				m_flMatVal[i][j] = VEC_T_NAN;
			}
		}
	}

	float* operator[](int i) { return m_flMatVal[i]; }
	const float* operator[](int i) const { return m_flMatVal[i]; }
	float* Base() { return &m_flMatVal[0][0]; }
	const float* Base() const { return &m_flMatVal[0][0]; }
	Vector Origin() { return { m_flMatVal[0][3], m_flMatVal[1][3], m_flMatVal[2][3] }; }

	float m_flMatVal[3][4];
};

typedef float matrix4x4_t[4][4];

class VMatrix
{
private:
	Vector m[4][4]{};

public:
	inline const matrix3x4_t& As3x4() const
	{
		return *((const matrix3x4_t*)this);
	}
};

class QAngle {
public:
	float pitch, yaw, roll = 0.0f;

	QAngle() : pitch(0), yaw(0), roll(0) {}
	QAngle(float p, float y, float r) : pitch(p), yaw(y), roll(r) {}
};

class vector_2d {
public:
	float m_x, m_y;
	vector_2d() {}
	vector_2d(float x, float y) {
		m_x = x;
		m_y = y;
	}
	vector_2d(vec2 other) {
		m_x = other.x;
		m_y = other.y;
	}
};

struct Vertex_t {
	Vertex_t() {}
	Vertex_t(const vector_2d& pos, const vector_2d& coord = vector_2d(0, 0)) {
		m_Position = pos;
		m_TexCoord = coord;
	}
	void Init(const vector_2d& pos, const vector_2d& coord = vector_2d(0, 0)) {
		m_Position = pos;
		m_TexCoord = coord;
	}

	vector_2d m_Position;
	vector_2d m_TexCoord;
};

class Quaternion {
public:
	Quaternion() = default;
	Quaternion(float ix, float iy, float iz, float iw) : x(ix), y(iy), z(iz), w(iw) {}
	float* Base() { return (float*)this; }
	const float* Base() const { return (float*)this; }

	bool operator==(const Quaternion& src) const;
	bool operator!=(const Quaternion& src) const;
	float operator[](int i) const;
	float& operator[](int i);

	float x, y, z, w;
};