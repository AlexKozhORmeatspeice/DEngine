#include "dpch.h"
#include "OpenGLTexture.h"

namespace DEngine
{
    static GLenum DEngineImageFormatToGLDataFormat(ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::R8:      return GL_RED;
            case ImageFormat::RGB8:    return GL_RGB;
            case ImageFormat::RGBA8:   return GL_RGBA;
            case ImageFormat::RGBA32F: return GL_RGBA;
        }
        D_CORE_ASSERT(false, "Can't translate image format to gl format");
        return GL_NONE;
    }

    static GLenum DEngineImageFormatToGLInternalFormat(ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::R8:      return GL_R8;
            case ImageFormat::RGB8:    return GL_RGB8;
            case ImageFormat::RGBA8:   return GL_RGBA8;
            case ImageFormat::RGBA32F: return GL_RGBA32F;
        }
        D_CORE_ASSERT(false, "Can't translate image format to gl format");
        return GL_NONE;
    }

    OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& spec, const DataBuffer& data)
        : m_Specification(spec)
    {
        m_Width = spec.Width;
        m_Height = spec.Height;
        
        m_InternalFormat = DEngineImageFormatToGLInternalFormat(m_Specification.Format);
        m_DataFormat = DEngineImageFormatToGLDataFormat(m_Specification.Format);

        if (m_InternalFormat == GL_NONE || m_DataFormat == GL_NONE)
        {
            D_CORE_ERROR("OpenGLTexture2D: invalid format");
            return;
        }

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

        if (data)
        {
            SetData(data);
        }
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture2D::SetData(const DataBuffer& data)
    {
        uint32_t expectedSize = m_Width * m_Height;
        switch (m_DataFormat)
        {
            case GL_RED:   expectedSize *= 1; break;
            case GL_RGB:   expectedSize *= 3; break;
            case GL_RGBA:  expectedSize *= 4; break;
            default:
                D_CORE_ERROR("OpenGLTexture2D::SetData: unknown format");
                return;
        }

        if (data.Size != expectedSize)
        {
            D_CORE_ERROR("OpenGLTexture2D::SetData: data size mismatch! Expected {0}, got {1}", 
                         expectedSize, data.Size);
            return;
        }

        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, 
                           m_DataFormat, GL_UNSIGNED_BYTE, data.Data);
    }

    uint32_t OpenGLTexture2D::GetRendererID() const
    {
        return m_RendererID;
    }

    void OpenGLTexture2D::Bind(unsigned int slot) const
    {
        glBindTextureUnit(slot, m_RendererID);
    }
}