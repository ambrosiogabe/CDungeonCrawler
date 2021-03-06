#pragma once

#include "cocoa/renderer/Texture.h"
#include "cocoa/core/Handle.h"
#include "cocoa/util/DynamicArray.h"

namespace Cocoa
{
	struct Framebuffer
	{
		uint32 Fbo = (uint32)-1;
		int32 Width = 0;
		int32 Height = 0;

		// Depth/Stencil attachment (optional)
		uint32 Rbo = (uint32)-1;
		ByteFormat DepthStencilFormat = ByteFormat::None;
		bool IncludeDepthStencil = true;

		// Color attachments
		std::vector<Texture> ColorAttachments; // TODO: All color attachments will be resized to match the framebuffer size (perhaps this should be changed in the future...?)
	};

	namespace NFramebuffer
	{
		COCOA void Delete(Framebuffer& framebuffer);
		COCOA void Generate(Framebuffer& framebuffer);

		COCOA const Texture& GetColorAttachment(const Framebuffer& framebuffer, int index);
		COCOA void AddColorAttachment(Framebuffer& framebuffer, const Texture& textureSpec); // TODO: The order the attachments are added will be the index they get (change this in the future too...?)
		COCOA void ClearColorAttachmentUint32(const Framebuffer& framebuffer, int colorAttachment, uint32 clearColor);
		COCOA uint32 ReadPixelUint32(const Framebuffer& framebuffer, int colorAttachment, int x, int y);

		COCOA void Bind(const Framebuffer& framebuffer);
		COCOA void Unbind(const Framebuffer& framebuffer);
	};
}