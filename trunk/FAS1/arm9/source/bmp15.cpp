/*---------------------------------------------------------------------------------


Copyright (C) 2007 Acekard, www.acekard.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


---------------------------------------------------------------------------------*/









#include <list>
#include <string>
#include "bmp15.h"

cBMP15::cBMP15() : _width(0), _height(0), _pitch(0), _buffer(NULL)
{
}

cBMP15::cBMP15( u32 width, u32 height ) : _width(0), _height(0), _pitch(0), _buffer(NULL)
{
    _width = width;
    _height = height;
    _pitch = (width + (width & 1)) << 1;
    //u32 pitch = (((width*16)+31)>>5)<<2;            // 通用算法？
}

cBMP15::~cBMP15()
{
}

cBMP15 createBMP15( u32 width, u32 height )
{
    cBMP15 bmp( width, height );

    u32 pitch = bmp.pitch();           // 15bit bmp pitch 算法

    u32 bufferSize = height * pitch;
    if( bufferSize & 3 ) // 如果 bufferSize 不是按4字节对齐，就把他调整到对齐
        bufferSize += 4 - (bufferSize & 3);
    bmp._buffer = new u32[bufferSize>>2];
    return bmp;
}


typedef std::pair< std::string, cBMP15 >  str_bmp_pair;
typedef std::list< str_bmp_pair > str_bmp_list;
static str_bmp_list _bmpPool;

cBMP15 createBMP15FromMem( void * mem )
{
    return cBMP15();
}

cBMP15 createBMP15FromFile( const char* filename )
{
    str_bmp_list::iterator it;
    for( it = _bmpPool.begin(); it != _bmpPool.end(); ++it )
    {
        if( filename == it->first ) {
            return it->second;
        }
    }

    FILE * f = fopen( filename, "rb" );
    if( NULL == f ) {
        return cBMP15();
    }

    // 读取文件长度
    fseek( f, 0, SEEK_END );
    int fileSize = ftell( f );

    if( -1 == fileSize ) {
        fclose( f );
        return cBMP15();
    }

    u16 bmMark = 0;
    fseek( f, 0, SEEK_SET );
    fread( &bmMark, 1, 2, f );
    if( bmMark != 0x4d42 ) {// 'B' 'M' header
        fclose( f );
        return cBMP15();
    }

    // 找出bmp高和宽
    u32 width = 0;
    u32 height = 0;
    fseek( f, 0x12, SEEK_SET );
    fread( &width, 1, 4, f );
    fseek( f, 0x16, SEEK_SET );
    fread( &height, 1, 4, f );

    cBMP15 bmp = createBMP15( width, height );

    u32 bmpDataOffset = 0;
    fseek( f, 0x0a, SEEK_SET );
    fread( &bmpDataOffset, 1, 4, f );

    long position = bmpDataOffset;
    fseek( f, position, SEEK_SET );
    u16 * pbuffer = ((u16 *)bmp.buffer()) + (bmp.pitch()>>1) * height - (bmp.pitch()>>1);

    for( u32 i = 0; i < height; ++i ) {
        fread( pbuffer, 1, bmp.pitch(), f );
        position += bmp.pitch();
        pbuffer -= bmp.pitch() >> 1;
        fseek( f, position, SEEK_SET );
    }
    fclose( f );

    pbuffer = (u16 *)bmp.buffer();
    for( u32 i = 0; i < height; ++i )
    {
        for( u32 j = 0; j < (bmp.pitch()>>1); ++j )
        {
            u16 pixelColor = pbuffer[i*(bmp.pitch()>>1)+j];
            pixelColor = ((pixelColor & 0x7C00) >> 10)
                | ((pixelColor & 0x03E0)) | ((pixelColor & 0x1F) << 10);
            pbuffer[i*(bmp.pitch()>>1)+j] = pixelColor | (pixelColor ? BIT(15):0);
        }
    }

    str_bmp_pair bmpPoolItem( std::string(filename), bmp );
    _bmpPool.push_back( bmpPoolItem );

    return bmp;
}

