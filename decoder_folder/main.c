#include "header.h"


WaveHeader wave_1;
Audio_buffer audio_buf_1;
DSPFilter dsp_1;
ImageFrame img_1;

const float num_taps[15] = {0.005f, -0.012f, -0.031f, 0.0f, 0.088f, 0.215f, 0.324f, 0.377f, 0.324f, 0.215f, 0.088f, 0.0f, -0.031f, -0.012f, 0.005f};


float *dsp_filter(DSPFilter *dsp_1)
{
    dsp_1->num_taps = 15;

    dsp_1->coeffs = num_taps;

    dsp_1->delay_line = calloc(dsp_1->num_taps, sizeof(float));

    dsp_1->write_index = 0;

    float *envelope = malloc(audio_buf_1.total_sample * sizeof(float));

    for (size_t i = 0; i < audio_buf_1.total_sample; i++)
    {
        dsp_1->delay_line[dsp_1->write_index] = fabsf((float)audio_buf_1.buffer[i] / 32768.0f);

        float sum = 0.0f;
        for (size_t k = 0; k < dsp_1->num_taps; k++)
        {
            int idx = (int)dsp_1->write_index - (int)k;
            if (idx < 0)
            {
                idx += dsp_1->num_taps;
            }
            sum += dsp_1->delay_line[idx] * dsp_1->coeffs[k];
        }
        envelope[i] = sum;
        dsp_1->write_index = (dsp_1->write_index + 1) % dsp_1->num_taps;
        
    }
    free(dsp_1->delay_line);
    return envelope;
}

int img_frame(ImageFrame *img_1, float *envelope)
{
    img_1->width = 2080;
    img_1->height = (size_t)(audio_buf_1.total_sample / 5512.5f);
    img_1->pixel_data = malloc(img_1->width * img_1->height * sizeof(uint8_t));

    if(!img_1->pixel_data)
    {
        return 1;
    }

    for (size_t row = 0; row < img_1->height; row++)
    {
        float line_start = row * 5512.5f;
        for (size_t col = 0; col < img_1->width; col++)
        {
            size_t sample_idx = (size_t)(line_start + (col * 5512.5f / 2080.0f));
            int pixel_val = envelope[sample_idx] * 255.0f * 2.5f;
            if(pixel_val > 255)
            {
                pixel_val = 255;
            }
            img_1->pixel_data[row * img_1->width + col] = (uint8_t)pixel_val;
        }
    }

    FILE *pgm_file = fopen("sat_image.pgm", "wb");
    fprintf(pgm_file, "P5\n%zu %zu\n255\n", img_1->width, img_1->height);
    fwrite(img_1->pixel_data, sizeof(uint8_t), img_1->width * img_1->height, pgm_file);
    fclose(pgm_file);

}

int main()
{
    FILE *file_open = fopen("sat_wave_sample.wav", "rb");
    if(!file_open)
    {
        return 1;
    }

    fread(&wave_1, sizeof(WaveHeader), 1, file_open);

    audio_buf_1.sample_size = wave_1.SampleRate;

    audio_buf_1.total_sample = wave_1.Subchunk2Size / 2;

    audio_buf_1.buffer = malloc(audio_buf_1.total_sample * sizeof(int16_t));
    if(!audio_buf_1.buffer)
    {
        return 1;
    }
    fread(audio_buf_1.buffer, sizeof(int16_t), audio_buf_1.total_sample, file_open);

    fclose(file_open);

    float *env_data = dsp_filter(&dsp_1);
    printf("smaple_1 %f\n", env_data[100]);

    img_frame(&img_1, env_data);
    return 0;
}