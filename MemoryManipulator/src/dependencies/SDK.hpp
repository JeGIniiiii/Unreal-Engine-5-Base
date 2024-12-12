/**
 * @file SDK.hpp
 * @brief 游戏SDK定义,包含游戏内存偏移量和数据结构
 */

#pragma once
#include "common.h"

// https://dumpspace.spuckwaffel.com/Games/?hash=0cdf70b9&type=classes&idx=AAbilitySystemDebugHUD
namespace UE5
{
	// 游戏关键内存偏移量
	constexpr std::uint64_t GWORLD = 0x8FEFAC8;		// UWorld全局实例偏移
	constexpr std::uint64_t GNAMES = 0x8DB8D40;		// GNames全局实例偏移

	constexpr std::uint64_t ACTORARRAY = 0xA0;		// Actor数组偏移
	constexpr std::uint64_t ACTORCOUNT = 0xA8;		// Actor数量偏移

	constexpr std::uint64_t GAMEINSTANCE = 0x1d8;	// GameInstance偏移
	constexpr std::uint64_t GAMESTATE = 0x160;		// GameState偏移

	constexpr std::uint64_t PLAYERARRAY = 0x2a8;		// 玩家数组偏移
	constexpr std::uint64_t PLAYERCOUNT = 0x2a8 + 0x8;	// 玩家数量偏移

	constexpr std::uint64_t LOCALPLAYER = 0x38;		// 本地玩家偏移
	constexpr std::uint64_t PLAYERCONTROLLER = 0x30;	// 玩家控制器偏移

	constexpr std::uint64_t PLAYERCAMERAMANAGER = 0x348;	// 玩家相机管理器偏移
	constexpr std::uint64_t CAMERACACHEPRIVATE = 0x1390;	// 相机缓存偏移

	constexpr std::uint64_t APAWN = 0x308;		// APawn偏移
	constexpr std::uint64_t ROOTCOMPONENT = 0x1a0;	// 根组件偏移
	constexpr std::uint64_t RELATIVELOCATION = 0x128;	// 相对位置偏移
}

/**
 * @class Vector3
 * @brief 3D向量类,用于表示位置和方向
 */
class Vector3
{
public:
	double x, y, z;	// 三维坐标分量

	// 构造函数
	Vector3(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

	// 检查位置是否有效
	bool isValidLocation()
	{
		return x != 0 && y != 0 && z != 0;
	}

	// 计算点积
	double dot(const Vector3& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	// 向量减法运算符
	Vector3 operator-(const Vector3& other) const
	{
		return Vector3(x - other.x, y - other.y, z - other.z);
	}

	// 向量加法运算符
	Vector3 operator+(const Vector3& other) const
	{
		return Vector3(x + other.x, y + other.y, z + other.z);
	}

	// 向量加法赋值运算符
	Vector3& operator+=(const Vector3& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	// 向量减法赋值运算符
	Vector3& operator-=(const Vector3& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	// 向量数乘运算符
	Vector3 operator*(double scalar) const {
		return Vector3(x * scalar, y * scalar, z * scalar);
	}

	// 向量数除运算符
	Vector3 operator/(double scalar) const {
		if (scalar != 0) {
			return Vector3(x / scalar, y / scalar, z / scalar);
		}
		return *this;
	}

	// 向量数乘赋值运算符
	Vector3& operator*=(double scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	// 向量数除赋值运算符
	Vector3& operator/=(double scalar) {
		if (scalar != 0) {
			x /= scalar;
			y /= scalar;
			z /= scalar;
		}
		return *this;
	}

	// 计算向量长度
	double magnitude() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	// 向量归一化
	Vector3 normalize() const {
		double mag = magnitude();
		if (mag > 0) {
			return *this / mag;
		}
		return *this;
	}

	// 向量加法
	Vector3 Add(const Vector3& other) const {
		return Vector3(x + other.x, y + other.y, z + other.z);
	}

	// 检查向量是否有效
	bool IsValid() {
		return !std::isnan(x) && !std::isinf(x) &&
			!std::isnan(y) && !std::isinf(y) &&
			!std::isnan(z) && !std::isinf(z);
	}
};

/**
 * @struct FMinimalViewInfo
 * @brief 相机视图信息结构
 */
struct FMinimalViewInfo
{
	Vector3 Location;	// 相机位置
	Vector3 Rotation;	// 相机旋转
	float FOV;		// 视场角
};

/**
 * @struct BaseData
 * @brief 基础游戏数据结构
 */
struct BaseData
{
	std::uint64_t ActorArray;		// Actor数组地址
	std::uint32_t ActorCount;		// Actor数量

	std::uint64_t PlayerArray;		// 玩家数组地址
	std::uint32_t PlayerCount;		// 玩家数量

	std::uint64_t PlayerCameraManager;	// 相机管理器地址
};

/**
 * @struct EntityData
 * @brief 实体数据结构
 */
struct EntityData
{
	std::vector<std::uint64_t> RootComponents;	// 根组件地址列表
};

/**
 * @struct PositionData
 * @brief 位置数据结构
 */
struct PositionData
{
	std::vector<class Vector3> PlayerPositions;	// 玩家位置列表
	FMinimalViewInfo ViewInfo;					// 视图信息
};

/**
 * @struct Vector4
 * @brief 4D向量结构,用于表示四元数
 */
struct Vector4
{
	double x, y, z, w;	// 四维坐标分量
};

/**
 * @struct MATRIX
 * @brief 4x4矩阵结构
 */
typedef struct MATRIX {
	union {
		struct {
			double        _11, _12, _13, _14;	// 第一行
			double        _21, _22, _23, _24;	// 第二行
			double        _31, _32, _33, _34;	// 第三行
			double        _41, _42, _43, _44;	// 第四行
		};
		double m[4][4];	// 矩阵数组形式
	};
} MATRIX;

/**
 * @struct FTransform
 * @brief 变换结构,包含旋转、平移和缩放
 */
struct FTransform
{
	Vector4 rot;		// 旋转(四元数)
	Vector3 translation;	// 平移
	char pad[4];		// 对齐填充
	Vector3 scale;		// 缩放
	char pad1[4];		// 对齐填充

	/**
	 * @brief 将变换转换为矩阵形式
	 * @return 返回4x4变换矩阵
	 */
	MATRIX ToMatrixWithScale()
	{
		MATRIX m;
		// 设置平移分量
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		// 计算旋转分量
		double x2 = rot.x + rot.x;
		double y2 = rot.y + rot.y;
		double z2 = rot.z + rot.z;

		double xx2 = rot.x * x2;
		double yy2 = rot.y * y2;
		double zz2 = rot.z * z2;
		
		// 设置主对角线元素
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;

		// 计算混合项
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

		// 设置齐次坐标分量
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
