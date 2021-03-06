#include <stdio.h>
#include <stdarg.h>
#include "Graphics.h"

#define FMT_TO_STR_MAXLEN 1024

const int ScreenBufferSize = sizeof(int) * SCREEN_W * SCREEN_H;

Graphics::Graphics(int bgcolor, bool fullscreen)
{
	Init(fullscreen);
	ClearCharset();
	LoadCharset(CHARSET_FILE);
	Clear(bgcolor);
}

Graphics::~Graphics()
{
	Dispose();
}

void Graphics::Init(bool fullscreen)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d");
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	Window = SDL_CreateWindow("Lightbringer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		WINDOW_W, WINDOW_H, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

	Renderer = SDL_CreateRenderer(Window, -1, 
		SDL_RENDERER_PRESENTVSYNC || SDL_RENDERER_ACCELERATED || SDL_RENDERER_TARGETTEXTURE);
	
	SDL_RenderSetLogicalSize(Renderer, SCREEN_W, SCREEN_H);

	ScreenTexture = SDL_CreateTexture(Renderer,
		SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_W, SCREEN_H);
}

void Graphics::Dispose()
{
	SDL_DestroyTexture(ScreenTexture);
	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
	SDL_Quit();
}

void Graphics::ClearCharset()
{
	for (unsigned i = 0; i < CHARSET_SIZE; i++)
		SetChar(i, 0, 0, 0, 0, 0, 0, 0, 0);
}

void Graphics::ToggleFullscreen()
{
	Uint32 fullscreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;
	Uint32 isFullscreen = SDL_GetWindowFlags(Window) & fullscreenFlag;
	SDL_SetWindowFullscreen(Window, isFullscreen ? 0 : fullscreenFlag);
	SDL_ShowCursor(isFullscreen);
}

void Graphics::Update()
{
	static int pitch;
	static void* pixels;

	SDL_LockTexture(ScreenTexture, NULL, &pixels, &pitch);
	SDL_memcpy(pixels, Buffer, ScreenBufferSize);
	SDL_UnlockTexture(ScreenTexture);
	SDL_RenderCopy(Renderer, ScreenTexture, NULL, NULL);
	SDL_RenderPresent(Renderer);
}

void Graphics::Clear(int color)
{
	for (int y = 0; y < SCREEN_H; y++)
		for (int x = 0; x < SCREEN_W; x++)
			SetPixel(x, y, color);
}

void Graphics::SetPixel(int x, int y, int color)
{
	if (x >= 0 && y >= 0 && x < SCREEN_W && y < SCREEN_H)
		Buffer[y][x] = color;
}

void Graphics::SetChar(int chr,
	int row1, int row2, int row3, int row4,
	int row5, int row6, int row7, int row8)
{
	byte* pixels = Charset[chr];

	pixels[0] = row1;
	pixels[1] = row2;
	pixels[2] = row3;
	pixels[3] = row4;
	pixels[4] = row5;
	pixels[5] = row6;
	pixels[6] = row7;
	pixels[7] = row8;
}

void Graphics::PutChar(int chr, int x, int y, int forecolor, int backcolor)
{
	DrawChar(chr, x * CHAR_W, y * CHAR_H, forecolor, backcolor);
}

void Graphics::DrawChar(int chr, int x, int y, int forecolor, int backcolor)
{
	byte* pixels = Charset[chr];

	const int initialX = x;

	for (int i = 0; i < CHAR_H; i++)
	{
		const unsigned int& bits = pixels[i];

		for (int pos = CHAR_W - 1; pos >= 0; pos--, x++)
			SetPixel(x, y, (bits & (1 << pos)) ? forecolor : backcolor);

		y++;
		x = initialX;
	}
}

void Graphics::Print(int x, int y, int forecolor, int backcolor, const char* fmt, ...)
{
	char str[FMT_TO_STR_MAXLEN] = { 0 };
	va_list arg;
	va_start(arg, fmt);
	vsprintf_s(str, fmt, arg);
	va_end(arg);
	
	int i = 0;

	while (true)
	{
		int chr = str[i++];
		if (chr == '\0')
			break;

		PutChar(chr, x++, y, forecolor, backcolor);
	}
}

void Graphics::SaveCharset(const char* filename)
{
	FILE* fp = fopen(filename, "wb");

	fwrite(Charset, sizeof(Charset), 1, fp);
	fflush(fp);
	fclose(fp);
}

void Graphics::LoadCharset(const char* filename)
{
	FILE* fp = fopen(filename, "rb");

	fread(Charset, sizeof(Charset), 1, fp);
	fclose(fp);
}

void Graphics::SetupDefaultCharset()
{
	int i = 0;

	SetChar(i++, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	SetChar(i++, 0x7e, 0x81, 0xa5, 0x81, 0xbd, 0x99, 0x81, 0x7e);
	SetChar(i++, 0x7e, 0xff, 0xdb, 0xff, 0xc3, 0xe7, 0xff, 0x7e);
	SetChar(i++, 0x6c, 0xfe, 0xfe, 0xfe, 0x7c, 0x38, 0x10, 0x00);
	SetChar(i++, 0x10, 0x38, 0x7c, 0xfe, 0x7c, 0x38, 0x10, 0x00);
	SetChar(i++, 0x38, 0x7c, 0x38, 0xfe, 0xfe, 0xd6, 0x10, 0x38);
	SetChar(i++, 0x10, 0x10, 0x38, 0x7c, 0xfe, 0x7c, 0x10, 0x38);
	SetChar(i++, 0x00, 0x00, 0x18, 0x3c, 0x3c, 0x18, 0x00, 0x00);
	SetChar(i++, 0xff, 0xff, 0xe7, 0xc3, 0xc3, 0xe7, 0xff, 0xff);
	SetChar(i++, 0x00, 0x3c, 0x66, 0x42, 0x42, 0x66, 0x3c, 0x00);
	SetChar(i++, 0xff, 0xc3, 0x99, 0xbd, 0xbd, 0x99, 0xc3, 0xff);
	SetChar(i++, 0x0f, 0x07, 0x0f, 0x7d, 0xcc, 0xcc, 0xcc, 0x78);
	SetChar(i++, 0x3c, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x7e, 0x18);
	SetChar(i++, 0x3f, 0x33, 0x3f, 0x30, 0x30, 0x30, 0xf0, 0xf0);
	SetChar(i++, 0x7f, 0x63, 0x7f, 0x63, 0x63, 0x67, 0xe7, 0xe0);
	SetChar(i++, 0x99, 0x5a, 0x3c, 0xe7, 0xe7, 0x3c, 0x5a, 0x99);
	SetChar(i++, 0x80, 0xe0, 0xf8, 0xfe, 0xf8, 0xe0, 0x80, 0x00);
	SetChar(i++, 0x01, 0x07, 0x1f, 0x7f, 0x1f, 0x07, 0x01, 0x00);
	SetChar(i++, 0x18, 0x3c, 0x7e, 0x18, 0x18, 0x7e, 0x3c, 0x18);
	SetChar(i++, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x66, 0x00);
	SetChar(i++, 0x7f, 0xdb, 0xdb, 0x7b, 0x1b, 0x1b, 0x1b, 0x00);
	SetChar(i++, 0x7e, 0xc3, 0x78, 0xcc, 0xcc, 0x78, 0x8c, 0xf8);
	SetChar(i++, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x7e, 0x00);
	SetChar(i++, 0x18, 0x3c, 0x7e, 0x18, 0x7e, 0x3c, 0x18, 0xff);
	SetChar(i++, 0x18, 0x3c, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x00);
	SetChar(i++, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x3c, 0x18, 0x00);
	SetChar(i++, 0x00, 0x18, 0x0c, 0xfe, 0x0c, 0x18, 0x00, 0x00);
	SetChar(i++, 0x00, 0x30, 0x60, 0xfe, 0x60, 0x30, 0x00, 0x00);
	SetChar(i++, 0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xfe, 0x00, 0x00);
	SetChar(i++, 0x00, 0x24, 0x66, 0xff, 0x66, 0x24, 0x00, 0x00);
	SetChar(i++, 0x00, 0x18, 0x3c, 0x7e, 0xff, 0xff, 0x00, 0x00);
	SetChar(i++, 0x00, 0xff, 0xff, 0x7e, 0x3c, 0x18, 0x00, 0x00);
	SetChar(i++, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	SetChar(i++, 0x18, 0x3c, 0x3c, 0x18, 0x18, 0x00, 0x18, 0x00);
	SetChar(i++, 0x6c, 0x6c, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00);
	SetChar(i++, 0x6c, 0x6c, 0xfe, 0x6c, 0xfe, 0x6c, 0x6c, 0x00);
	SetChar(i++, 0x18, 0x3e, 0x60, 0x3c, 0x06, 0x7c, 0x18, 0x00);
	SetChar(i++, 0x00, 0xc6, 0xcc, 0x18, 0x30, 0x66, 0xc6, 0x00);
	SetChar(i++, 0x38, 0x6c, 0x38, 0x76, 0xdc, 0xcc, 0x76, 0x00);
	SetChar(i++, 0x70, 0x30, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00);
	SetChar(i++, 0x18, 0x30, 0x60, 0x60, 0x60, 0x30, 0x18, 0x00);
	SetChar(i++, 0x60, 0x30, 0x18, 0x18, 0x18, 0x30, 0x60, 0x00);
	SetChar(i++, 0x00, 0x66, 0x3c, 0xff, 0x3c, 0x66, 0x00, 0x00);
	SetChar(i++, 0x00, 0x30, 0x30, 0xfc, 0x30, 0x30, 0x00, 0x00);
	SetChar(i++, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x30, 0x60);
	SetChar(i++, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00);
	SetChar(i++, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00);
	SetChar(i++, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x80, 0x00);
	SetChar(i++, 0x78, 0xcc, 0xdc, 0xfc, 0xec, 0xcc, 0x78, 0x00);
	SetChar(i++, 0x30, 0xf0, 0x30, 0x30, 0x30, 0x30, 0xfc, 0x00);
	SetChar(i++, 0x78, 0xcc, 0x0c, 0x38, 0x60, 0xcc, 0xfc, 0x00);
	SetChar(i++, 0x78, 0xcc, 0x0c, 0x78, 0x0c, 0xcc, 0x78, 0x00);
	SetChar(i++, 0x1c, 0x3c, 0x6c, 0xcc, 0xfe, 0x0c, 0x0c, 0x00);
	SetChar(i++, 0xfc, 0xc0, 0xf8, 0x0c, 0x0c, 0xcc, 0x78, 0x00);
	SetChar(i++, 0x38, 0x60, 0xc0, 0xf8, 0xcc, 0xcc, 0x78, 0x00);
	SetChar(i++, 0xfc, 0xcc, 0x0c, 0x18, 0x30, 0x60, 0x60, 0x00);
	SetChar(i++, 0x78, 0xcc, 0xcc, 0x78, 0xcc, 0xcc, 0x78, 0x00);
	SetChar(i++, 0x78, 0xcc, 0xcc, 0x7c, 0x0c, 0x18, 0x70, 0x00);
	SetChar(i++, 0x00, 0x00, 0x30, 0x30, 0x00, 0x30, 0x30, 0x00);
	SetChar(i++, 0x00, 0x00, 0x30, 0x30, 0x00, 0x70, 0x30, 0x60);
	SetChar(i++, 0x0c, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0c, 0x00);
	SetChar(i++, 0x00, 0x00, 0x7e, 0x00, 0x7e, 0x00, 0x00, 0x00);
	SetChar(i++, 0x60, 0x30, 0x18, 0x0c, 0x18, 0x30, 0x60, 0x00);
	SetChar(i++, 0x78, 0xcc, 0x0c, 0x18, 0x30, 0x00, 0x30, 0x00);
	SetChar(i++, 0x7c, 0xc6, 0xde, 0xd6, 0xde, 0xc0, 0x78, 0x00);
	SetChar(i++, 0x18, 0x3c, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x00);
	SetChar(i++, 0xfc, 0x66, 0x66, 0x7c, 0x66, 0x66, 0xfc, 0x00);
	SetChar(i++, 0x3c, 0x66, 0xc0, 0xc0, 0xc0, 0x66, 0x3c, 0x00);
	SetChar(i++, 0xf8, 0x6c, 0x66, 0x66, 0x66, 0x6c, 0xf8, 0x00);
	SetChar(i++, 0xfe, 0x62, 0x68, 0x78, 0x68, 0x62, 0xfe, 0x00);
	SetChar(i++, 0xfe, 0x62, 0x68, 0x78, 0x68, 0x60, 0xf0, 0x00);
	SetChar(i++, 0x3c, 0x66, 0xc0, 0xc0, 0xce, 0x66, 0x3e, 0x00);
	SetChar(i++, 0x66, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x00);
	SetChar(i++, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00);
	SetChar(i++, 0x1e, 0x0c, 0x0c, 0x0c, 0xcc, 0xcc, 0x78, 0x00);
	SetChar(i++, 0xe6, 0x66, 0x6c, 0x78, 0x6c, 0x66, 0xe6, 0x00);
	SetChar(i++, 0xf0, 0x60, 0x60, 0x60, 0x62, 0x66, 0xfe, 0x00);
	SetChar(i++, 0xc6, 0xee, 0xfe, 0xd6, 0xc6, 0xc6, 0xc6, 0x00);
	SetChar(i++, 0xc6, 0xe6, 0xf6, 0xde, 0xce, 0xc6, 0xc6, 0x00);
	SetChar(i++, 0x38, 0x6c, 0xc6, 0xc6, 0xc6, 0x6c, 0x38, 0x00);
	SetChar(i++, 0xfc, 0x66, 0x66, 0x7c, 0x60, 0x60, 0xf0, 0x00);
	SetChar(i++, 0x38, 0x6c, 0xc6, 0xc6, 0xce, 0x6c, 0x3e, 0x00);
	SetChar(i++, 0xfc, 0x66, 0x66, 0x7c, 0x78, 0x6c, 0xe6, 0x00);
	SetChar(i++, 0x7c, 0xc6, 0xc0, 0x7c, 0x06, 0xc6, 0x7c, 0x00);
	SetChar(i++, 0xfe, 0xba, 0x38, 0x38, 0x38, 0x38, 0x7c, 0x00);
	SetChar(i++, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00);
	SetChar(i++, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x6c, 0x38, 0x00);
	SetChar(i++, 0xc6, 0xc6, 0xc6, 0xd6, 0xfe, 0xee, 0xc6, 0x00);
	SetChar(i++, 0xc6, 0xc6, 0x6c, 0x38, 0x6c, 0xc6, 0xc6, 0x00);
	SetChar(i++, 0xc6, 0xc6, 0xc6, 0x7c, 0x38, 0x38, 0x7c, 0x00);
	SetChar(i++, 0xfe, 0xcc, 0x98, 0x30, 0x62, 0xc6, 0xfe, 0x00);
	SetChar(i++, 0x78, 0x60, 0x60, 0x60, 0x60, 0x60, 0x78, 0x00);
	SetChar(i++, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x02, 0x00);
	SetChar(i++, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x00);
	SetChar(i++, 0x10, 0x38, 0x6c, 0xc6, 0x00, 0x00, 0x00, 0x00);
	SetChar(i++, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff);
	SetChar(i++, 0x38, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00);
	SetChar(i++, 0x00, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x7e, 0x00);
	SetChar(i++, 0xe0, 0x60, 0x60, 0x7c, 0x66, 0x66, 0xfc, 0x00);
	SetChar(i++, 0x00, 0x00, 0x7c, 0xc6, 0xc0, 0xc6, 0x7c, 0x00);
	SetChar(i++, 0x1c, 0x0c, 0x0c, 0x7c, 0xcc, 0xcc, 0x7e, 0x00);
	SetChar(i++, 0x00, 0x00, 0x7c, 0xc6, 0xfe, 0xc0, 0x7c, 0x00);
	SetChar(i++, 0x3c, 0x66, 0x60, 0xf8, 0x60, 0x60, 0xf8, 0x00);
	SetChar(i++, 0x00, 0x00, 0x7e, 0xcc, 0xcc, 0x7c, 0x0c, 0xf8);
	SetChar(i++, 0xe0, 0x60, 0x60, 0x7c, 0x66, 0x66, 0xe6, 0x00);
	SetChar(i++, 0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3c, 0x00);
	SetChar(i++, 0x0c, 0x00, 0x1e, 0x06, 0x06, 0x66, 0x3c, 0x00);
	SetChar(i++, 0xe0, 0x60, 0x66, 0x6c, 0x78, 0x6c, 0xe6, 0x00);
	SetChar(i++, 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00);
	SetChar(i++, 0x00, 0x00, 0xec, 0xfe, 0xda, 0xda, 0xda, 0x00);
	SetChar(i++, 0x00, 0x00, 0xfc, 0x66, 0x66, 0x66, 0x66, 0x00);
	SetChar(i++, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x00);
	SetChar(i++, 0x00, 0x00, 0xfc, 0x66, 0x66, 0x7c, 0x60, 0xf0);
	SetChar(i++, 0x00, 0x00, 0x7e, 0xcc, 0xcc, 0x7c, 0x0c, 0x1e);
	SetChar(i++, 0x00, 0x00, 0xfc, 0x66, 0x66, 0x60, 0xf0, 0x00);
	SetChar(i++, 0x00, 0x00, 0x7e, 0xc0, 0x7c, 0x06, 0xfc, 0x00);
	SetChar(i++, 0x20, 0x60, 0xfc, 0x60, 0x60, 0x66, 0x3c, 0x00);
	SetChar(i++, 0x00, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x7e, 0x00);
	SetChar(i++, 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0x7c, 0x38, 0x00);
	SetChar(i++, 0x00, 0x00, 0xda, 0xda, 0xda, 0xda, 0x74, 0x00);
	SetChar(i++, 0x00, 0x00, 0xc6, 0x6c, 0x38, 0x6c, 0xc6, 0x00);
	SetChar(i++, 0x00, 0x00, 0xc6, 0xc6, 0x7e, 0x06, 0xfc, 0x00);
	SetChar(i++, 0x00, 0x00, 0xfe, 0x18, 0x30, 0x62, 0xfe, 0x00);
	SetChar(i++, 0x0e, 0x18, 0x18, 0x70, 0x18, 0x18, 0x0e, 0x00);
	SetChar(i++, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00);
	SetChar(i++, 0x70, 0x18, 0x18, 0x0e, 0x18, 0x18, 0x70, 0x00);
	SetChar(i++, 0x76, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	SetChar(i++, 0x10, 0x38, 0x6c, 0xc6, 0xc6, 0xc6, 0xfe, 0x00);
	SetChar(i++, 0x7c, 0xc6, 0xc0, 0xc0, 0xc6, 0x7c, 0x18, 0x30);
	SetChar(i++, 0x00, 0xcc, 0x00, 0xcc, 0xcc, 0xcc, 0x7e, 0x00);
	SetChar(i++, 0x18, 0x30, 0x7c, 0xc6, 0xfe, 0xc0, 0x7c, 0x00);
	SetChar(i++, 0x30, 0x48, 0x78, 0x0c, 0x7c, 0xcc, 0x7e, 0x00);
	SetChar(i++, 0xcc, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x7e, 0x00);
	SetChar(i++, 0x60, 0x30, 0x78, 0x0c, 0xfc, 0xcc, 0x7e, 0x00);
	SetChar(i++, 0x30, 0x48, 0x78, 0x0c, 0xfc, 0xcc, 0x7e, 0x00);
	SetChar(i++, 0x00, 0x7c, 0xc6, 0xc0, 0xc6, 0x7c, 0x18, 0x30);
	SetChar(i++, 0x30, 0x48, 0x7c, 0xc6, 0xfe, 0xc0, 0x7c, 0x00);
	SetChar(i++, 0xcc, 0x00, 0x7c, 0xc6, 0xfe, 0xc0, 0x7c, 0x00);
	SetChar(i++, 0x60, 0x30, 0x7c, 0xc6, 0xfe, 0xc0, 0x7c, 0x00);
	SetChar(i++, 0x6c, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3c, 0x00);
	SetChar(i++, 0x38, 0x44, 0x38, 0x18, 0x18, 0x18, 0x3c, 0x00);
	SetChar(i++, 0x30, 0x18, 0x38, 0x18, 0x18, 0x18, 0x3c, 0x00);
	SetChar(i++, 0xcc, 0x30, 0x78, 0xcc, 0xcc, 0xfc, 0xcc, 0x00);
	SetChar(i++, 0x30, 0x48, 0x78, 0xcc, 0xcc, 0xfc, 0xcc, 0x00);
	SetChar(i++, 0x18, 0x30, 0xfc, 0x60, 0x78, 0x60, 0xfc, 0x00);
	SetChar(i++, 0x00, 0x00, 0x7e, 0x09, 0x7f, 0xc8, 0x7e, 0x00);
	SetChar(i++, 0x3f, 0x6c, 0xcc, 0xff, 0xcc, 0xcc, 0xcf, 0x00);
	SetChar(i++, 0x00, 0x38, 0x44, 0x7c, 0xc6, 0xc6, 0x7c, 0x00);
	SetChar(i++, 0x00, 0xc6, 0x00, 0x7c, 0xc6, 0xc6, 0x7c, 0x00);
	SetChar(i++, 0x60, 0x30, 0x00, 0x7c, 0xc6, 0xc6, 0x7c, 0x00);
	SetChar(i++, 0x30, 0x48, 0x00, 0xcc, 0xcc, 0xcc, 0x7e, 0x00);
	SetChar(i++, 0x60, 0x30, 0x00, 0xcc, 0xcc, 0xcc, 0x7e, 0x00);
	SetChar(i++, 0xcc, 0x00, 0xc6, 0xc6, 0x7e, 0x06, 0x7c, 0x00);
	SetChar(i++, 0xc6, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x00);
	SetChar(i++, 0xcc, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x7e, 0x00);
	SetChar(i++, 0x00, 0x00, 0x7c, 0xce, 0xd6, 0xe6, 0x7c, 0x00);
	SetChar(i++, 0x38, 0x6c, 0x64, 0xf0, 0x60, 0x66, 0xfc, 0x00);
	SetChar(i++, 0x3a, 0x6c, 0xce, 0xd6, 0xe6, 0x6c, 0xb8, 0x00);
	SetChar(i++, 0x00, 0x00, 0xc6, 0x7c, 0x38, 0x7c, 0xc6, 0x00);
	SetChar(i++, 0x0e, 0x1b, 0x18, 0x7e, 0x18, 0xd8, 0x70, 0x00);
	SetChar(i++, 0x18, 0x30, 0x78, 0x0c, 0x7c, 0xcc, 0x7e, 0x00);
	SetChar(i++, 0x0c, 0x18, 0x38, 0x18, 0x18, 0x18, 0x3c, 0x00);
	SetChar(i++, 0x0c, 0x18, 0x00, 0x7c, 0xc6, 0xc6, 0x7c, 0x00);
	SetChar(i++, 0x18, 0x30, 0x00, 0xcc, 0xcc, 0xcc, 0x7e, 0x00);
	SetChar(i++, 0x76, 0xdc, 0x00, 0xfc, 0x66, 0x66, 0x66, 0x00);
	SetChar(i++, 0x76, 0xdc, 0x00, 0xe6, 0xf6, 0xde, 0xce, 0x00);
	SetChar(i++, 0x70, 0xd0, 0x78, 0x00, 0xf8, 0x00, 0x00, 0x00);
	SetChar(i++, 0x70, 0xd8, 0x70, 0x00, 0xf8, 0x00, 0x00, 0x00);
	SetChar(i++, 0x30, 0x00, 0x30, 0x60, 0xc0, 0xcc, 0x78, 0x00);
	SetChar(i++, 0x3c, 0x42, 0xb9, 0xa5, 0xb9, 0xa5, 0x42, 0x3c);
	SetChar(i++, 0x00, 0x00, 0x00, 0xfe, 0x06, 0x06, 0x00, 0x00);
	SetChar(i++, 0xc6, 0x4c, 0x58, 0x36, 0x69, 0xc2, 0x84, 0x0f);
	SetChar(i++, 0xc6, 0x4c, 0x58, 0x36, 0x6a, 0xdf, 0x82, 0x02);
	SetChar(i++, 0x18, 0x00, 0x18, 0x18, 0x3c, 0x3c, 0x18, 0x00);
	SetChar(i++, 0x00, 0x33, 0x66, 0xcc, 0x66, 0x33, 0x00, 0x00);
	SetChar(i++, 0x00, 0xcc, 0x66, 0x33, 0x66, 0xcc, 0x00, 0x00);
	SetChar(i++, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88);
	SetChar(i++, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55);
	SetChar(i++, 0xdd, 0x77, 0xdd, 0x77, 0xdd, 0x77, 0xdd, 0x77);
	SetChar(i++, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18);
	SetChar(i++, 0x18, 0x18, 0x18, 0xf8, 0xf8, 0x18, 0x18, 0x18);
	SetChar(i++, 0x18, 0x30, 0x7c, 0xc6, 0xc6, 0xfe, 0xc6, 0x00);
	SetChar(i++, 0x38, 0x44, 0x7c, 0xc6, 0xc6, 0xfe, 0xc6, 0x00);
	SetChar(i++, 0x60, 0x30, 0x7c, 0xc6, 0xc6, 0xfe, 0xc6, 0x00);
	SetChar(i++, 0x3c, 0x42, 0x99, 0xa1, 0xa1, 0x99, 0x42, 0x3c);
	SetChar(i++, 0x24, 0x24, 0xe4, 0x04, 0x04, 0xe4, 0x24, 0x24);
	SetChar(i++, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24);
	SetChar(i++, 0x00, 0x00, 0xfc, 0x04, 0x04, 0xe4, 0x24, 0x24);
	SetChar(i++, 0x24, 0x24, 0xe4, 0x04, 0x04, 0xfc, 0x00, 0x00);
	SetChar(i++, 0x18, 0x18, 0x7e, 0xc0, 0xc0, 0x7e, 0x18, 0x18);
	SetChar(i++, 0x66, 0x3c, 0x18, 0x7e, 0x18, 0x7e, 0x18, 0x18);
	SetChar(i++, 0x00, 0x00, 0x00, 0xf0, 0xf8, 0x38, 0x18, 0x18);
	SetChar(i++, 0x18, 0x18, 0x1c, 0x1f, 0x0f, 0x00, 0x00, 0x00);
	SetChar(i++, 0x18, 0x18, 0x18, 0xff, 0xff, 0x00, 0x00, 0x00);
	SetChar(i++, 0x00, 0x00, 0x00, 0xff, 0xff, 0x18, 0x18, 0x18);
	SetChar(i++, 0x18, 0x18, 0x18, 0x1f, 0x1f, 0x18, 0x18, 0x18);
	SetChar(i++, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00);
	SetChar(i++, 0x18, 0x18, 0x18, 0xff, 0xff, 0x18, 0x18, 0x18);
	SetChar(i++, 0x76, 0xdc, 0x78, 0x0c, 0x7c, 0xcc, 0x7e, 0x00);
	SetChar(i++, 0x76, 0xdc, 0x7c, 0xc6, 0xc6, 0xfe, 0xc6, 0x00);
	SetChar(i++, 0x24, 0x24, 0x27, 0x20, 0x20, 0x3f, 0x00, 0x00);
	SetChar(i++, 0x00, 0x00, 0x3f, 0x20, 0x20, 0x27, 0x24, 0x24);
	SetChar(i++, 0x24, 0x24, 0xe7, 0x00, 0x00, 0xff, 0x00, 0x00);
	SetChar(i++, 0x00, 0x00, 0xff, 0x00, 0x00, 0xe7, 0x24, 0x24);
	SetChar(i++, 0x24, 0x24, 0x27, 0x20, 0x20, 0x27, 0x24, 0x24);
	SetChar(i++, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00);
	SetChar(i++, 0x24, 0x24, 0xe7, 0x00, 0x00, 0xe7, 0x24, 0x24);
	SetChar(i++, 0x00, 0x82, 0xfe, 0x6c, 0xfe, 0x82, 0x00, 0x00);
	SetChar(i++, 0x90, 0x60, 0x98, 0x0c, 0x7c, 0xcc, 0x7e, 0x00);
	SetChar(i++, 0xf8, 0x6c, 0x66, 0xf6, 0x66, 0x6c, 0xf8, 0x00);
	SetChar(i++, 0x38, 0x44, 0xfe, 0x60, 0x7c, 0x60, 0xfe, 0x00);
	SetChar(i++, 0xc6, 0x00, 0xfe, 0x60, 0x7c, 0x60, 0xfe, 0x00);
	SetChar(i++, 0x30, 0x18, 0xfe, 0x60, 0x7c, 0x60, 0xfe, 0x00);
	SetChar(i++, 0x60, 0x20, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00);
	SetChar(i++, 0x0c, 0x18, 0x3c, 0x18, 0x18, 0x18, 0x3c, 0x00);
	SetChar(i++, 0x18, 0x24, 0x3c, 0x18, 0x18, 0x18, 0x3c, 0x00);
	SetChar(i++, 0x6c, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3c, 0x00);
	SetChar(i++, 0x18, 0x18, 0x38, 0xf8, 0xf0, 0x00, 0x00, 0x00);
	SetChar(i++, 0x00, 0x00, 0x00, 0x0f, 0x1f, 0x1c, 0x18, 0x18);
	SetChar(i++, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
	SetChar(i++, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff);
	SetChar(i++, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00);
	SetChar(i++, 0x30, 0x18, 0x3c, 0x18, 0x18, 0x18, 0x3c, 0x00);
	SetChar(i++, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00);
	SetChar(i++, 0x18, 0x30, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x00);
	SetChar(i++, 0x7c, 0xc6, 0xc6, 0xfc, 0xc6, 0xc6, 0xdc, 0xc0);
	SetChar(i++, 0x38, 0x44, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x00);
	SetChar(i++, 0x60, 0x30, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x00);
	SetChar(i++, 0x76, 0xdc, 0x00, 0x7c, 0xc6, 0xc6, 0x7c, 0x00);
	SetChar(i++, 0x76, 0xdc, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x00);
	SetChar(i++, 0x00, 0x00, 0x66, 0x66, 0x66, 0x7c, 0x60, 0xc0);
	SetChar(i++, 0x00, 0x70, 0x3c, 0x36, 0x3c, 0x30, 0x78, 0x00);
	SetChar(i++, 0xf0, 0x60, 0x7c, 0x66, 0x7c, 0x60, 0xf0, 0x00);
	SetChar(i++, 0x18, 0x30, 0xcc, 0xcc, 0xcc, 0xcc, 0x7e, 0x00);
	SetChar(i++, 0x30, 0x48, 0x00, 0xcc, 0xcc, 0xcc, 0x78, 0x00);
	SetChar(i++, 0x60, 0x30, 0xcc, 0xcc, 0xcc, 0xcc, 0x7e, 0x00);
	SetChar(i++, 0x18, 0x30, 0x00, 0xc6, 0xc6, 0x7e, 0x06, 0x7c);
	SetChar(i++, 0x18, 0x30, 0xc6, 0xc6, 0x7c, 0x38, 0x7c, 0x00);
	SetChar(i++, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	SetChar(i++, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	SetChar(i++, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00);
	SetChar(i++, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x00, 0x7e, 0x00);
	SetChar(i++, 0x00, 0x00, 0xfe, 0x00, 0xfe, 0x00, 0x00, 0x00);
	SetChar(i++, 0xe6, 0x2c, 0xf8, 0x36, 0xea, 0xdf, 0x82, 0x02);
	SetChar(i++, 0x7f, 0xdb, 0xdb, 0x7b, 0x1b, 0x1b, 0x1b, 0x00);
	SetChar(i++, 0x7e, 0xc3, 0x78, 0xcc, 0xcc, 0x78, 0x8c, 0xf8);
	SetChar(i++, 0x30, 0x30, 0x00, 0xfc, 0x00, 0x30, 0x30, 0x00);
	SetChar(i++, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x60);
	SetChar(i++, 0x38, 0x6c, 0x6c, 0x38, 0x00, 0x00, 0x00, 0x00);
	SetChar(i++, 0x00, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	SetChar(i++, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00);
	SetChar(i++, 0x18, 0x78, 0x18, 0x18, 0x7e, 0x00, 0x00, 0x00);
	SetChar(i++, 0x78, 0x0c, 0x38, 0x0c, 0x78, 0x00, 0x00, 0x00);
	SetChar(i++, 0x78, 0x0c, 0x38, 0x60, 0x7c, 0x00, 0x00, 0x00);
	SetChar(i++, 0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00);
	SetChar(i++, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
}
