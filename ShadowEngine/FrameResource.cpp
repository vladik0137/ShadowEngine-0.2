#include "FrameResource.h"

FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT VertexCount)
{
	ThrowIfFailed(device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));

	PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
	ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);

	VertexBuffer = std::make_unique<UploadBuffer<Vertex>>(device, VertexCount, true);
}

FrameResource::~FrameResource()
{
}


