#pragma once
#include <gp/config.hpp>

#pragma warning(push,0)

#include "EigenMath/Dense"

#pragma warning(pop)

typedef Eigen::Matrix4d double4x4;
typedef Eigen::Matrix4f float4x4;
typedef Eigen::Matrix4i int4x4;

typedef Eigen::RowVector4d double4;
typedef Eigen::RowVector4f float4;
typedef Eigen::RowVector4i int4;

#include <DirectXMath.h>

typedef DirectX::XMMATRIX Matrix4x4;
typedef DirectX::XMVECTOR Vector4;
