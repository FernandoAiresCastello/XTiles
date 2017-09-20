//=============================================================================
//
//		XTILES 0.3a
//		BASE STRUCTURES/FUNCTIONS
//
//		April 12th, 2012
//		Last updated: April 5th, 2015
//		By Fernando Aires Castello
//		http://xtiles.blogspot.com/
//
//=============================================================================

#pragma once

#include <SDL.h>

#define TILE_WIDTH		8		/**< How many pixels horizontally in a tile */
#define TILE_HEIGHT		8		/**< How many pixels vertically in a tile */
#define TILE_SIZE		64		/**< How many pixels in a tile */
#define TILESET_SIZE	3*256	/**< How many tiles in the tileset */
#define PALETTE_SIZE	256		/**< How many colors in the palette */
#define TILE_PIXEL_OFF	'0'
#define TILE_PIXEL_ON	'1'
#define TILE_PIXEL_TRANSPARENT	' '

/**
*	@brief The main namespace containing all functions used to create, manipulate and display tiles on the fly.
*/
namespace XTiles
{
	/**
	*	@brief A set of pixel data.
	*/
	struct Tile
	{
		/**
		*	Data for all pixels in the tile. This data is interpreted differently depending on the color mode.
		*/
		int pixel[TILE_SIZE];
	};

	/**
	*	@brief A set of tiles.
	*/
	struct Tileset
	{
		/**
		*	All tiles in this tileset.
		*/
		Tile tile[TILESET_SIZE];
	};

	/**
	*	@brief A set of colors.
	*/
	struct Palette
	{
		/**
		*	RGB color values.
		*/
		Uint32 color[PALETTE_SIZE];
	};

	/**
	*	\enum MulticolorMode
	*	Multicolor tile modes
	*/
	enum MulticolorMode
	{
		MULTICOLOR_INDEXED = 0, /**< The tile data is interpreted as indexed pixel colors */
		MULTICOLOR_DIRECT = 1,	/**< The tile data is interpreted as colors in the format 0xRRGGBB */
	};

	/**
	*	Tileset used by the engine. It can contain up to 512 tiles.
	*/
	extern Tileset tileset;
	/**
	*	Palette used by the engine. It can contain up to 255 colors.
	*/
	extern Palette palette;

	/**
	*	Get a color from the Palette.
	*	@param paletteIndex : The index (0-255) of the color in the Palette
	*	@return The color at that index
	*/
	Uint32& color( int paletteIndex );
	/**
	*	Get a tile from the Tileset.
	*	@param tilesetIndex : The index (0-511) of the tile in the Tileset
	*	@return The tile at that index
	*/
	Tile& tile( int tilesetIndex );
	/**
	*	Set a color in the Palette.
	*	@param paletteIndex : The index (0-255) of the color in the Palette
	*	@param rgb : The color (0xRRGGBB) to set at that index
	*/
	void setColor( int paletteIndex, Uint32 rgb );
	/**
	*	Get the red component from the specified color
	*	@param rgb : The color (0xRRGGBB) from which to extract the component
	*/
	int getColorR( Uint32 rgb );
	/**
	*	Get the green component from the specified color
	*	@param rgb : The color (0xRRGGBB) from which to extract the component
	*/
	int getColorG( Uint32 rgb );
	/**
	*	Get the blue component from the specified color
	*	@param rgb : The color (0xRRGGBB) from which to extract the component
	*/
	int getColorB( Uint32 rgb );
	/**
	*	Set the red, green and blue components in the specified color
	*	@param rgb : The color (0xRRGGBB) to add the components
	*	@param r : The red component value
	*	@param g : The green component value
	*	@param b : The blue component value
	*/
	void setColorRGB( Uint32& rgb, int r, int g, int b );
	/**
	*	Set the red, green and blue components in the specified color
	*	@param paletteIndex : The index of the color to add the components
	*	@param r : The red component value
	*	@param g : The green component value
	*	@param b : The blue component value
	*/
	void setColorRGB( int paletteIndex, int r, int g, int b );
	/**
	*	Set a tile in the Tileset.
	*	@param tilesetIndex : The index (0-511) of the tile in the Tileset
	*	@param tile : The tile to set at that index
	*/
	void setTile( int tilesetIndex, Tile tile );
	/**
	*	Set a tile as a copy from a tile in the Tileset.
	*	@param tile : The tile to set
	*	@param tilesetIndex : The index (0-511) of the tile in the Tileset
	*/
	void setTile( Tile& tile, int tilesetIndex );
	/**
	*	Create and set a bicolor tile in the tileset. The tile is created based on 8 binary strings each containing 8 characters that represent the color attribute of a pixel. Binary strings contain only characters '0' and '1'. Character '0' means "background color" and character '1' means "foreground color".
	*	@param tilesetIndex : The index (0-511) of the tile in the Tileset
	*	@param line1 : The binary string for the 1st row
	*	@param line2 : The binary string for the 2nd row
	*	@param line3 : The binary string for the 3rd row
	*	@param line4 : The binary string for the 4th row
	*	@param line5 : The binary string for the 5th row
	*	@param line6 : The binary string for the 6th row
	*	@param line7 : The binary string for the 7th row
	*	@param line8 : The binary string for the 8th row
	*/
	void setTile( int tilesetIndex,
		const char* line1, const char* line2, const char* line3, const char* line4, 
		const char* line5, const char* line6, const char* line7, const char* line8 );
	/** 
	*	Copy data from one tile into another
	*
	*	@param tile : The tile to receive the copied data
	*	@param copyTile : The tile from which to copy data
	*/
	void setTile( Tile& tile, Tile copyTile );
	/** 
	*	Copy data from one tile into another
	*
	*	@param tileIndex : The index of the tile to receive the copied data
	*	@param copyTileIndex : The index of the tile from which to copy data
	*/
	void setTile( int tileIndex, int copyTileIndex );
	/**
	*	Clear tile data (turn all pixels off)
	*	
	*	@param tile : The tile to clear
	*/
	void clearTile( Tile& tile );
	/**
	*	Clear tile data (turn all pixels off)
	*	
	*	@param tilesetIndex : The index of the tile to clear
	*/
	void clearTile( int tilesetIndex );
	/**
	*	Clear the entire tileset.
	*/
	void clearTileset();
	/**
	*	Invert tile data. Tiles that are on get turned off, tiles that are off get turned on. Transparent tiles are unaffected.
	*
	*	@param tile : The tile to invert data
	*/
	void invertTile( Tile& tile );
	/**
	*	Invert tile data. Tiles that are on get turned off, tiles that are off get turned on. Transparent tiles are unaffected.
	*
	*	@param tileIndex : The index of the tile to invert data
	*/
	void invertTile( int tileIndex );
	/**
	*	Clear the entire palette.
	*/
	void clearPalette();
	/**
	*	Generate a default tileset containing 256 ASCII characters based on IBM-ASCII. The remainning 256 tiles are blank. Any tile in the tileset can be customized by a call to setTile().
	*/
	void generateTileset();
	/**
	*	Generate a default palette containing 256 colors based on Atari 5200. Any color in the palette can be customized by a call to setColor().
	*/
	void generatePalette();
	/**
	*	Load the tileset from a file. This function assumes that the tileset to be loaded has been saved through a call to saveTileset().
	*	@param file : Path to the tileset file
	*/
	bool loadTileset( const char* file );
	/**
	*	Load the palette from a file. This function assumes that the palette to be loaded has been saved through a call to savePalette().
	*	@param file : Path to the palette file
	*/
	bool loadPalette( const char* file );
	/**
	*	Save the tileset to a file.
	*	@param file : Path to the tileset file
	*/
	void saveTileset( const char* file );
	/**
	*	Save the palette to a file.
	*	@param file : Path to the palette file
	*/
	void savePalette( const char* file );
	/**
	*	Draw a bicolor predefined tile on an SDL surface. Remember to call SDL_Flip(surface) so that the screen gets updated.
	*	@param surface : The SDL surface where the tile will be rendered
	*	@param tileIndex : The tile index in the tileset
	*	@param fgColor : The foreground color index in the palette
	*	@param bgColor : The background color index in the palette
	*	@param x : The X coordinate of the top-left corner of the tile
	*	@param y : The Y coordinate of the top-left corner of the tile
	*	@param w : The width (horizontal stretch) of the tile
	*	@param h : The height (vertical stretch) of the tile
	*	@param align : Whether to align to grid of tiles
	*/
	void drawTile( SDL_Surface* surface, int tileIndex,
				   int fgColor, int bgColor, int x, int y, int w, int h, bool align = true );

	void drawTile( SDL_Surface* surface, Tile tile,
				   int fgColor, int bgColor, int x, int y, int w, int h, bool align = true );
	/**
	*	Draw a multicolor predefined tile on an SDL surface. If colorMode is MULTICOLOR_DIRECT, then this function assumes that every value in the tile structure is a pixel color in the format 0xRRGGBB. If colorMode is MULTICOLOR_INDEXED, then this function assumes that every value in the tile structure is an index into the color palette. Remember to call SDL_Flip(surface) so that the screen gets updated.
	*	@param colorMode : The way in which the function will determine the colors of the tile.
	*	@param surface : The SDL surface where the tile will be rendered
	*	@param tileIndex : The tile index at the tileset
	*	@param x : The X coordinate of the top-left corner of the tile
	*	@param y : The Y coordinate of the top-left corner of the tile
	*	@param w : The width (horizontal stretch) of the tile
	*	@param h : The height (vertical stretch) of the tile
	*	@param align : Whether to align to grid of tiles
	*/
	void drawTileMC( int colorMode, SDL_Surface* surface, int tileIndex,
					 int x, int y, int w, int h, bool align = true );
	/**
	*	Draw a rectangle on an SDL surface. This is a low-level function used internally. It is not needed if you only want to manipulate tiles. If you use it, remember to call SDL_Flip(surface) so that the screen gets updated.
	*	@param surface : The SDL surface where the rectangle will be rendered
	*	@param x : The X coordinate of the top-left corner of the rectangle
	*	@param y : The Y coordinate of the top-left corner of the rectangle
	*	@param w : The width of the rectangle
	*	@param h : The height of the rectangle
	*	@param rgb : The color (0xRRGGBB) of the rectangle
	*/
	void drawRect( SDL_Surface* surface, int x, int y, int w, int h, int rgb );
	/**
	*	Directly put a pixel on an SDL surface. This is a low-level function used internally. It is not needed if you only want to manipulate tiles. If you use it, remember to call SDL_Flip(surface) so that the screen gets updated.
	*	@param surface : The SDL surface where the tile will be rendered
	*	@param x : The X coordinate of the pixel
	*	@param y : The Y coordinate of the pixel
	*	@param rgb : The color (0xRRGGBB) of the pixel
	*/
	void putPixel( SDL_Surface* surface, Sint16 x, Sint16 y, Uint32 rgb );
	/**
	*	Draw a string of consecutive tiles. If the string exceeds the width of the surface, then it wraps around to the next line of the surface. Remember to call SDL_Flip(surface) so that the screen gets updated.
	*	@param surface : The SDL surface where the tiles will be rendered
	*	@param x : The x coordinate of the top-left corner of the first tile
	*	@param y : The y coordinate of the top-left corner of the first tile
	*	@param fgColor : The foreground color index in the palette
	*	@param bgColor : The background color index in the palette
	*	@param w : The width (horizontal stretch) of the tiles
	*	@param h : The height (vertical stretch) of the tiles
	*	@param str : The string to draw
	*/
	void drawString( SDL_Surface* surface,
		int x, int y, int fgColor, int bgColor, int w, int h, const char* str );
	/**
	*	Fill the entire SDL surface with copies of the same bicolor tile.
	*	@param surface : The SDL surface where the tiles will be rendered
	*	@param tileIndex : The tile index in the tileset
	*	@param fgColor : The foreground color index in the palette
	*	@param bgColor : The background color index in the palette
	*	@param w : The width (horizontal stretch) of each tile
	*	@param h : The height (vertical stretch) of each tile
	*/
	void fill( SDL_Surface* surface,
		int tileIndex, int fgColor, int bgColor, int w, int h );
	/**
	*	Clear the entire SDL surface to the specified color. Calling this function automatically updates the screen.
	*	@param surface : The SDL surface to be cleared
	*	@param bgColor : The color index in the palette
	*/
	void clear( SDL_Surface* surface, int bgColor );
}
