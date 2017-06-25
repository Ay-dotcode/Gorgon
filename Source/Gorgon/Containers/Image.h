#pragma once

#include <vector>

#include "../Types.h"
#include "../Geometry/Point.h"
#include "../Geometry/Size.h"
#include "../Graphics/Color.h"
#include "../IO/Stream.h"

namespace Gorgon {
	namespace Containers {

		/// This class is a container for image data. It supports different color modes and access to the
		/// underlying data through () operator. This object implements move semantics. Since copy constructor is
		/// expensive, it is deleted against accidental use. If a copy of the object is required, use Duplicate function.
		class Image {
		public:

			/// Constructs an empty image data
			Image() {
				
			}

			/// Constructs a new image data with the given width, height and color mode. This constructor 
			/// does not initialize data inside the image
			Image(const Geometry::Size &size, Graphics::ColorMode mode) : size(size), mode(mode) {
				bpp=Graphics::GetBytesPerPixel(mode);
				data=(Byte*)malloc(size.Area()*bpp*sizeof(Byte));
			}

			/// Copy constructor is disabled
			Image(const Image &) = delete;

			/// Move constructor
			Image(Image &&data) : Image() {
				Swap(data);
			}

			/// Copy assignment is disabled
			Image &operator=(const Image &) = delete;

			/// Move assignment
			Image &operator=(Image &&other) { 
				if(this == &other) return *this;
				
				Destroy();
				Swap(other);
				
				return *this;
			}

			/// Destructor
			~Image() {
				Destroy();
			}

			/// Duplicates this image, essentially performing the work of copy constructor
			Image Duplicate() const {
				Image data;
				data.Assign(this->data, size, mode);

				return data;
			}

			/// Resizes the image to the given size and color mode. This function discards the contents
			/// of the image and does not perform any initialization.
			void Resize(const Geometry::Size &size, Graphics::ColorMode mode) {
#ifndef NDEBUG
				if(!size.IsValid())
					throw std::runtime_error("Image size cannot be negative");
#endif

				// Check if resize is really necessary
				if(this->size==size && this->bpp==Graphics::GetBytesPerPixel(mode))
					return;

				this->size   = size;
				this->mode   = mode;
				this->bpp    = Graphics::GetBytesPerPixel(mode);

				if(data) {
					free(data);
				}
				
				data=(Byte*)malloc(size.Area()*bpp*sizeof(Byte));
			}

			/// Assigns the image to the copy of the given data. Ownership of the given data
			/// is not transferred. If the given data is not required elsewhere, consider using
			/// Assume function. This variant performs resize and copy at the same time. The given 
			/// data should have the size of width*height*Graphics::GetBytesPerPixel(mode)*sizeof(Byte). 
			/// This function does not perform any checks for the data size while copying it.
			/// If width or height is 0, the newdata is not accessed and this method effectively
			/// Destroys the current image. In this case, both width and height should be specified as 0.
			void Assign(Byte *newdata, const Geometry::Size &size, Graphics::ColorMode mode) {
#ifndef NDEBUG
				if(!size.IsValid())
					throw std::runtime_error("Image size cannot be negative");
#endif
				this->size   = size;
				this->mode   = mode;
				this->bpp    = Graphics::GetBytesPerPixel(mode);

				if(data && data!=newdata) {
					free(data);
				}
				
				if(size.Area()*bpp>0) {
					data=(Byte*)malloc(size.Area()*bpp*sizeof(Byte));
					memcpy(data, newdata, size.Area()*bpp*sizeof(Byte));
				}
				else {
					data=nullptr;
				}
			}

			/// Assigns the image to the copy of the given data. Ownership of the given data
			/// is not transferred. If the given data is not required elsewhere, consider using
			/// Assume function. The size and color mode of the image stays the same. The given 
			/// data should have the size of width*height*Graphics::GetBytesPerPixel(mode)*sizeof(Byte). 
			/// This function does not perform any checks for the data size while copying it.
			void Assign(Byte *newdata) {
				memcpy(data, newdata, size.Area()*bpp*sizeof(Byte));
			}

			/// Assumes the ownership of the given data. This variant changes the size and
			/// color mode of the image. The given data should have the size of 
			/// width*height*Graphics::GetBytesPerPixel(mode)*sizeof(Byte). This function
			/// does not perform any checks for the data size while assuming it.
			/// newdata could be nullptr however, in this case
			/// width, height should be 0. mode is not assumed to be ColorMode::Invalid while
			/// the image is empty, therefore it could be specified as any value.
			void Assume(Byte *newdata, const Geometry::Size &size, Graphics::ColorMode mode) {
#ifndef NDEBUG
				if(!size.IsValid())
					throw std::runtime_error("Image size cannot be negative");
#endif
				this->size   = size;
				this->mode   = mode;
				this->bpp    = Graphics::GetBytesPerPixel(mode);

				if(data && data!=newdata) {
					free(data);
				}
				
				data=newdata;
			}

			/// Assumes the ownership of the given data. The size and color mode of the image stays the same.
			/// The given data should have the size of width*height*Graphics::GetBytesPerPixel(mode)*sizeof(Byte).
			/// This function does not perform any checks for the data size while assuming it.
			void Assume(Byte *newdata) {
				if(data && data!=newdata) {
					free(data);
				}
				
				data=newdata;
			}

			/// Returns and disowns the current data buffer. If image is empty, this method will return a nullptr.
			Byte *Release() {
				auto temp=data;
				data=nullptr;
				Destroy();

				return temp;
			}

			/// Cleans the contents of the buffer by setting every byte it contains to 0.
			void Clear() {
#ifndef NDEBUG
				if(!data) {
					throw std::runtime_error("Image data is empty");
				}
#endif

				memset(data, 0, size.Area()*bpp*sizeof(Byte));
			}

			/// Destroys this image by setting width and height to 0 and freeing the memory
			/// used by its data. Also color mode is set to ColorMode::Invalid
			void Destroy() {
				if(data) {
					free(data);
					data=nullptr;
				}
				size   = {0, 0};
				bpp    = 0;
				mode   = Graphics::ColorMode::Invalid;
			}

			/// Swaps this image with another. This function is used to implement move semantics.
			void Swap(Image &other) {
				using std::swap;

				swap(size,   other.size);
				swap(bpp,    other.bpp);
				swap(data,   other.data);
				swap(mode,   other.mode);
			}

			/// Returns the raw data pointer
			Byte *RawData() {
				return data;
			}

			/// Returns the raw data pointer
			const Byte *RawData() const {
				return data;
			}

			/// Converts this image data to RGBA buffer
			void ConvertToRGBA() {
				if(!data) return;

				switch(mode) {
				case Graphics::ColorMode::BGRA:
					for(int i=0; i<size.Area(); i++) {
						std::swap(data[i*4+2], data[i*4+0]);
					}
					break;

				case Graphics::ColorMode::Grayscale_Alpha: {
					auto pdata = data;
					data = (Byte*)malloc(size.Area()*4);

					for(int i=0; i<size.Area(); i++) {
						data[i*4+0] = pdata[i*2+0];
						data[i*4+1] = pdata[i*2+0];
						data[i*4+2] = pdata[i*2+0];
						data[i*4+3] = pdata[i*2+1];
					}
					delete pdata;
				}
				break;

				case Graphics::ColorMode::Grayscale: {
					auto pdata = data;
					data = (Byte*)malloc(size.Area()*4);

					for(int i=0; i<size.Area(); i++) {
						data[i*4+0] = pdata[i+0];
						data[i*4+1] = pdata[i+0];
						data[i*4+2] = pdata[i+0];
						data[i*4+3] = 255;
					}
					delete pdata;
				}
				break;

				case Graphics::ColorMode::Alpha: {
					auto pdata = data;
					data = (Byte*)malloc(size.Area()*4);

					for(int i=0; i<size.Area(); i++) {
						data[i*4+0] = 255;
						data[i*4+1] = 255;
						data[i*4+2] = 255;
						data[i*4+3] = pdata[i+0];
					}
					delete pdata;
				}
				break;

				case Graphics::ColorMode::RGB: {
					auto pdata = data;
					data = (Byte*)malloc(size.Area()*4);

					for(int i=0; i<size.Area(); i++) {
						data[i*4+0] = pdata[i*3+0];
						data[i*4+1] = pdata[i*3+1];
						data[i*4+2] = pdata[i*3+2];
						data[i*4+3] = 255;
					}
					delete pdata;
				}
				break;

				case Graphics::ColorMode::BGR: {
					auto pdata = data;
					data = (Byte*)malloc(size.Area()*4);

					for(int i=0; i<size.Area(); i++) {
						data[i*4+0] = pdata[i*3+2];
						data[i*4+1] = pdata[i*3+1];
						data[i*4+2] = pdata[i*3+0];
						data[i*4+3] = 255;
					}
					delete pdata;
				}
				break;

				}

				mode = Graphics::ColorMode::RGBA;
			}

			/// Copies this image to a RGBA buffer, buffer should be resize before calling this function
			void CopyToRGBABuffer(Byte *buffer) const {
				if(!data) return;

				int i;

				switch(mode) {
					case Graphics::ColorMode::RGBA:
						memcpy(buffer, data, size.Area()*4);
						break;

					case Graphics::ColorMode::BGRA:
						for(i=0; i<size.Area(); i++) {
							buffer[i*4+0] = data[i*4+2];
							buffer[i*4+1] = data[i*4+1];
							buffer[i*4+2] = data[i*4+0];
							buffer[i*4+3] = data[i*4+3];
						}
						break;

					case Graphics::ColorMode::Grayscale_Alpha:
						for(i=0; i<size.Area(); i++) {
							buffer[i*4+0] = data[i*2+0];
							buffer[i*4+1] = data[i*2+0];
							buffer[i*4+2] = data[i*2+0];
							buffer[i*4+3] = data[i*2+1];
						}
						break;

					case Graphics::ColorMode::Grayscale:
						for(i=0; i<size.Area(); i++) {
							buffer[i*4+0] = data[i+0];
							buffer[i*4+1] = data[i+0];
							buffer[i*4+2] = data[i+0];
							buffer[i*4+3] = 255;
						}
						break;

					case Graphics::ColorMode::Alpha: {
						for(i=0; i<size.Area(); i++) 
							buffer[i*4+0] = 255;
							buffer[i*4+1] = 255;
							buffer[i*4+2] = 255;
							buffer[i*4+3] = data[i+0];
						}
						break;

					case Graphics::ColorMode::RGB: 
						for(i=0; i<size.Area(); i++) {
							buffer[i*4+0] = data[i*3+0];
							buffer[i*4+1] = data[i*3+1];
							buffer[i*4+2] = data[i*3+2];
							buffer[i*4+3] = 255;
						}
						break;

					case Graphics::ColorMode::BGR:
						for(i=0; i<size.Area(); i++) {
							buffer[i*4+0] = data[i*3+2];
							buffer[i*4+1] = data[i*3+1];
							buffer[i*4+2] = data[i*3+0];
							buffer[i*4+3] = 255;
						}
						break;

				}
			}

			/// Copies this image to a RGBA buffer, buffer should be resize before calling this function.
			/// This function is here mostly to create icon for Win32
			void CopyToBGRABuffer(Byte *buffer) const {
				if(!data) return;

				int i;

				switch(mode) {
					case Graphics::ColorMode::BGRA:
						memcpy(buffer, data, size.Area()*4);
						break;

					case Graphics::ColorMode::RGBA:
						for(i=0; i<size.Area(); i++) {
							buffer[i*4+0] = data[i*4+2];
							buffer[i*4+1] = data[i*4+1];
							buffer[i*4+2] = data[i*4+0];
							buffer[i*4+3] = data[i*4+3];
						}
						break;

					case Graphics::ColorMode::Grayscale_Alpha:
						for(i=0; i<size.Area(); i++) {
							buffer[i*4+0] = data[i*2+0];
							buffer[i*4+1] = data[i*2+0];
							buffer[i*4+2] = data[i*2+0];
							buffer[i*4+3] = data[i*2+1];
						}
						break;

					case Graphics::ColorMode::Grayscale:
						for(i=0; i<size.Area(); i++) {
							buffer[i*4+0] = data[i+0];
							buffer[i*4+1] = data[i+0];
							buffer[i*4+2] = data[i+0];
							buffer[i*4+3] = 255;
						}
						break;

					case Graphics::ColorMode::Alpha: {
						for(i=0; i<size.Area(); i++) 
							buffer[i*4+0] = 255;
							buffer[i*4+1] = 255;
							buffer[i*4+2] = 255;
							buffer[i*4+3] = data[i+0];
						}
						break;

					case Graphics::ColorMode::BGR: 
						for(i=0; i<size.Area(); i++) {
							buffer[i*4+0] = data[i*3+0];
							buffer[i*4+1] = data[i*3+1];
							buffer[i*4+2] = data[i*3+2];
							buffer[i*4+3] = 255;
						}
						break;

					case Graphics::ColorMode::RGB:
						for(i=0; i<size.Area(); i++) {
							buffer[i*4+0] = data[i*3+2];
							buffer[i*4+1] = data[i*3+1];
							buffer[i*4+2] = data[i*3+0];
							buffer[i*4+3] = 255;
						}
						break;

				}
			}

			/// Imports a given bitmap file. BMP RLE compression and colorspaces are not supported.
			bool ImportBMP(const std::string &filename, bool dib = false) {
				std::ifstream file(filename, std::ios::binary);

				if(!file.is_open()) return false;

				return ImportBMP(file, dib);
			}

			/// Imports a given bitmap file. BMP RLE compression and colorspaces are not supported.
			bool ImportBMP(std::istream &file, bool dib = false) {
				using namespace IO;

				unsigned long off = 0;

				if(!dib) {
					if(ReadString(file, 2) != "BM") return false;

					auto fsize = IO::ReadUInt32(file);

					ReadUInt16(file); //reserved 1
					ReadUInt16(file); //reserved 2

					off = ReadUInt32(file);
				}

				auto headersize = IO::ReadUInt32(file);

				int width, height;
				int bpp;
				bool upsidedown = false;
				bool bitcompress = false;
				bool grayscalepalette = true;
				int colorsused = 0;
				bool alpha = false;
				int redshift, greenshift, blueshift, alphashift;
				float redmult = 1, greenmult = 1, bluemult = 1, alphamult = 1;
				uint32_t redmask = 0, greenmask = 0, bluemask = 0, alphamask = 0;

				std::vector<Graphics::RGBA> palette;

				auto shiftcalc=[](uint32_t v) {
					int pos = 0;
					while(v) {
						if(v&1) return pos;
						v=v>>1;
						pos++;
					}

					return pos;
				};

				if(headersize == 12) { //2x bmp
					width = ReadInt16(file);
					height = ReadInt16(file);
					ReadUInt16(file); //planes
					bpp = ReadUInt16(file);
				}

				if(headersize >= 40) { //3x
					width = ReadInt32(file);
					height = ReadInt32(file);
					ReadUInt16(file); //planes
					bpp = ReadUInt16(file);

					auto compress = ReadUInt32(file);

					if(compress != 0 && compress != 3) return false;

					bitcompress = compress != 0;

					ReadUInt32(file); //size of bitmap

					ReadInt32(file); //horz resolution
					ReadInt32(file); //vert resolution

					colorsused = ReadUInt32(file);
					ReadUInt32(file); //colors important

					if(bitcompress && headersize == 40) {
						redmask = (uint32_t)ReadUInt32(file);
						greenmask = (uint32_t)ReadUInt32(file);
						bluemask = (uint32_t)ReadUInt32(file);
					}
					else if(bpp == 16) {
						redmask   = 0x7c00;
						greenmask = 0x03e0;
						bluemask  = 0x001f;
					}
					else if(bpp == 32) {
						redmask   = 0x00ff0000;
						greenmask = 0x0000ff00;
						bluemask  = 0x000000ff;
					}
				}

				if(headersize >= 108) {
					redmask = (uint32_t)ReadUInt32(file);
					greenmask = (uint32_t)ReadUInt32(file);
					bluemask = (uint32_t)ReadUInt32(file);
					alphamask = (uint32_t)ReadUInt32(file);

					file.seekg(13*4, std::ios::cur); //colorspace information
				}

				redshift = shiftcalc(redmask);
				greenshift = shiftcalc(greenmask);
				blueshift = shiftcalc(bluemask);
				alphashift = shiftcalc(alphamask);

				if(redmask) {
					redmult = 255.f / (redmask>>redshift);
				}
				if(greenmask) {
					greenmult = 255.f / (greenmask>>greenshift);
				}
				if(bluemask) {
					bluemult = 255.f / (bluemask>>blueshift);
				}
				if(alphamask) {
					alphamult = 255.f / (alphamask>>alphashift);
				}

				if(headersize > 108) {
					file.seekg(headersize - 108, std::ios::cur);
				}

				if(height > 0)
					upsidedown = true;
				else
					height *= -1;

				if(bpp <= 8) { //paletted
					if(colorsused == 0)
						colorsused = 1 << bpp;

					palette.reserve(colorsused);

					for(int i=0; i<colorsused; i++) {
						Byte r, g, b, a;
						
						b = ReadUInt8(file);
						g = ReadUInt8(file);
						r = ReadUInt8(file);

						if(r!=b || b!=g) grayscalepalette = false;

						if(headersize > 12) {
							a = ReadUInt8(file); //reserved
						}

						palette.emplace_back(r, g, b, a);
					}

					for(int i=0; i<colorsused; i++) {
						if(palette[i].A) {
							alpha = true;
							break;
						}
					}
				}

				if(!dib)
					file.seekg(off, std::ios::beg);

				if((bpp == 32 || bpp == 16) && alphamask != 0) {
					Resize({width, height}, Graphics::ColorMode::RGBA);
					alpha = true;
				}
				else if(bpp <= 8 && grayscalepalette) {
					if(alpha)
						Resize({width, height}, Graphics::ColorMode::Grayscale_Alpha);
					else 
						Resize({width, height}, Graphics::ColorMode::Grayscale);
				}
				else if(alpha) {
					if(redmask == 0 && greenmask == 0 && bluemask == 0) {
						Resize({width, height}, Graphics::ColorMode::Alpha);
					}
					else {
						Resize({width, height}, Graphics::ColorMode::RGBA);
					}
				}
				else {
					Resize({width, height}, Graphics::ColorMode::RGB);
				}

				int ys, ye, yc;

				if(upsidedown) {
					ys = height-1;
					ye = -1;
					yc = -1;
				}
				else {
					ys = 0;
					ye = height;
					yc = 1;
				}

				if(bpp == 24) {
					for(int y = ys; y!=ye; y += yc) {
						int bytes = 0;
						for(int x=0; x<width; x++) {
							this->operator ()({x, y}, 0) = ReadUInt8(file);
							this->operator ()({x, y}, 1) = ReadUInt8(file);
							this->operator ()({x, y}, 2) = ReadUInt8(file);

							bytes += 3;
						}

						if(bytes%4) {
							file.seekg(4-bytes%4, std::ios::cur);
						}
					}
				}
				else if(bpp == 16 || bpp == 32) {
					for(int y = ys; y!=ye; y += yc) {
						int bytes = 0;
						for(int x=0; x<width; x++) {
							uint32_t data;
							
							if(bpp == 16)
								data = ReadUInt16(file);
							else
								data = ReadUInt32(file);

							if(redmask != 0 || greenmask != 0 || bluemask != 0) {
								this->operator ()({x, y}, 0) = (Byte)std::round(((data&redmask)>>redshift) * redmult);
								this->operator ()({x, y}, 1) = (Byte)std::round(((data&greenmask)>>greenshift) * greenmult);
								this->operator ()({x, y}, 2) = (Byte)std::round(((data&bluemask)>>blueshift) * bluemult);
							}

							if(alpha) {
								if(redmask != 0 || greenmask != 0 || bluemask != 0) {
									this->operator ()({x, y}, 3) = (Byte)std::round(((data&alphamask)>>alphashift) * alphamult);
								}
								else {
									this->operator ()({x, y}, 0) = (Byte)std::round(((data&alphamask)>>alphashift) * alphamult);
								}
							}
							
							bytes += bpp/8;
						}

						if(bytes%4) {
							file.seekg(4-bytes%4, std::ios::cur);
						}
					}
				}
				else {
					Byte bitmask = (1 << bpp) - 1;
					bitmask = bitmask << (8-bpp);
					for(int y = ys; y!=ye; y += yc) {
						int bytes = 0;
						int bits  = 0;
						Byte v = 0;
						for(int x=0; x<width; x++) {
							int ind;
							if(bits == 0) {
								v = ReadUInt8(file);
								bits = 8;
								bytes++;
							}

							ind = (v&bitmask)>>(8-bpp);
							if(ind >= colorsused)
								continue;

							auto col = palette[ind];
							
							if(grayscalepalette) {
								this->operator ()({x, y}, 0) = col.R;

								if(alpha)
									this->operator ()({x, y}, 1) = col.A;
							}
							else {
								this->operator ()({x, y}, 0) = col.R;
								this->operator ()({x, y}, 1) = col.G;
								this->operator ()({x, y}, 2) = col.B;
								

								if(alpha) {
									this->operator ()({x, y}, 3) = col.A;
								}
							}

							v = v<<bpp;
							bits -= bpp;
						}

						if(bytes%4) {
							file.seekg(4-bytes%4, std::ios::cur);
						}
					}
				}

				return true;
			}

			/// Exports the image as a bitmap. RGB is exported as 24-bit, RGBA, BGR, BGRA is exported
			/// as 32-bit, Grayscale exported as 8-bit, Grayscale alpha, alpha only is exported as
			/// 16-bit
			bool ExportBMP(const std::string &filename, bool dib = false) {
				std::ofstream file(filename, std::ios::binary);

				if(!file.is_open()) return false;

				return ExportBMP(file, dib);
			}

			/// Exports the image as a bitmap. RGB is exported as 24-bit, RGBA, BGR, BGRA is exported
			/// as 32-bit, Grayscale exported as 8-bit, Grayscale alpha, alpha only is exported as
			/// 16-bit
			bool ExportBMP(std::ostream &file, bool usev4 = false, bool dib = false) {
				using namespace IO;
				using Graphics::ColorMode;

				long datasize = 0;
				long headersize = 0;
				long extraspace = 0;
				int bpp = 0;
				int compression = 0;
				int stride = 0;

				switch(mode) {
				case ColorMode::RGB:
				case ColorMode::BGR:
					stride = 3 * size.Width;
					if(stride%4) stride += (4-(stride%4));

					datasize = stride * size.Height;

					headersize = 40;
					bpp = 24;
					break;

				case ColorMode::RGBA:
				case ColorMode::BGRA:
				case ColorMode::Grayscale_Alpha:
					stride = 4 * size.Width;
					if(stride%4) stride += (4-(stride%4));

					datasize = stride * size.Height;

					headersize = 108;
					bpp = 32;
					compression = 3;
					break;

				case ColorMode::Grayscale:
					stride = size.Width;
					if(stride%4) stride += (4-(stride%4));

					datasize = stride * size.Height;

					headersize = 40;
					extraspace = 256 * 4; //palette
					bpp = 8;
					break;

				case ColorMode::Alpha:
					stride = 2 * size.Width;
					if(stride%4) stride += (4-(stride%4));

					datasize = stride * size.Height;

					headersize = 108;
					datasize = stride * size.Height;
					compression = 3;
					bpp = 16;
					break;

				default:
					throw std::runtime_error("Unsupported color mode");
				}

				
				//header
				if(!dib) {
					WriteString(file, "BM");
					WriteUInt32(file, 14 + headersize + extraspace + datasize);
					WriteUInt16(file, 0); //reserved 1
					WriteUInt16(file, 0); //reserved 2
					WriteUInt32(file, 14 + headersize + extraspace);
				}

				WriteUInt32(file, headersize);
				WriteInt32 (file, size.Width);
				WriteInt32 (file, size.Height);
				WriteUInt16(file, 1);
				WriteUInt16(file, bpp);
				WriteUInt32(file, compression);
				WriteUInt32(file, datasize);
				WriteInt32 (file, 2834);
				WriteInt32 (file, 2834);
				WriteInt32(file, 0); //colors used
				WriteInt32(file, 0); //colors important

				if(compression == 3) {
					if(mode == ColorMode::Alpha) {
						WriteUInt32(file, 0x00000001);
						WriteUInt32(file, 0x00000002);
						WriteUInt32(file, 0x00000004);
					}
					else if(mode == ColorMode::BGRA) {
						WriteUInt32(file, 0x00ff0000);
						WriteUInt32(file, 0x0000ff00);
						WriteUInt32(file, 0x000000ff);
					}
					else {	
						WriteUInt32(file, 0x000000ff);
						WriteUInt32(file, 0x0000ff00);
						WriteUInt32(file, 0x00ff0000);
					}
				}

				if(headersize == 108 || usev4) {
					if(compression != 3) {
						WriteUInt32(file, 0x00000001);
						WriteUInt32(file, 0x00000002);
						WriteUInt32(file, 0x00000004);
					}
					if(mode == ColorMode::Alpha) {
						WriteUInt32(file, 0x0000ff00);
					}
					else {
						WriteUInt32(file, 0xff000000);
					}
					WriteUInt32(file, 1); //device dependent RGB
					for(int i=0; i<12; i++)  //color profile settings, all 0
						WriteUInt32(file, 0);
				}

				int ostride;
				switch(mode) {
					case ColorMode::RGB:
						ostride = size.Width*3;
						for(int y=size.Height-1; y>=0; y--) {
							WriteArray(file, data+y*ostride, ostride);

							for(int j=0; j<stride-ostride; j++)
								WriteUInt8(file, 0);
						}
						break;

					case ColorMode::BGR:
						ostride = size.Width*3;

						for(int y=size.Height-1; y>=0; y--) {
							WriteArray(file, data+y*ostride, ostride);

							for(int j=0; j<stride-ostride; j++)
								WriteUInt8(file, 0);
						}
						break;


					case ColorMode::RGBA:
					case ColorMode::BGRA:
						ostride = size.Width*4;
						for(int y=size.Height-1; y>=0; y--) {
							WriteArray(file, data+y*ostride, ostride);
						}
						break;

					case ColorMode::Grayscale:
						//write palette
						for(int i=0; i<256; i++) {
							WriteUInt8(file, (Byte)i);
							WriteUInt8(file, (Byte)i);
							WriteUInt8(file, (Byte)i);
							WriteUInt8(file, 0);
						}

						//write data
						ostride = size.Width;
						for(int y=size.Height-1; y>=0; y--) {
							WriteArray(file, data+y*ostride, ostride);

							for(int j=0; j<stride-ostride; j++)
								WriteUInt8(file, 0);
						}


						break;
						
					
					case ColorMode::Grayscale_Alpha:
						ostride = size.Width*2;

						for(int y=size.Height-1; y>=0; y--) {
							for(int x=0; x<size.Width; x++) {
								WriteUInt8(file, data[y*ostride+x*2]);
								WriteUInt8(file, data[y*ostride+x*2]);
								WriteUInt8(file, data[y*ostride+x*2]);
								WriteUInt8(file, data[y*ostride+x*2+1]);
							}
						}
						break;


					case ColorMode::Alpha:
						ostride = size.Width;

						for(int y=size.Height-1; y>=0; y--) {
							for(int x=0; x<size.Width; x++) {
								WriteUInt8(file, 0xff);
								WriteUInt8(file, data[y*ostride+x]);
							}

							for(int j=0; j<stride-ostride*2; j++)
								WriteUInt8(file, 0);
						}
						break;
				}

				return true;
			}

			/// Provides access to the given component in x and y coordinates. This
			/// function performs bounds checking only on debug mode.
			Byte &operator()(const Geometry::Point &p, unsigned component=0) {
#ifndef NDEBUG
				if(p.X<0 || p.Y<0 || p.X>=size.Width || p.Y>=size.Height || component>=bpp) {
					throw std::runtime_error("Index out of bounds");
				}
#endif
				return data[bpp*(size.Width*p.Y+p.X)+component];
			}

			/// Provides access to the given component in x and y coordinates. This
			/// function performs bounds checking only on debug mode.
			Byte operator()(const Geometry::Point &p, unsigned component=0) const {
#ifndef NDEBUG
				if(p.X<0 || p.Y<0 || p.X>=size.Width || p.Y>=size.Height || component>=bpp) {
					throw std::runtime_error("Index out of bounds");
				}
#endif
				return data[bpp*(size.Width*p.Y+p.X)+component];
			}

			/// Provides access to the given component in x and y coordinates. This
			/// function returns 0 if the given coordinates are out of bounds. This
			/// function works slower than the () operator.
			Byte Get(const Geometry::Point &p, unsigned component=0) const {
				if(p.X<0 || p.Y<0 || p.X>=size.Width || p.Y>=size.Height || component>=bpp) {
					return 0;
				}

				return data[bpp*(size.Width*p.Y+p.X)+component];
			}

			/// Provides access to the given component in x and y coordinates. This
			/// function performs bounds checking only on debug mode.
			Byte &operator()(int x, int y, unsigned component=0) {
#ifndef NDEBUG
				if(x<0 || y<0 || x>=size.Width || y>=size.Height || component>=bpp) {
					throw std::runtime_error("Index out of bounds");
				}
#endif
				return data[bpp*(size.Width*y+x)+component];
			}

			/// Provides access to the given component in x and y coordinates. This
			/// function performs bounds checking only on debug mode.
			Byte operator()(int x, int y, unsigned component=0) const {
#ifndef NDEBUG
				if(x<0 || y<0 || x>=size.Width || y>=size.Height || component>=bpp) {
					throw std::runtime_error("Index out of bounds");
				}
#endif
				return data[bpp*(size.Width*y+x)+component];
			}

			/// Provides access to the given component in x and y coordinates. This
			/// function returns 0 if the given coordinates are out of bounds. This
			/// function works slower than the () operator.
			Byte Get(int x, int y, unsigned component=0) const {
				if(x<0 || y<0 || x>=size.Width || y>=size.Height || component>=bpp) {
					return 0;
				}

				return data[bpp*(size.Width*y+x)+component];
			}

			/// Returns the size of the image
			Geometry::Size GetSize() const {
				return size;
			}

			/// Returns the width of the image
			int GetWidth() const {
				return size.Width;
			}

			/// Returns the height of the image
			int GetHeight() const {
				return size.Height;
			}

			/// Total size of this image in bytes
			unsigned long GetTotalSize() const {
				return size.Area()*bpp;
			}

			/// Returns the color mode of the image
			Graphics::ColorMode GetMode() const {
				return mode;
			}
			
			/// Changes the color mode of the image. Only works if the bits/pixel 
			/// of the target mode is the same as the original
			void ChangeMode(Graphics::ColorMode value) {
                if(Graphics::GetBytesPerPixel(mode) != Graphics::GetBytesPerPixel(value))
                    throw std::runtime_error("Modes differ in number of bits/pixel");
                
                mode = value;
            }

			/// Returns the bytes occupied by a single pixel of this image
			unsigned GetBytesPerPixel() const {
				return bpp;
			}

		protected:
			/// Data that stores pixels of the image
			Byte *data = nullptr;

			/// Width of the image
			Geometry::Size size = {0, 0};

			/// Color mode of the image
			Graphics::ColorMode mode = Graphics::ColorMode::Invalid;

			/// Bytes per pixel information
			unsigned bpp = 0;
		};

		/// Swaps two images. Should be used unqualified for ADL.
		inline void swap(Image &l, Image &r) {
			l.Swap(r);
		}


	}
}
