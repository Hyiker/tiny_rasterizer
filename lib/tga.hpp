// https://github.com/ColumbusUtrigas/TGA/blob/master/tga.hpp
// MIT License
//
// Copyright (c) 2018
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef TGA_HPP
#define TGA_HPP
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <string>

/*!
 * @file tga.hpp
 * @brief All C++ library functional.
 */

namespace tga {

template <typename Type>
static void ReadPixel8(Type*& Buffer, Type& Red) {
    Red = *Buffer++;
}

template <typename Type>
static void ReadPixel24(Type*& Buffer, Type& Red, Type& Green, Type& Blue) {
    Blue = *Buffer++;
    Green = *Buffer++;
    Red = *Buffer++;
}

template <typename Type>
static void ReadPixel32(Type*& Buffer, Type& Red, Type& Green, Type& Blue,
                        Type& Alpha) {
    Blue = *Buffer++;
    Green = *Buffer++;
    Red = *Buffer++;
    Alpha = *Buffer++;
}

template <typename Type>
static void WritePixel8(Type*& Buffer, Type Red) {
    *Buffer++ = Red;
}

template <typename Type>
static void WritePixel24(Type*& Buffer, Type Red, Type Green, Type Blue) {
    *Buffer++ = Red;
    *Buffer++ = Green;
    *Buffer++ = Blue;
}

template <typename Type>
static void WritePixel32(Type*& Buffer, Type Red, Type Green, Type Blue,
                         Type Alpha) {
    *Buffer++ = Red;
    *Buffer++ = Green;
    *Buffer++ = Blue;
    *Buffer++ = Alpha;
}

/*
 * @brief Format of loaded image.
 */
enum class ImageFormat { Monochrome, RGB, RGBA, Undefined };

/**
 * @brief Main TGA class.
 *
 * **Destructor deletes data. Pointer obtained with 'GetData' invalidates after
 * destructing TGA object.**
 *
 * # Examples
 *
 * ```cpp
 * #include "tga.hpp"
 *
 * tga::TGA tga;
 * if (!tga.Load("image.tga")) error();
 *
 * uint8_t* Data = tga.GetData();
 * uint64_t Size = tga.GetSize();
 * uint32_t Width = tga.GetWidth();
 * uint32_t Height = tga.GetHeight();
 * tga::ImageFormat Format = tga.GetFormat();
 * ```
 */
class TGA {
   private:
    struct Header {
        uint8_t IDLength;
        uint8_t ColorMapType;
        uint8_t ImageType;

        uint16_t ColorMapOrigin;
        uint16_t ColorMapLength;
        uint8_t ColorMapEntrySize;

        uint16_t XOrigin;
        uint16_t YOrigin;
        uint16_t Width;
        uint16_t Height;
        uint8_t Bits;
        uint8_t ImageDescriptor;
    };

   private:
    uint8_t* Data;
    uint64_t Size;
    uint32_t Width;
    uint32_t Height;
    ImageFormat Format;

   private:
    template <typename Type>
    void RGBPaletted(Type* InBuffer, uint8_t* ColorMap, uint8_t* OutBuffer,
                     size_t Size);

    template <typename Type>
    void RGBAPaletted(Type* InBuffer, uint8_t* ColorMap, uint8_t* OutBuffer,
                      size_t Size);

    void RGBCompressed(uint8_t* InBuffer, uint8_t* OutBuffer, size_t Size);
    void RGBACompressed(uint8_t* InBuffer, uint8_t* OutBuffer, size_t Size);
    void MonochromeCompressed(uint8_t* InBuffer, uint8_t* OutBuffer,
                              size_t Size);

   public:
    TGA()
        : Data(nullptr),
          Size(0),
          Width(0),
          Height(0),
          Format(ImageFormat::Undefined) {}

    uint8_t* GetData() const { return Data; }
    uint64_t GetSize() const { return Size; }
    uint32_t GetWidth() const { return Width; }
    uint32_t GetHeight() const { return Height; }
    ImageFormat GetFormat() const { return Format; }

    void Clear() {
        delete[] Data;
        Size = 0;
        Width = 0;
        Height = 0;
        Format = ImageFormat::Undefined;
    }

    /**
     * @brief Loads image.
     *
     * **This function clears data i.e. invalidates data pointer.**
     *
     * @param Filename Image filename.
     * @return true if success, otherwise false.
     */
    bool Load(const std::string& Filename);

    ~TGA() { Clear(); }
};

}  // namespace tga
#endif
