#pragma once
#include <gp/config.hpp>

#include <vector>
#include "Math.h"
#include "Algorithm.h"
#include "RenderEngine/Object3D.h"

bool registration(Object3d& a, Object3d& b, float4x4& rotaMat, float4& transVec, bool& Flag);

bool SVD(int Num, float4* a, float4* b, float4x4& rotaMat, float4& transVec);

Object3d SphericalFilter(Object3d& a, int k, float R);
