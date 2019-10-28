#pragma once

#include "DX12Util.h"
#include "ShadowMath.h"
#include "UploadBuffer.h"

struct ObjectConstants
{
	DirectX::XMFLOAT4X4 World = ShadowMath::Identity4x4();
};

struct PassConstants
{
	DirectX::XMFLOAT4X4 View = ShadowMath::Identity4x4();
	DirectX::XMFLOAT4X4 InvView = ShadowMath::Identity4x4();
	DirectX::XMFLOAT4X4 Proj = ShadowMath::Identity4x4();
	DirectX::XMFLOAT4X4 InvProj = ShadowMath::Identity4x4();
	DirectX::XMFLOAT4X4 ViewProj = ShadowMath::Identity4x4();
	DirectX::XMFLOAT4X4 InvViewProj = ShadowMath::Identity4x4();
	DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
	float cbPerObjectPad1 = 0.0f;
	DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
	float NearZ = 0.0f;
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;
};

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

struct FrameResource
{
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT VertexCount);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource();

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

	std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

	std::unique_ptr<UploadBuffer<Vertex>> VertexBuffer = nullptr;

	UINT64 Fence = 0;
};