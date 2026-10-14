#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>







#pragma pack(push, 1)
typedef struct
{
    
    char        Chunk_ID[4];
    uint32_t    chunk_size;
    char        format[4];

    char        Subchunk1ID[4];
    uint32_t    Subchunk1Size;
    uint16_t    AudioFormat;
    uint16_t    NumChannels;
    uint32_t    SampleRate;
    uint32_t    ByteRate;
    uint16_t    BlockAlign;
    uint16_t    BitsPerSample;
    char        Subchunk2ID[4];
    uint32_t    Subchunk2Size;

}WaveHeader;
#pragma pack(pop)

typedef struct 
{
    int16_t       *buffer;
    size_t      total_sample;
    uint32_t    sample_size;
}Audio_buffer;

typedef struct
{
    const float *coeffs;
    float       *delay_line;
    size_t      num_taps;
    size_t      write_index;

}DSPFilter;


typedef struct
{
    uint8_t     *pixel_data;
    size_t      width;
    size_t      height;
}ImageFrame;



#endif