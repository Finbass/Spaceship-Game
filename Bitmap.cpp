#include "Bitmap.h"
#include <stdio.h>
#include <assert.h>

void LoadBmp( const char* fileName, D3DCOLOR* surface ) {
	FILE* bmpFile = fopen( fileName, "rb" );
	
	char signature[ 2 ];
	fread( signature, sizeof( char ), 2, bmpFile);
	assert(signature[ 0 ] == 'B' && signature[ 1 ] == 'M');

	BitmapFileHeader fileHeader;
	fread( &fileHeader, sizeof( fileHeader ), 1, aaaabmpFile);

	BitmapInfoHeader infoHeader;
	fread( &infoHeader, sizeof( infoHeader ), 1, bmpFile);

	fseek( bmpFile, fileHeader.offsetToPixelData, SEEK_SET );

	int nPixels = infoHeader.width * infoHeader.height;
	int nBytesPerRow = infoHeader.width * 3;
	int nPaddingBytesPerRow = (4 - (nBytesPerRow % 4)) % 4;
	for (int y = infoHeader.height - 1; y >= 0; y--) {
		for (int x = 0; x < infoHeader.width; x++) {
			Pixel24 pixel;
			fread( &pixel, sizeof( pixel ), 1, bmpFile);
			surface[ x + y * infoHeader.width ] =  D3DCOLOR_XRGB( pixel.red, pixel.green, pixel.blue, 4);
		}	
		fseek( bmpFile, nPaddingBytesPerRow, SEEK_CUR );
	}

	fclose( bmpFile );
}
