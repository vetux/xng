/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "qtogltexturebuffer.hpp"
#include "qtoglcheckerror.hpp"
#include "qtogltypeconverter.hpp"

using namespace xengine;
using namespace xengine::opengl;

//TODO: If users want to use integer texture formats the shaders would need to sample using isampler2d which does not exist in hlsl and therefore cant be cross compiled.
//For now all integer format textures are uploaded as the normalized float format,

QtOGLTextureBuffer::QtOGLTextureBuffer(Attributes attributes) : TextureBuffer(attributes), handle() {
    QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();

    GLenum type = QtOGLTypeConverter::convert(attributes.textureType);

    glGenTextures(1, &handle);
    glBindTexture(type, handle);

    if (type != GL_TEXTURE_2D_MULTISAMPLE) {
        glTexParameteri(type, GL_TEXTURE_WRAP_S, QtOGLTypeConverter::convert(attributes.wrapping));
        glTexParameteri(type, GL_TEXTURE_WRAP_T, QtOGLTypeConverter::convert(attributes.wrapping));
    }
    checkGLError("QtOGLTextureBuffer::QtOGLTextureBuffer()");

    if (type != GL_TEXTURE_2D_MULTISAMPLE) {
        glTexParameteri(type,
                        GL_TEXTURE_MIN_FILTER,
                        QtOGLTypeConverter::convert(attributes.filterMin));
        glTexParameteri(type,
                        GL_TEXTURE_MAG_FILTER,
                        QtOGLTypeConverter::convert(attributes.filterMag));
    }
    checkGLError("QtOGLTextureBuffer::QtOGLTextureBuffer()");

    if (attributes.textureType == TEXTURE_2D) {
        GLuint texInternalFormat = QtOGLTypeConverter::convert(attributes.format);
        GLuint texFormat = GL_RGBA;

        if (attributes.format >= R8I) {
            texFormat = GL_RGBA_INTEGER; //Integer formats require _INTEGER format
        }

        GLuint texType = GL_UNSIGNED_BYTE;

        if (attributes.format == ColorFormat::DEPTH) {
            texInternalFormat = GL_DEPTH;
            texFormat = GL_DEPTH_COMPONENT;
            texType = GL_FLOAT;
        } else if (attributes.format == ColorFormat::DEPTH_STENCIL) {
            texInternalFormat = GL_DEPTH24_STENCIL8;
            texFormat = GL_DEPTH_STENCIL;
            texType = GL_UNSIGNED_INT_24_8;
        }

        glTexImage2D(type,
                     0,
                     texInternalFormat,
                     attributes.size.x,
                     attributes.size.y,
                     0,
                     texFormat,
                     texType,
                     NULL);
    } else if (attributes.textureType == TEXTURE_2D_MULTISAMPLE) {
        GLuint texInternalFormat = QtOGLTypeConverter::convert(attributes.format);

        if (attributes.format == ColorFormat::DEPTH) {
            texInternalFormat = GL_DEPTH;
        } else if (attributes.format == ColorFormat::DEPTH_STENCIL) {
            texInternalFormat = GL_DEPTH24_STENCIL8;
        }

        glTexImage2DMultisample(type,
                                attributes.samples,
                                texInternalFormat,
                                attributes.size.x,
                                attributes.size.y,
                                GL_TRUE);
    } else {
        for (unsigned int i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         QtOGLTypeConverter::convert(attributes.format),
                         attributes.size.x,
                         attributes.size.y,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         NULL);
        }
    }
    checkGLError("QtOGLTextureBuffer::QtOGLTextureBuffer()");

    if (type != GL_TEXTURE_2D_MULTISAMPLE && attributes.generateMipmap) {
        glGenerateMipmap(type);
        glTexParameteri(type, GL_TEXTURE_MIN_FILTER,
                        QtOGLTypeConverter::convert(attributes.mipmapFilter));
        glTexParameteri(type, GL_TEXTURE_MAG_FILTER,
                        QtOGLTypeConverter::convert(attributes.filterMag));
    }

    glBindTexture(type, 0);

    checkGLError("QtOGLTextureBuffer::QtOGLTextureBuffer()");
}

QtOGLTextureBuffer::~QtOGLTextureBuffer() {
    glDeleteTextures(1, &handle);
}

void QtOGLTextureBuffer::upload(const Image<ColorRGB> &buffer) {
    setTextureType(TextureBuffer::TEXTURE_2D);

    attributes.format = TextureBuffer::RGB;
    attributes.size = buffer.getSize();

    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 QtOGLTypeConverter::convert(attributes.format),
                 attributes.size.x,
                 attributes.size.y,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 buffer.getData());

    if (attributes.generateMipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    checkGLError("QtOGLTextureBuffer::upload(RGB)");
}

void QtOGLTextureBuffer::upload(const Image<ColorRGBA> &buffer) {
    setTextureType(TextureBuffer::TEXTURE_2D);

    attributes.format = TextureBuffer::RGBA;
    attributes.size = buffer.getSize();

    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 QtOGLTypeConverter::convert(attributes.format),
                 attributes.size.x,
                 attributes.size.y,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 buffer.getData());

    if (attributes.generateMipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    checkGLError("QtOGLTextureBuffer::upload(RGBA)");
}

void QtOGLTextureBuffer::upload(const Image<float> &buffer) {
    setTextureType(TextureBuffer::TEXTURE_2D);

    attributes.format = TextureBuffer::R32F;
    attributes.size = buffer.getSize();

    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 QtOGLTypeConverter::convert(attributes.format),
                 attributes.size.x,
                 attributes.size.y,
                 0,
                 GL_RED,
                 GL_FLOAT,
                 buffer.getData());

    if (attributes.generateMipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    checkGLError("QtOGLTextureBuffer::upload(float)");
}

void QtOGLTextureBuffer::upload(const Image<int> &buffer) {
    setTextureType(TextureBuffer::TEXTURE_2D);

    attributes.format = TextureBuffer::R;
    attributes.size = buffer.getSize();

    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 QtOGLTypeConverter::convert(attributes.format),
                 attributes.size.x,
                 attributes.size.y,
                 0,
                 GL_RED_INTEGER,
                 GL_INT,
                 buffer.getData());

    if (attributes.generateMipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    checkGLError("QtOGLTextureBuffer::upload(int)");
}

void QtOGLTextureBuffer::upload(const Image<char> &buffer) {
    setTextureType(TextureBuffer::TEXTURE_2D);

    attributes.format = TextureBuffer::R;
    attributes.size = buffer.getSize();

    glBindTexture(GL_TEXTURE_2D, handle);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 QtOGLTypeConverter::convert(attributes.format),
                 attributes.size.x,
                 attributes.size.y,
                 0,
                 GL_RED,
                 GL_BYTE,
                 buffer.getData());

    if (attributes.generateMipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    checkGLError("QtOGLTextureBuffer::upload(char)");
}

void QtOGLTextureBuffer::upload(const Image<unsigned char> &buffer) {
    setTextureType(TextureBuffer::TEXTURE_2D);

    attributes.format = TextureBuffer::R;
    attributes.size = buffer.getSize();

    glBindTexture(GL_TEXTURE_2D, handle);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 QtOGLTypeConverter::convert(attributes.format),
                 attributes.size.x,
                 attributes.size.y,
                 0,
                 GL_RED,
                 GL_UNSIGNED_BYTE,
                 buffer.getData());

    if (attributes.generateMipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    checkGLError("QtOGLTextureBuffer::upload(unsigned char)");
}

xengine::Image<ColorRGBA> QtOGLTextureBuffer::download() {
    if (attributes.textureType != TEXTURE_2D)
        throw std::runtime_error("TextureBuffer not texture 2d");

    auto output = Image<ColorRGBA>(attributes.size);
    glBindTexture(GL_TEXTURE_2D, handle);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *) output.getData());
    glBindTexture(GL_TEXTURE_2D, 0);
    checkGLError("QtOGLTextureBuffer::download");
    return output;
}

void QtOGLTextureBuffer::upload(CubeMapFace face, const Image<ColorRGBA> &buffer) {
    setTextureType(TextureBuffer::TEXTURE_CUBE_MAP);

    attributes.format = TextureBuffer::RGBA;
    attributes.size = buffer.getSize();

    glBindTexture(GL_TEXTURE_CUBE_MAP, handle);
    glTexImage2D(QtOGLTypeConverter::convert(face),
                 0,
                 QtOGLTypeConverter::convert(attributes.format),
                 attributes.size.x,
                 attributes.size.y,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 buffer.getData());

    if (attributes.generateMipmap) {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    checkGLError("QtOGLTextureBuffer::upload(CUBEMAP)");
}

Image<ColorRGBA> QtOGLTextureBuffer::download(TextureBuffer::CubeMapFace face) {
    if (attributes.textureType != TEXTURE_CUBE_MAP)
        throw std::runtime_error("TextureBuffer not cubemap");

    throw std::runtime_error("Not Implemented");
}

void QtOGLTextureBuffer::uploadCubeMap(const Image<ColorRGBA> &buffer) {
    auto faceSize = buffer.getSize();
    faceSize.x = faceSize.x / 6;
    if (faceSize.x != faceSize.y)
        throw std::runtime_error("Invalid cubemap image");

    for (int i = 0; i < 6; i++) {
        upload(static_cast<CubeMapFace>(i), buffer.slice(Recti(Vec2i(faceSize.x * i, 0), faceSize)));
    }
}

Image<ColorRGBA> QtOGLTextureBuffer::downloadCubeMap() {
    auto size = attributes.size;
    size.x = size.x * 6;
    Image<ColorRGBA> ret(size);
    for (int i = 0; i < 6; i++) {
        ret.blit({Vec2i(i * attributes.size.x, 0), attributes.size}, download(static_cast<CubeMapFace>(i)));
    }
    return ret;
}

void QtOGLTextureBuffer::setTextureType(TextureType t) {
    if (attributes.textureType != t) {
        attributes.textureType = t;

        GLenum type = QtOGLTypeConverter::convert(attributes.textureType);

        glDeleteTextures(1, &handle);

        glGenTextures(1, &handle);
        glBindTexture(type, handle);

        glTexParameteri(type, GL_TEXTURE_WRAP_S, QtOGLTypeConverter::convert(attributes.wrapping));
        glTexParameteri(type, GL_TEXTURE_WRAP_T, QtOGLTypeConverter::convert(attributes.wrapping));
        checkGLError("QtOGLTextureBuffer::QtOGLTextureBuffer()");

        glTexParameteri(type,
                        GL_TEXTURE_MIN_FILTER,
                        QtOGLTypeConverter::convert(attributes.filterMin));
        glTexParameteri(type,
                        GL_TEXTURE_MAG_FILTER,
                        QtOGLTypeConverter::convert(attributes.filterMag));
        checkGLError("QtOGLTextureBuffer::QtOGLTextureBuffer()");

        if (attributes.textureType == TEXTURE_2D) {
            GLuint texInternalFormat = QtOGLTypeConverter::convert(attributes.format);
            GLuint texFormat = GL_RGBA;

            if (attributes.format >= R8I) {
                texFormat = GL_RGBA_INTEGER; //Integer formats require _INTEGER format
            }

            GLuint texType = GL_UNSIGNED_BYTE;

            if (attributes.format == ColorFormat::DEPTH) {
                texInternalFormat = GL_DEPTH;
                texFormat = GL_DEPTH_COMPONENT;
                texType = GL_FLOAT;
            } else if (attributes.format == ColorFormat::DEPTH_STENCIL) {
                texInternalFormat = GL_DEPTH24_STENCIL8;
                texFormat = GL_DEPTH_STENCIL;
                texType = GL_UNSIGNED_INT_24_8;
            }

            glTexImage2D(type,
                         0,
                         texInternalFormat,
                         attributes.size.x,
                         attributes.size.y,
                         0,
                         texFormat,
                         texType,
                         NULL);
        } else {
            for (unsigned int i = 0; i < 6; i++) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0,
                             QtOGLTypeConverter::convert(attributes.format),
                             attributes.size.x,
                             attributes.size.y,
                             0,
                             GL_RGBA,
                             GL_UNSIGNED_BYTE,
                             NULL);
            }
        }
        checkGLError("QtOGLTextureBuffer::QtOGLTextureBuffer()");

        if (attributes.generateMipmap) {
            glGenerateMipmap(type);
            glTexParameteri(type, GL_TEXTURE_MIN_FILTER,
                            QtOGLTypeConverter::convert(attributes.mipmapFilter));
            glTexParameteri(type, GL_TEXTURE_MAG_FILTER,
                            QtOGLTypeConverter::convert(attributes.filterMag));
        }

        glBindTexture(type, 0);
        checkGLError("QtOGLTextureBuffer::QtOGLTextureBuffer()");
    }
}