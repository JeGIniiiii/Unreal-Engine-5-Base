#pragma once
#include "common.h"

// https://dumpspace.spuckwaffel.com/Games/?hash=0cdf70b9&type=classes&idx=AAbilitySystemDebugHUD
namespace UE5
{
	constexpr std::uint64_t GWORLD = 0x8FEFAC8;
	constexpr std::uint64_t GNAMES = 0x8DB8D40;

	constexpr std::uint64_t ACTORARRAY = 0xA0;
	constexpr std::uint64_t ACTORCOUNT = 0xA8;

	constexpr std::uint64_t GAMEINSTANCE = 0x1d8;
	constexpr std::uint64_t GAMESTATE = 0x160;

	constexpr std::uint64_t PLAYERARRAY = 0x2a8;
	constexpr std::uint64_t PLAYERCOUNT = 0x2a8 + 0x8;

	constexpr std::uint64_t LOCALPLAYER = 0x38;
	constexpr std::uint64_t PLAYERCONTROLLER = 0x30;

	constexpr std::uint64_t PLAYERCAMERAMANAGER = 0x348;
	constexpr std::uint64_t CAMERACACHEPRIVATE = 0x1390;

	constexpr std::uint64_t APAWN = 0x308;
	constexpr std::uint64_t ROOTCOMPONENT = 0x1a0;
	constexpr std::uint64_t RELATIVELOCATION = 0x128;
}

class Vector3
{
public:
    double x, y, z;

    Vector3(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

    bool isValidLocation()
    {
        return x != 0 && y != 0 && z != 0;
    }

    double dot(const Vector3& other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3 operator-(const Vector3& other) const
    {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    Vector3 operator+(const Vector3& other) const
    {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vector3& operator-=(const Vector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vector3 operator*(double scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    Vector3 operator/(double scalar) const {
        if (scalar != 0) {
            return Vector3(x / scalar, y / scalar, z / scalar);
        }
        return *this;
    }

    Vector3& operator*=(double scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    Vector3& operator/=(double scalar) {
        if (scalar != 0) {
            x /= scalar;
            y /= scalar;
            z /= scalar;
        }
        return *this;
    }

    double magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vector3 normalize() const {
        double mag = magnitude();
        if (mag > 0) {
            return *this / mag;
        }
        return *this;
    }

    Vector3 Add(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    bool IsValid() {
        return !std::isnan(x) && !std::isinf(x) &&
            !std::isnan(y) && !std::isinf(y) &&
            !std::isnan(z) && !std::isinf(z);
    }
};

struct FMinimalViewInfo
{
	Vector3 Location;
	Vector3 Rotation;
	float FOV;
};

struct BaseData
{
	std::uint64_t ActorArray;
	std::uint32_t ActorCount;

	std::uint64_t PlayerArray;
	std::uint32_t PlayerCount;

	std::uint64_t PlayerCameraManager;
};

struct EntityData
{
	std::vector<std::uint64_t> RootComponents;
};

struct PositionData
{
	std::vector<class Vector3> PlayerPositions;
	FMinimalViewInfo ViewInfo;
};

struct Vector4
{
    double x, y, z, w;
};

typedef struct MATRIX {
    union {
        struct {
            double        _11, _12, _13, _14;
            double        _21, _22, _23, _24;
            double        _31, _32, _33, _34;
            double        _41, _42, _43, _44;
        };
        double m[4][4];
    };
} MATRIX;

struct FTransform
{
    Vector4 rot;
    Vector3 translation;
    char pad[4];
    Vector3 scale;
    char pad1[4];

    MATRIX ToMatrixWithScale()
    {
        MATRIX m;
        m._41 = translation.x;
        m._42 = translation.y;
        m._43 = translation.z;

        double x2 = rot.x + rot.x;
        double y2 = rot.y + rot.y;
        double z2 = rot.z + rot.z;

        double xx2 = rot.x * x2;
        double yy2 = rot.y * y2;
        double zz2 = rot.z * z2;
        m._11 = (1.0f - (yy2 + zz2)) * scale.x;
        m._22 = (1.0f - (xx2 + zz2)) * scale.y;
        m._33 = (1.0f - (xx2 + yy2)) * scale.z;

        double yz2 = rot.y * z2;
        double wx2 = rot.w * x2;
        m._32 = (yz2 - wx2) * scale.z;
        m._23 = (yz2 + wx2) * scale.y;

        double xy2 = rot.x * y2;
        double wz2 = rot.w * z2;
        m._21 = (xy2 - wz2) * scale.y;
        m._12 = (xy2 + wz2) * scale.x;

        double xz2 = rot.x * z2;
        double wy2 = rot.w * y2;
        m._31 = (xz2 + wy2) * scale.z;
        m._13 = (xz2 - wy2) * scale.x;

        m._14 = 0.0f;
        m._24 = 0.0f;
        m._34 = 0.0f;
        m._44 = 1.0f;

        return m;
    }
};

static MATRIX CreateMatrix(Vector3 rot, Vector3 origin) {
    const double DEG_TO_RAD = static_cast<double>(3.14159265358979323846) / 180.f;
    const double radPitch = rot.x * DEG_TO_RAD;
    const double radYaw = rot.y * DEG_TO_RAD;
    const double radRoll = rot.z * DEG_TO_RAD;

    const double SP = sinf(radPitch);
    const double CP = cosf(radPitch);
    const double SY = sinf(radYaw);
    const double CY = cosf(radYaw);
    const double SR = sinf(radRoll);
    const double CR = cosf(radRoll);

    MATRIX matrix;
    matrix.m[0][0] = CP * CY;
    matrix.m[0][1] = CP * SY;
    matrix.m[0][2] = SP;
    matrix.m[0][3] = 0.f;

    matrix.m[1][0] = SR * SP * CY - CR * SY;
    matrix.m[1][1] = SR * SP * SY + CR * CY;
    matrix.m[1][2] = -SR * CP;
    matrix.m[1][3] = 0.f;

    matrix.m[2][0] = -(CR * SP * CY + SR * SY);
    matrix.m[2][1] = CY * SR - CR * SP * SY;
    matrix.m[2][2] = CR * CP;
    matrix.m[2][3] = 0.f;

    matrix.m[3][0] = origin.x;
    matrix.m[3][1] = origin.y;
    matrix.m[3][2] = origin.z;
    matrix.m[3][3] = 1.f;

    return matrix;
}

static Vector3 WorldToScreen(FMinimalViewInfo camera, Vector3 WorldLocation)
{
    Vector3 Screenlocation(0, 0, 0);
    const MATRIX tempMatrix = CreateMatrix(camera.Rotation, Vector3(0, 0, 0));

    Vector3 vAxisX(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
    Vector3 vAxisY(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
    Vector3 vAxisZ(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

    Vector3 vDelta = WorldLocation - camera.Location;
    Vector3 vTransformed = Vector3(vDelta.dot(vAxisY), vDelta.dot(vAxisZ), vDelta.dot(vAxisX));

    if (vTransformed.z < 1.f)
        vTransformed.z = 1.f;

    const double FOV_DEG_TO_RAD = static_cast<double>(3.14159265358979323846) / 360.f;

    Screenlocation.x = 960 + vTransformed.x * (960 / tanf(
        camera.FOV * FOV_DEG_TO_RAD)) / vTransformed.z;
    Screenlocation.y = 540 - vTransformed.y * (960 / tanf(
        camera.FOV * FOV_DEG_TO_RAD)) / vTransformed.z;

    return Screenlocation;
}
