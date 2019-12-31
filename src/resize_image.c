#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    /**
     * Interpolates the value of pixel in an image using nearest neighbour method.
     * 
     * Given an image we round our float coordinates to nearest int, and return the 
     * resulting point in the image.
     * 
     * @param im the image to interpolate nearest neighbour from
     * @param float x and y float value to interpolate the nearest neighbour
     * @param c image channel
     * 
     * @returns nearest neighbour corresponding to given real values in the image
     * 
    */
    
    return get_pixel(im, round(x), round(y), c);
}

image nn_resize(image im, int w, int h)
{
    /** 
     * Resize the given image with provided width and height args.
     * 
     * We need to interpolate between the pixels. In other words, we need values
     * for pixel positions like (1.25, 3.75) and other such positions where pixel 
     * values are unavailable. In this case we use nearest neighbour method.
     * 
     * @param im the image to resize
     * @param w new width of the image
     * @param h new height of the image
     * 
     * @returns resized image
    */ 
    
    image resized_image = make_image(w, h, im.c);

    float ratio_x = (float)im.w / (float)w;
    float correction_factor_x = -0.5 + 0.5 * ratio_x;

    float ratio_y = (float)im.h / (float)h;
    float correction_factor_y = -0.5 + 0.5 * ratio_y;

    int z, row, col;

    for (row = 0; row < resized_image.h; row++)
    {
        for (col = 0; col < resized_image.w; col++)
        {
            float x = ratio_x * col + correction_factor_x;
            float y = ratio_y * row + correction_factor_y;
            
            for (z = 0; z < resized_image.c; z++)
            {
                float val = nn_interpolate(im, x, y, z);
                set_pixel(resized_image, col, row, z, val);
            }
        }
    }
    return resized_image;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    /**
     * Interpolates the value of pixel in an image using bilinear interpolation method.
     * 
     * Given an image we approximate our float coordinates based on its distance from
     * closest four real pixel values, and returns it.
     * 
     * @param im the image to interpolate unavailable pixels from
     * @param floats x and y pixel coordinates
     * @param c image channel
     * 
     * @returns pixel value approximation corresponding to given real values in the image
     * 
    */
    
    int left = floorf(x);
    int right = ceilf(x);
    int bottom = ceilf(y);
    int top = floorf(y);
    
    float v1 = get_pixel(im, left, top, c);
    float v2 = get_pixel(im, right, top, c);
    float v3 = get_pixel(im, left, bottom, c);
    float v4 = get_pixel(im, right, bottom, c);

    float q1 = (bottom - y) * v1 + (y - top) * v3;
    float q2 = (bottom - y) * v2 + (y - top) * v4;

    float q = (x - left) * q2 + (right - x) * q1;

    return q;
}

image bilinear_resize(image im, int w, int h)
{
    /** 
     * Resize the given image with provided width and height args.
     * 
     * We need to interpolate between the pixels. In other words, we need values
     * for pixel positions like (1.25, 3.75) and other such positions where pixel 
     * values are unavailable. In this case we use bilinear interpolation method.
     * 
     * @param im the image to resize
     * @param w new width of the image
     * @param h new height of the image
     * 
     * @returns resized image
    */ 
    
    image resized_image = make_image(w, h, im.c);
    
    float ratio_x = (float)im.w / (float)w;
    float correction_factor_x = -0.5 + 0.5 * ratio_x;
    
    float ratio_y = (float)im.h / (float)h;
    float correction_factor_y = -0.5 + 0.5 * ratio_y;
    
    int z, row, col;
    
    for (row = 0; row < resized_image.h; row++)
    {
        for (col = 0; col < resized_image.w; col++)
        {
            float x = ratio_x * col + correction_factor_x;
            float y = ratio_y * row + correction_factor_y;
            
            for (z = 0; z < resized_image.c; z++)
            {
                float val = bilinear_interpolate(im, x, y, z);
                set_pixel(resized_image, col, row, z, val);
            }
        }
    }
    
    return resized_image;
}

