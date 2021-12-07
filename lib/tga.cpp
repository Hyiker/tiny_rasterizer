#include "lib/tga.hpp"
using namespace tga;
template <typename Type>
void TGA::RGBPaletted(Type* InBuffer, uint8_t* ColorMap, uint8_t* OutBuffer,
                      size_t Size) {
    const int PixelSize = 3;
    Type Index;
    uint8_t Red, Green, Blue;
    uint8_t* ColorMapPtr;

    for (size_t i = 0; i < Size; i++) {
        Index = InBuffer[i];
        ColorMapPtr = &ColorMap[Index * PixelSize];

        ReadPixel24(ColorMapPtr, Red, Green, Blue);
        WritePixel24(OutBuffer, Red, Green, Blue);
    }
}

template <typename Type>
void TGA::RGBAPaletted(Type* InBuffer, uint8_t* ColorMap, uint8_t* OutBuffer,
                       size_t Size) {
    const int PixelSize = 4;
    Type Index;
    uint8_t Red, Green, Blue, Alpha;
    uint8_t* ColorMapPtr;

    for (size_t i = 0; i < Size; i++) {
        Index = InBuffer[i];
        ColorMapPtr = &ColorMap[Index * PixelSize];

        ReadPixel32(ColorMapPtr, Red, Green, Blue, Alpha);
        WritePixel32(OutBuffer, Red, Green, Blue, Alpha);
    }
}

void TGA::MonochromeCompressed(uint8_t* InBuffer, uint8_t* OutBuffer,
                               size_t Size) {
    uint8_t Header;
    uint8_t Red;
    size_t i, j, PixelCount;

    for (i = 0; i < Size;) {
        Header = *InBuffer++;
        PixelCount = (Header & 0x7F) + 1;

        if (Header & 0x80) {
            ReadPixel8(InBuffer, Red);

            for (j = 0; j < PixelCount; j++) {
                WritePixel8(OutBuffer, Red);
            }

            i += PixelCount;
        } else {
            for (j = 0; j < PixelCount; j++) {
                ReadPixel8(InBuffer, Red);
                WritePixel8(OutBuffer, Red);
            }

            i += PixelCount;
        }
    }
}

void TGA::RGBCompressed(uint8_t* InBuffer, uint8_t* OutBuffer, size_t Size) {
    uint8_t Header;
    uint8_t Red, Green, Blue;
    size_t i, j, PixelCount;

    for (i = 0; i < Size;) {
        Header = *InBuffer++;
        PixelCount = (Header & 0x7F) + 1;

        if (Header & 0x80) {
            ReadPixel24(InBuffer, Red, Green, Blue);

            for (j = 0; j < PixelCount; j++) {
                WritePixel24(OutBuffer, Red, Green, Blue);
            }

            i += PixelCount;
        } else {
            for (j = 0; j < PixelCount; j++) {
                ReadPixel24(InBuffer, Red, Green, Blue);
                WritePixel24(OutBuffer, Red, Green, Blue);
            }
            i += PixelCount;
        }
    }
}

void TGA::RGBACompressed(uint8_t* InBuffer, uint8_t* OutBuffer, size_t Size) {
    uint8_t Header;
    uint8_t Red, Green, Blue, Alpha;
    size_t i, j, PixelCount;

    for (i = 0; i < Size;) {
        Header = *InBuffer++;
        PixelCount = (Header & 0x7F) + 1;

        if (Header & 0x80) {
            ReadPixel32(InBuffer, Red, Green, Blue, Alpha);

            for (j = 0; j < PixelCount; j++) {
                WritePixel32(OutBuffer, Red, Green, Blue, Alpha);
            }

            i += PixelCount;
        } else {
            for (j = 0; j < PixelCount; j++) {
                ReadPixel32(InBuffer, Red, Green, Blue, Alpha);
                WritePixel32(OutBuffer, Red, Green, Blue, Alpha);
            }

            i += PixelCount;
        }
    }
}

bool TGA::Load(const std::string& Filename) {
    Clear();

    std::ifstream File(Filename, std::ios::binary);
    if (!File.is_open()) return false;

    Header Head;
    size_t FileSize = 0;

    File.seekg(0, std::ios_base::end);
    FileSize = File.tellg();
    File.seekg(0, std::ios_base::beg);

    File.read((char*)&Head.IDLength, sizeof(Head.IDLength));
    File.read((char*)&Head.ColorMapType, sizeof(Head.ColorMapType));
    File.read((char*)&Head.ImageType, sizeof(Head.ImageType));
    File.read((char*)&Head.ColorMapOrigin, sizeof(Head.ColorMapOrigin));
    File.read((char*)&Head.ColorMapLength, sizeof(Head.ColorMapLength));
    File.read((char*)&Head.ColorMapEntrySize, sizeof(Head.ColorMapEntrySize));
    File.read((char*)&Head.XOrigin, sizeof(Head.XOrigin));
    File.read((char*)&Head.YOrigin, sizeof(Head.YOrigin));
    File.read((char*)&Head.Width, sizeof(Head.Width));
    File.read((char*)&Head.Height, sizeof(Head.Height));
    File.read((char*)&Head.Bits, sizeof(Head.Bits));
    File.read((char*)&Head.ImageDescriptor, sizeof(Head.ImageDescriptor));

    uint8_t* Descriptor = new uint8_t[Head.ImageDescriptor];
    File.read((char*)Descriptor, Head.ImageDescriptor);

    size_t ColorMapElementSize = Head.ColorMapEntrySize / 8;
    size_t ColorMapSize = Head.ColorMapLength * ColorMapElementSize;
    uint8_t* ColorMap = new uint8_t[ColorMapSize];

    if (Head.ColorMapType == 1) {
        File.read((char*)ColorMap, ColorMapSize);
    }

    size_t PixelSize =
        Head.ColorMapLength == 0 ? (Head.Bits / 8) : ColorMapElementSize;
    size_t DataSize =
        FileSize - sizeof(Header) - (Head.ColorMapType == 1 ? ColorMapSize : 0);
    size_t ImageSize = Head.Width * Head.Height * PixelSize;

    uint8_t* Buffer = new uint8_t[DataSize];
    File.read((char*)Buffer, DataSize);

    Data = new uint8_t[ImageSize];
    memset(Data, 0, ImageSize);

    switch (Head.ImageType) {
        case 0:
            break;  // No Image
        case 1:     // Uncompressed paletted
        {
            if (Head.Bits == 8) {
                switch (PixelSize) {
                    case 3:
                        RGBPaletted((uint8_t*)Buffer, ColorMap, Data,
                                    Head.Width * Head.Height);
                        break;
                    case 4:
                        RGBAPaletted((uint8_t*)Buffer, ColorMap, Data,
                                     Head.Width * Head.Height);
                        break;
                }
            } else if (Head.Bits == 16) {
                switch (PixelSize) {
                    case 3:
                        RGBPaletted((uint16_t*)Buffer, ColorMap, Data,
                                    Head.Width * Head.Height);
                        break;
                    case 4:
                        RGBAPaletted((uint16_t*)Buffer, ColorMap, Data,
                                     Head.Width * Head.Height);
                        break;
                }
            }

            break;
        }
        case 2:  // Uncompressed TrueColor
        {
            if (Head.Bits == 24 || Head.Bits == 32) {
                std::copy(&Buffer[0], &Buffer[ImageSize], &Data[0]);

                for (size_t i = 0; i < ImageSize; i += PixelSize) {
                    std::swap(Data[i], Data[i + 2]);
                }
            }

            break;
        }

        case 3:  // Uncompressed Monochrome
        {
            if (Head.Bits == 8) {
                std::copy(&Buffer[0], &Buffer[ImageSize], &Data[0]);
            }

            break;
        }

        case 9:
            break;  // Compressed paletted TODO
        case 10:    // Compressed TrueColor
        {
            switch (Head.Bits) {
                case 24:
                    RGBCompressed(Buffer, Data, Head.Width * Head.Height);
                    break;
                case 32:
                    RGBACompressed(Buffer, Data, Head.Width * Head.Height);
                    break;
            }

            break;
        }

        case 11:  // Compressed Monocrhome
        {
            if (Head.Bits == 8) {
                MonochromeCompressed(Buffer, Data, Head.Width * Head.Height);
            }

            break;
        }
    }

    if (Head.ImageType != 0) {
        switch (PixelSize) {
            case 1:
                Format = ImageFormat::Monochrome;
                break;
            case 3:
                Format = ImageFormat::RGB;
                break;
            case 4:
                Format = ImageFormat::RGBA;
                break;
        }

        Width = Head.Width;
        Height = Head.Height;
        Size = ImageSize;
    }

    delete[] ColorMap;
    delete[] Descriptor;

    return true;
}