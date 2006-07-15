// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->Cxbx->jpegdec->jpegdec.cpp
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx and Cxbe are free software; you can redistribute them
// *  and/or modify them under the terms of the GNU General Public
// *  License as published by the Free Software Foundation; either
// *  version 2 of the license, or (at your option) any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// *  GNU General Public License for more details.
// *
// *  You should have recieved a copy of the GNU General Public License
// *  along with this program; see the file COPYING.
// *  If not, write to the Free Software Foundation, Inc.,
// *  59 Temple Place - Suite 330, Bostom, MA 02111-1307, USA.
// *
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#include "jpegdec.h"

#include <stdio.h>
#include <malloc.h>

extern "C"
{
    #include "jpeglib.h"
}

static void init_source(j_decompress_ptr cinfo)
{
    // do nothing!
}

static boolean fill_input_buffer(j_decompress_ptr cinfo)
{
    return FALSE; 
}

static void skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
    if ((size_t)num_bytes > cinfo->src->bytes_in_buffer) {
        cinfo->src->next_input_byte = NULL;
        cinfo->src->bytes_in_buffer = 0;
    } else {
        cinfo->src->next_input_byte += (size_t) num_bytes;
        cinfo->src->bytes_in_buffer -= (size_t) num_bytes;
    }
}

static void term_source(j_decompress_ptr cinfo)
{
    // do nothing!
} 

void jpeg_memory_src(j_decompress_ptr cinfo, uint08 *buffer, uint32 bufferSize)
{
    jpeg_source_mgr *src;

    src = cinfo->src = (jpeg_source_mgr *)malloc(sizeof(jpeg_source_mgr));

    src->init_source       = init_source;
    src->fill_input_buffer = fill_input_buffer;
    src->skip_input_data   = skip_input_data;
    src->resync_to_restart = jpeg_resync_to_restart;
    src->term_source     = term_source;
    src->next_input_byte = buffer;
    src->bytes_in_buffer = bufferSize;
}

// convert in memory jpeg to bmp
uint08 *jpeg2bmp(uint08 *jpeg, uint32 jpegSize, uint32 *bmpSize, uint32 *bmpWidth, uint32 *bmpHeight)
{
    jpeg_decompress_struct cinfo;
    jpeg_error_mgr         jerr;

    uint08 *buffer = 0, *bufcur = 0;

    uint32 row_stride = 0, y;

    cinfo.err = jpeg_std_error(&jerr);

    // initialize jpeg decompression object
    jpeg_create_decompress(&cinfo);

    // specify data source
    //jpeg_stdio_src(&cinfo, infile);
    jpeg_memory_src(&cinfo, jpeg, jpegSize);

    // read jpeg header
    jpeg_read_header(&cinfo, TRUE);

    // begin decompression
    jpeg_start_decompress(&cinfo);

    // update row_stride
    row_stride = cinfo.output_width * cinfo.output_components;

    // save these for caller
    *bmpWidth = cinfo.output_width;
    *bmpHeight = cinfo.output_height;

    // calculate bitmap size
    *bmpSize = row_stride*cinfo.output_height;

    // allocate bitmap data
    buffer = (uint08*)malloc(*bmpSize);

    bufcur = buffer;
    for(y=0;y<cinfo.output_height;y++)
    {
        jpeg_read_scanlines(&cinfo, (JSAMPARRAY)&bufcur, 1);
        bufcur += row_stride;
    }

    // re-order color data
    {
        uint32 v=0;
        
        for(v=0;v<cinfo.output_width*cinfo.output_height;v++)
        {
            uint08 r = buffer[v*3+2];
            uint08 g = buffer[v*3+1];
            uint08 b = buffer[v*3+0];

            buffer[v*3+2] = b;
            buffer[v*3+1] = g;
            buffer[v*3+0] = r;
        }
    }

    jpeg_finish_decompress(&cinfo);

    free(cinfo.src);

    jpeg_destroy_decompress(&cinfo);

    *bmpSize = row_stride*cinfo.output_height;

    return (uint08*)buffer;
}
