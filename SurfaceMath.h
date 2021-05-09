#pragma once
#include "MiniginPCH.h"



namespace SurfaceMath
{
	enum _RGB
	{
		R, G, B, A
	};

	namespace internal
	{

#define _min(x,y) ( x > y ? y : x )

		//static unsigned int countSetBits(unsigned int n)
		//{
		//	unsigned int count = 0;

		//	while (n)
		//	{
		//		count += n & 1;
		//		n >>= 1;
		//	}
		//	return count;
		//}

		//static unsigned int findClosestSetPosition(unsigned int n)
		//{
		//	unsigned i = 1, pos = 0;

		//	while ((i & n) > 0)
		//	{
		//		i <<= 1;
		//		++pos;
		//	}

		//	return pos;
		//}


		static Uint32 GetPixel(SDL_Surface* input, unsigned int x, unsigned int y)
		{
			int BytesPerPixel = input->format->BytesPerPixel;
			Uint32 data{};
			int tY = (y * input->pitch);
			int pixelPos = tY + (x * BytesPerPixel);

			memcpy(&data, (void*)((Uint8*)input->pixels + pixelPos), BytesPerPixel);


			return data;
		}

		static void SetPixel(SDL_Surface* surface, Uint32 pixel, unsigned int x, unsigned int y)
		{
			int BytesPerPixel = surface->format->BytesPerPixel;
			int tY = (y * surface->pitch);
			int pixelPos = tY + (x * BytesPerPixel);

			memcpy((void*)((Uint8*)surface->pixels + pixelPos), &pixel, BytesPerPixel);

		}

		static Uint8 GetChannelValue(Uint32 input, Uint32 mask, Uint8 shift, Uint8 loss = 0)
		{
			Uint32 temp;
			temp = input & mask;  /* Isolate red component */
			temp = temp >> shift; /* Shift it down to 8-bit */
			temp = temp << loss;  /* Expand to a full 8-bit number */
			return (Uint8)temp;
		}

		static void SetChannelValue(Uint32 input, Uint32& output, Uint32 mask, Uint8 shift, Uint8 loss)
		{
			//empty masked bits on output
			output = (output & (~mask));

			//fill masked bits with new values
			output |= ((input >> loss) << shift);
		}

		static Uint32 SubstractPixel(Uint32 lhs, SDL_PixelFormat* formatLeft, Uint32 rhs, SDL_PixelFormat* formatRight)
		{
			Uint8 redL, redR, greenL, greenR, blueL, blueR, alphaL, alphaR;

			redL = GetChannelValue(lhs, formatLeft->Rmask, formatLeft->Rshift, formatLeft->Rloss);
			redR = GetChannelValue(rhs, formatRight->Rmask, formatRight->Rshift, formatRight->Rloss);

			greenL = GetChannelValue(lhs, formatLeft->Gmask, formatLeft->Gshift, formatLeft->Gloss);
			greenR = GetChannelValue(rhs, formatRight->Gmask, formatRight->Gshift, formatRight->Gloss);

			blueL = GetChannelValue(lhs, formatLeft->Bmask, formatLeft->Bshift, formatLeft->Bloss);
			blueR = GetChannelValue(rhs, formatRight->Bmask, formatRight->Bshift, formatRight->Bloss);

			alphaL = GetChannelValue(lhs, formatLeft->Amask, formatLeft->Ashift, formatLeft->Aloss);
			alphaR = GetChannelValue(rhs, formatRight->Amask, formatRight->Ashift, formatRight->Aloss);

			Uint32 newRed = (redL < redR) ? 0 : redL - redR;
			Uint32 newGreen = (greenL < greenR) ? 0 : greenL - greenR;
			Uint32 newBlue = (blueL < blueR) ? 0 : blueL - blueR;
			Uint32 newAlpha = alphaL | alphaR;

			Uint32 out{};
			SetChannelValue(newRed, out, formatLeft->Rmask, formatLeft->Rshift, formatLeft->Rloss);
			SetChannelValue(newGreen, out, formatLeft->Gmask, formatLeft->Gshift, formatLeft->Gloss);
			SetChannelValue(newBlue, out, formatLeft->Bmask, formatLeft->Bshift, formatLeft->Bloss);
			SetChannelValue(newAlpha, out, formatLeft->Amask, formatLeft->Ashift, formatLeft->Aloss);

			return out;
		}

		static Uint32 AddPixel(Uint32 lhs, SDL_PixelFormat* formatLeft, Uint32 rhs, SDL_PixelFormat* formatRight)
		{
			Uint16 redL, redR, greenL, greenR, blueL, blueR, alphaL, alphaR;

			redL = GetChannelValue(lhs, formatLeft->Rmask, formatLeft->Rshift, formatLeft->Rloss);
			redR = GetChannelValue(rhs, formatRight->Rmask, formatRight->Rshift, formatRight->Rloss);

			greenL = GetChannelValue(lhs, formatLeft->Gmask, formatLeft->Gshift, formatLeft->Gloss);
			greenR = GetChannelValue(rhs, formatRight->Gmask, formatRight->Gshift, formatRight->Gloss);

			blueL = GetChannelValue(lhs, formatLeft->Bmask, formatLeft->Bshift, formatLeft->Bloss);
			blueR = GetChannelValue(rhs, formatRight->Bmask, formatRight->Bshift, formatRight->Bloss);

			alphaL = GetChannelValue(lhs, formatLeft->Amask, formatLeft->Ashift, formatLeft->Aloss);
			alphaR = GetChannelValue(rhs, formatRight->Amask, formatRight->Ashift, formatRight->Aloss);

			Uint32 newRed = _min(redL + redR, 255);
			Uint32 newGreen = _min(greenL + greenR, 255);
			Uint32 newBlue = _min(blueL + blueR, 255);
			Uint32 newAlpha = alphaL | alphaR;

			Uint32 out{};
			SetChannelValue(newRed, out, formatLeft->Rmask, formatLeft->Rshift, formatLeft->Rloss);
			SetChannelValue(newGreen, out, formatLeft->Gmask, formatLeft->Gshift, formatLeft->Gloss);
			SetChannelValue(newBlue, out, formatLeft->Bmask, formatLeft->Bshift, formatLeft->Bloss);
			SetChannelValue(newAlpha, out, formatLeft->Amask, formatLeft->Ashift, formatLeft->Aloss);

			return out;
		}

		static Uint32 Colorize(Uint32 pixel, SDL_PixelFormat* format, SDL_Color* color, _RGB channel)
		{
			Uint32 out;
			Uint8 value;
			switch (channel)
			{
			default:
			case R: value = GetChannelValue(pixel, format->Rmask, format->Rshift, format->Rloss);
				break;
			case G: value = GetChannelValue(pixel, format->Gmask, format->Gshift, format->Gloss);
				break;
			case B: value = GetChannelValue(pixel, format->Bmask, format->Bshift, format->Bloss);
				break;
			case A: value = GetChannelValue(pixel, format->Amask, format->Ashift, format->Aloss);
				break;
			}

			float normalized = float(value) / 255.f;

			Uint8 red, green, blue, alpha{};

			red		= (Uint8) std::round(normalized * color->r);
			green	= (Uint8) std::round(normalized * color->g);
			blue	= (Uint8) std::round(normalized * color->b);

			if (format->Amask > 0)
			{
				alpha = GetChannelValue(pixel, format->Amask, format->Ashift, format->Aloss);
			}


			SetChannelValue(red, out, format->Rmask, format->Rshift, format->Rloss);
			SetChannelValue(green, out, format->Gmask, format->Gshift, format->Gloss);
			SetChannelValue(blue, out, format->Bmask, format->Bshift, format->Bloss);
			SetChannelValue(alpha, out, format->Amask, format->Ashift, format->Aloss);

			return out;
		}
	}


	static void AddMask(SDL_Surface** lhs, SDL_Surface* rhs, SDL_Rect src, SDL_Rect dst, int rhsX = 0, int rhsY = 0, bool overwriteMask = true, _RGB channelMask = R)
	{
		Uint32 bitmask{};
		Uint8 bitshift{}, bitloss{};
		switch (channelMask)
		{
		case SurfaceMath::R:
			bitmask = rhs->format->Rmask;
			bitshift = rhs->format->Rshift;
			bitloss = rhs->format->Rloss;
			break;
		case SurfaceMath::G:
			bitmask = rhs->format->Gmask;
			bitshift = rhs->format->Gshift;
			bitloss = rhs->format->Gloss;
			break;
		case SurfaceMath::B:
			bitmask = rhs->format->Bmask;
			bitshift = rhs->format->Bshift;
			bitloss = rhs->format->Bloss;
			break;
		case SurfaceMath::A:
			bitmask = rhs->format->Amask;
			bitshift = rhs->format->Ashift;
			bitloss = rhs->format->Aloss;
			break;
		}

		if ((*lhs)->format->Amask == 0)
		{
			SDL_Surface* temp = SDL_CreateRGBSurfaceWithFormat(0, (*lhs)->w, (*lhs)->h, 32, SDL_PIXELFORMAT_RGBA32);
			SDL_BlitSurface(*lhs, NULL, temp, NULL);
			SDL_FreeSurface(*lhs);
			*lhs = temp;
		}

		int xdiff = src.w - src.x - rhsX;
		int ydiff = src.h - src.y - rhsY;

		//mask position
		//x,y is for rhs, _x and _y is for lhs
		for (int y = dst.y; y < ydiff && y < dst.h; y++)
		{
			for (int x = dst.x; x < xdiff && x < dst.w; x++)
			{
				int _x = x - dst.x + src.x + rhsX;
				int _y = y - dst.y + src.y + rhsY;

				if (_x >= 0 && _y >= 0)
				{
					Uint32 InputPixel = internal::GetPixel((*lhs), _x, _y);
					Uint32 MaskPixel = internal::GetPixel(rhs, x, y);

					if (overwriteMask)
					{
						MaskPixel = internal::GetChannelValue(MaskPixel, bitmask, bitshift, bitloss);
					}
					else
					{
						MaskPixel = _min(
							internal::GetChannelValue(MaskPixel, bitmask, bitshift, bitloss) +
							internal::GetChannelValue(InputPixel, (*lhs)->format->Amask, (*lhs)->format->Ashift, (*lhs)->format->Aloss)
							, 255);
					}


					internal::SetChannelValue(MaskPixel, InputPixel, (*lhs)->format->Amask, (*lhs)->format->Ashift, (*lhs)->format->Aloss);

					internal::SetPixel((*lhs), InputPixel, _x, _y);
				}
			}
		}
	}

	static void Substract(SDL_Surface* lhs, SDL_Surface* rhs, SDL_Rect src, SDL_Rect dst, int rhsX = 0, int rhsY = 0)
	{
		//SDL_Surface* out = SDL_CreateRGBSurfaceWithFormat(0, lhs->w, lhs->h, 32, lhs->format->format);
		//SDL_BlitSurface(lhs, NULL, out, NULL);

		//if (src.w == INT_MAX)
		//	src.w = lhs->w;
		//if (src.h == INT_MAX)
		//	src.h = lhs->h;
		//if (dst.w == INT_MAX)
		//	dst.w = rhs->w;
		//if (dst.h == INT_MAX)
		//	dst.h = rhs->h;


		int xdiff = src.w - src.x - rhsX;
		int ydiff = src.h - src.y - rhsY;

		int newwidth = dst.w + dst.x;
		int newheight = dst.h + dst.y;

		//mask position
		//x,y is for rhs, _x and _y is for lhs
		for (int y = dst.y; y < ydiff && y < newheight; y++)
		{
			for (int x = dst.x; x < xdiff && x < newwidth; x++)
			{
				int _x = x - dst.x + src.x + rhsX;
				int _y = y - dst.y + src.y + rhsY;

				if (_x >= 0 && _y >= 0)
				{
					Uint32 pixelL = internal::GetPixel(lhs, _x, _y);
					Uint32 pixelR = internal::GetPixel(rhs, x, y);

					Uint32 substracted = internal::SubstractPixel(pixelL, lhs->format, pixelR, rhs->format);

					internal::SetPixel(lhs, substracted, _x, _y);
				}		
			}
		}
	}

	static void Add(SDL_Surface* lhs, SDL_Surface* rhs, int x = 0, int y = 0, int width = INT_MAX, int height = INT_MAX)
	{
		if (width == INT_MAX)
			width = rhs->w;
		if (height == INT_MAX)
			height = rhs->h;


		int xdiff = lhs->w - x;
		int ydiff = lhs->h - y;

		//mask position
		for (int _y = 0; _y < ydiff && _y < height; _y++)
		{
			for (int _x = 0; _x < xdiff && _x < width; _x++)
			{
				if (_x + x >= 0 && _y + y >= 0)
				{
					Uint32 pixelL = internal::GetPixel(lhs, x, y);
					Uint32 pixelR = internal::GetPixel(rhs, x, y);

					auto added = internal::AddPixel(pixelL, lhs->format, pixelR, rhs->format);

					internal::SetPixel(lhs, added, x, y);
				}
			}
		}
	}

	static void Colorize(SDL_Surface* texture, SDL_Color* color, _RGB channelToTakeAsGreyscale = R, int x = 0, int y = 0, int width = INT_MAX, int height = INT_MAX)
	{
		int xdiff = texture->w - x;
		int ydiff = texture->h - y;

		//mask position
		for (int _y = 0; _y < ydiff && _y < height; _y++)
		{
			for (int _x = 0; _x < xdiff && _x < width; _x++)
			{
				if (_x + x >= 0 && _y + y >= 0)
				{
					Uint32 pixel = internal::GetPixel(texture, _x + x, _y + y);

					pixel = internal::Colorize(pixel, texture->format, color, channelToTakeAsGreyscale);

					internal::SetPixel(texture, pixel, _x + x, _y + y);
				}
			}
		}
	}

	static SDL_Surface* Scale(SDL_Surface* texture, int newWidth, int newHeight)
	{
		SDL_Surface* out;
		out = SDL_CreateRGBSurfaceWithFormat(0, newWidth, newHeight, 32, texture->format->format);

			float x_ratio = (float)texture->w / (float)newWidth;
			float y_ratio = (float)texture->h / (float)newHeight;
			int _x, _y;

			for (int y = 0; y < newHeight; y++) 
			{
				for (int x = 0; x < newWidth; x++) 
				{

					_x = (int)floor(x*x_ratio);
					_y = (int)floor(y*y_ratio);

					memcpy((Uint8*)out->pixels + ((y*newWidth) + x), 
						(Uint8*)texture->pixels + ((_y*texture->w) + _x), 
						out->format->BytesPerPixel);
				}
			}
			return out;
	}

	static void Overlay(SDL_Surface* lhs, SDL_Surface* rhs, int x = 0, int y = 0, int width = INT_MAX, int height = INT_MAX)
	{
		if (width == INT_MAX)
			width = rhs->w;
		if (height == INT_MAX)
			height = rhs->h;

		int xdiff = lhs->w - x;
		int ydiff = lhs->h - y;

		//mask position
		for (int _y = 0; _y < ydiff && _y < height; _y++)
		{
			for (int _x = 0; _x < xdiff && _x < width; _x++)
			{
				if (_x + x >= 0 && _y + y >= 0)
				{
					Uint32 pixelR = internal::GetPixel(rhs, _x, _y);

					internal::SetPixel(lhs, pixelR, _x + x, _y + y);
				}
			}
		}
	}

	static SDL_Surface* Crop(SDL_Surface* lhs, int x, int y, int width, int height)
	{
		SDL_Surface* out;
		out = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, lhs->format->format);

		int xdiff = lhs->w - x;
		int ydiff = lhs->h - y;

		for (int _y = 0; _y < ydiff && _y < height; _y++)
		{
			for (int _x = 0; _x < xdiff && _x < width; _x++)
			{
				if (_x + x >= 0 && _y + y >= 0)
				{
					Uint32 pixel = internal::GetPixel(lhs, _x + x, _y + y);

					internal::SetPixel(out, pixel, _x, _y);
				}
			}
		}

		return out;
	}

}