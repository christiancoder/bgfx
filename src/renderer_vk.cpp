//=============================================================================================
//= Michael Anderson (christiancoder@gmail.com)
//=============================================================================================

#include "renderer_vk.h"

//=============================================================================================

namespace bgfx
{

//=============================================================================================

namespace vk
{

//=============================================================================================

static RendererContextVK* s_renderVK;

//=============================================================================================

static const char* s_allocScopeName[] =
{
	"vkCommand",
	"vkObject",
	"vkCache",
	"vkDevice",
	"vkInstance",
};
BX_STATIC_ASSERT(VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE == BX_COUNTOF(s_allocScopeName)-1);

//=============================================================================================

static void* VKAPI_PTR allocationFunction(void* userData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
	BX_UNUSED(userData, allocationScope);
	return bx::alignedAlloc(g_allocator, size, alignment, s_allocScopeName[allocationScope]);
}

//=============================================================================================

static void* VKAPI_PTR reallocationFunction(void* userData, void* original, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
	BX_UNUSED(userData, allocationScope);
	return bx::alignedRealloc(g_allocator, original, size, alignment, s_allocScopeName[allocationScope]);
}

//=============================================================================================

static void VKAPI_PTR freeFunction(void* userData, void* memory)
{
	BX_UNUSED(userData);

	if (NULL == memory)
	{
		return;
	}

	bx::alignedFree(g_allocator, memory, 8);
}

//=============================================================================================

static void VKAPI_PTR internalAllocationNotification(void* userData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
{
	BX_UNUSED(userData, size, allocationType, allocationScope);
}

//=============================================================================================

static void VKAPI_PTR internalFreeNotification(void* userData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
{
	BX_UNUSED(userData, size, allocationType, allocationScope);
}

//=============================================================================================

static VkAllocationCallbacks s_allocationCb =
{
	NULL,
	allocationFunction,
	reallocationFunction,
	freeFunction,
	internalAllocationNotification,
	internalFreeNotification,
};

//=============================================================================================
// R e n d e r e r C o n t e x t V K
//=============================================================================================

RendererContextVK::RendererContextVK()
{
	// Pretend all features are available.
	g_caps.supported = 0
		| BGFX_CAPS_ALPHA_TO_COVERAGE
		| BGFX_CAPS_BLEND_INDEPENDENT
		| BGFX_CAPS_COMPUTE
		| BGFX_CAPS_CONSERVATIVE_RASTER
		| BGFX_CAPS_DRAW_INDIRECT
		| BGFX_CAPS_FRAGMENT_DEPTH
		| BGFX_CAPS_FRAGMENT_ORDERING
		| BGFX_CAPS_GRAPHICS_DEBUGGER
		| BGFX_CAPS_HIDPI
		| BGFX_CAPS_INDEX32
		| BGFX_CAPS_INSTANCING
		| BGFX_CAPS_OCCLUSION_QUERY
		| BGFX_CAPS_RENDERER_MULTITHREADED
		| BGFX_CAPS_SWAP_CHAIN
		| BGFX_CAPS_TEXTURE_2D_ARRAY
		| BGFX_CAPS_TEXTURE_3D
		| BGFX_CAPS_TEXTURE_BLIT
		| BGFX_CAPS_TEXTURE_COMPARE_ALL
		| BGFX_CAPS_TEXTURE_COMPARE_LEQUAL
		| BGFX_CAPS_TEXTURE_CUBE_ARRAY
		| BGFX_CAPS_TEXTURE_READ_BACK
		| BGFX_CAPS_VERTEX_ATTRIB_HALF
		| BGFX_CAPS_VERTEX_ATTRIB_UINT10
		;

	// Pretend all features are available for all texture formats.
	for (uint32_t formatIdx = 0; formatIdx < TextureFormat::Count; ++formatIdx)
	{
		g_caps.formats[formatIdx] = 0
			| BGFX_CAPS_FORMAT_TEXTURE_NONE
			| BGFX_CAPS_FORMAT_TEXTURE_2D
			| BGFX_CAPS_FORMAT_TEXTURE_2D_SRGB
			| BGFX_CAPS_FORMAT_TEXTURE_2D_EMULATED
			| BGFX_CAPS_FORMAT_TEXTURE_3D
			| BGFX_CAPS_FORMAT_TEXTURE_3D_SRGB
			| BGFX_CAPS_FORMAT_TEXTURE_3D_EMULATED
			| BGFX_CAPS_FORMAT_TEXTURE_CUBE
			| BGFX_CAPS_FORMAT_TEXTURE_CUBE_SRGB
			| BGFX_CAPS_FORMAT_TEXTURE_CUBE_EMULATED
			| BGFX_CAPS_FORMAT_TEXTURE_VERTEX
			| BGFX_CAPS_FORMAT_TEXTURE_IMAGE
			| BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER
			| BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER_MSAA
			| BGFX_CAPS_FORMAT_TEXTURE_MSAA
			| BGFX_CAPS_FORMAT_TEXTURE_MIP_AUTOGEN
			;
	}

	// Pretend we have no limits
	g_caps.limits.maxTextureSize     = 16384;
	g_caps.limits.maxTextureLayers   = 2048;
	g_caps.limits.maxComputeBindings = g_caps.limits.maxTextureSamplers;
	g_caps.limits.maxFBAttachments   = BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS;
	g_caps.limits.maxVertexStreams   = BGFX_CONFIG_MAX_VERTEX_STREAMS;
}

//=============================================================================================

RendererContextVK::~RendererContextVK()
{
}

//=============================================================================================

RendererType::Enum RendererContextVK::getRendererType() const
{
	return RendererType::Vulkan;
}

//=============================================================================================

const char* RendererContextVK::getRendererName() const
{
	return BGFX_RENDERER_VULKAN_NAME;
}

//=============================================================================================

bool RendererContextVK::isDeviceRemoved()
{
	return false;
}

//=============================================================================================

void RendererContextVK::flip()
{
}

//=============================================================================================

void RendererContextVK::createIndexBuffer(IndexBufferHandle handle, const Memory* mem, uint16_t flags)
{
}

//=============================================================================================

void RendererContextVK::destroyIndexBuffer(IndexBufferHandle handle)
{
}

//=============================================================================================

void RendererContextVK::createVertexLayout(VertexLayoutHandle handle, const VertexLayout& layout)
{
}

//=============================================================================================

void RendererContextVK::destroyVertexLayout(VertexLayoutHandle handle)
{
}

//=============================================================================================

void RendererContextVK::createVertexBuffer(VertexBufferHandle handle, const Memory* mem, VertexLayoutHandle layoutHandle, uint16_t flags)
{
}

//=============================================================================================

void RendererContextVK::destroyVertexBuffer(VertexBufferHandle handle)
{
}

//=============================================================================================

void RendererContextVK::createDynamicIndexBuffer(IndexBufferHandle handle, uint32_t size, uint16_t flags)
{
}

//=============================================================================================

void RendererContextVK::updateDynamicIndexBuffer(IndexBufferHandle handle, uint32_t offset, uint32_t size, const Memory* mem)
{
}

//=============================================================================================

void RendererContextVK::destroyDynamicIndexBuffer(IndexBufferHandle handle)
{
}

//=============================================================================================

void RendererContextVK::createDynamicVertexBuffer(VertexBufferHandle handle, uint32_t size, uint16_t flags)
{
}

//=============================================================================================

void RendererContextVK::updateDynamicVertexBuffer(VertexBufferHandle handle, uint32_t offset, uint32_t size, const Memory* mem)
{
}

//=============================================================================================

void RendererContextVK::destroyDynamicVertexBuffer(VertexBufferHandle handle)
{
}

//=============================================================================================

void RendererContextVK::createShader(ShaderHandle handle, const Memory* mem)
{
}

//=============================================================================================

void RendererContextVK::destroyShader(ShaderHandle handle)
{
}

//=============================================================================================

void RendererContextVK::createProgram(ProgramHandle handle, ShaderHandle vsh, ShaderHandle fsh)
{
}

//=============================================================================================

void RendererContextVK::destroyProgram(ProgramHandle handle)
{
}

//=============================================================================================

void* RendererContextVK::createTexture(TextureHandle handle, const Memory* mem, uint64_t flags, uint8_t skip)
{
	return NULL;
}

//=============================================================================================

void RendererContextVK::updateTextureBegin(TextureHandle handle, uint8_t side, uint8_t mip)
{
}

//=============================================================================================

void RendererContextVK::updateTexture(TextureHandle handle, uint8_t side, uint8_t mip, const Rect& rect, uint16_t z, uint16_t depth, uint16_t pitch, const Memory* mem)
{
}

//=============================================================================================

void RendererContextVK::updateTextureEnd()
{
}

//=============================================================================================

void RendererContextVK::readTexture(TextureHandle handle, void* data, uint8_t mip)
{
}

//=============================================================================================

void RendererContextVK::resizeTexture(TextureHandle handle, uint16_t width, uint16_t height, uint8_t numMips, uint16_t numLayers)
{
}

//=============================================================================================

void RendererContextVK::overrideInternal(TextureHandle handle, uintptr_t ptr)
{
}

//=============================================================================================

uintptr_t RendererContextVK::getInternal(TextureHandle handle)
{
	return 0;
}

//=============================================================================================

void RendererContextVK::destroyTexture(TextureHandle handle)
{
}

//=============================================================================================

void RendererContextVK::createFrameBuffer(FrameBufferHandle handle, uint8_t num, const Attachment* attachment)
{
}

//=============================================================================================

void RendererContextVK::createFrameBuffer(FrameBufferHandle handle, void* nwh, uint32_t width, uint32_t height, TextureFormat::Enum format, TextureFormat::Enum depthFormat)
{
}

//=============================================================================================

void RendererContextVK::destroyFrameBuffer(FrameBufferHandle handle)
{
}

//=============================================================================================

void RendererContextVK::createUniform(UniformHandle handle, UniformType::Enum type, uint16_t num, const char* name)
{
}

//=============================================================================================

void RendererContextVK::destroyUniform(UniformHandle handle)
{
}

//=============================================================================================

void RendererContextVK::requestScreenShot(FrameBufferHandle handle, const char* filePath)
{
}

//=============================================================================================

void RendererContextVK::updateViewName(ViewId id, const char* name)
{
}

//=============================================================================================

void RendererContextVK::updateUniform(uint16_t loc, const void* data, uint32_t size)
{
}

//=============================================================================================

void RendererContextVK::invalidateOcclusionQuery(OcclusionQueryHandle handle)
{
}

//=============================================================================================

void RendererContextVK::setMarker(const char* marker, uint16_t len)
{
}

//=============================================================================================

void RendererContextVK::setName(Handle handle, const char* name, uint16_t len)
{
}

//=============================================================================================

void RendererContextVK::submit(Frame* render, ClearQuad& clearQuad, TextVideoMemBlitter& textVideoMemBlitter)
{
	const int64_t timerFreq = bx::getHPFrequency();
	const int64_t timeBegin = bx::getHPCounter();

	Stats& perfStats = render->m_perfStats;
	perfStats.cpuTimeBegin  = timeBegin;
	perfStats.cpuTimeEnd    = timeBegin;
	perfStats.cpuTimerFreq  = timerFreq;

	perfStats.gpuTimeBegin  = 0;
	perfStats.gpuTimeEnd    = 0;
	perfStats.gpuTimerFreq  = 1000000000;

	bx::memSet(perfStats.numPrims, 0, sizeof(perfStats.numPrims) );

	perfStats.gpuMemoryMax  = -INT64_MAX;
	perfStats.gpuMemoryUsed = -INT64_MAX;
}

//=============================================================================================

void RendererContextVK::blitSetup(TextVideoMemBlitter& blitter)
{
}

//=============================================================================================

void RendererContextVK::blitRender(TextVideoMemBlitter& blitter, uint32_t numIndices)
{
}

//=============================================================================================

RendererContextI* rendererCreate(const Init& init)
{
	BX_UNUSED(init);
	s_renderVK = BX_NEW(g_allocator, RendererContextVK);
	return s_renderVK;
}

//=============================================================================================

void rendererDestroy()
{
	BX_DELETE(g_allocator, s_renderVK);
	s_renderVK = NULL;
}

//=============================================================================================

} // namespace vk

//=============================================================================================

} // namespace bgfx

//=============================================================================================
