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









#ifndef _BMP15_H_
#define _BMP15_H_

#include <nds.h>
#include <memory>

class cBMP15
{
    friend cBMP15 createBMP15( u32 width, u32 height );
    friend cBMP15 createBMP15FromFile( const char * filename );
    friend cBMP15 createBMP15FromMem( void * mem );
    //friend void destroyBMP15( cBMP15 * bmp );

public:

    explicit cBMP15();
    explicit cBMP15( u32 width, u32 height );
    ~cBMP15();

public:

    u32 width() const { return _width; }        // width is memory bitmap's width

    u32 height() const { return _height; }    // height is memory bitmap's height

    u32 pitch() const { return _pitch; }        // pitch returns bytes per line

    u32 * buffer() { return _buffer; }

    const u32 * buffer() const { return _buffer; }

    bool valid() const { return NULL != _buffer; }

protected:

    u32 _width;

    u32 _height;

    u32 _pitch;

    u32 * _buffer;    // 按 32 位地址对齐，可以在 bitblt 的时候加快速度
};

cBMP15 createBMP15( u32 width, u32 height );
cBMP15 createBMP15FromFile( const char * filename );
cBMP15 createBMP15FromMem( void * mem );
//void destroyBMP15( cBMP15 * bmp );
//void destroyBMP15ByFilename( const char * filename );


#endif//_BMP15_H_
