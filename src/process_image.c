#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    /**
     * Gets the pixel value at given coordinate.
     * 
     * Since all data are stored linearly on the memory, we need to do
     * some basic arithmetic to translate the image's 3D coordinates
     * into a linear array. If a request to get pixel is received with
     * pixel values that are out of bound of the image the closest valid
     * pixel along the same axis is returned.
     * 
     * @param[out] im the image where the pixel is to be retrieved
     * @param[in] x the x location of the pixel (along the image's width)
     * @param[in] y the y location of the pixel (along the image's height)
     * @param[in] c the channel number of the image (along the z-axis)
     * 
     * @returns the pixel value at the given coordinate in the image
     * 
    */ 
    
    x = x > im.w ? im.w - 1 : x;
    y = y > im.h ? im.h - 1 : y;
    c = c > im.c ? im.c - 1 : c;
    
    x = x < 0 ? 0: x;
    y = y < 0 ? 0: y;
    c = c < 0 ? 0: c;
    
    return im.data[x + y * im.w + im.w * im.h * c];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    /**
     * Sets the value of pixel at given coordinate.
     * 
     * In case of trying to set a pixel that is out of bounds of the image,
     * we just ignore the request. Transformation similar to `get_pixel` is 
     * performed to access the pixel in memory.
     * 
     * @param im the image where the pixel is to be set
     * @param x the x location of the pixel (along the image's width)
     * @param y the y location of the pixel (along the image's height)
     * @param c the channel number of the image (along the z-axis)
     * @param v the value to be set
     * 
     */
    
    if (x > im.w) return;
    if (y > im.h) return;
    if (c > im.c) return;
    
    if (x < 0) return;
    if (y < 0) return;
    if (c < 0) return;
    
    im.data[x + y * im.w + im.w * im.h * c] = v;
}

image copy_image(image im)
{
    /**
     * Creates a copy of a given image.
     * 
     * We use an in-built C function memcpy to perform the copy operations 
     * instead of iterating through the entire image tensor
     * 
     * @param im the source image
     * 
     * @returns copy of the given image
     * 
    */ 
    
    image copy = make_image(im.w, im.h, im.c);
    memcpy(copy.data, im.data, im.w * im.h * im.c * sizeof(float));
    return copy;
}

image rgb_to_grayscale(image im)
{
    /**
     * Converts a given color image into grayscale.
     * 
     * Channels are extracted from the image and a certain 'luma calculation' 
     * is performed to squish the three channels into a single one which will
     * be our grayscale image. For more information on the luma calculation, 
     * go through this Wikipedia article: 
     * https://en.wikipedia.org/wiki/Luma_(video)
     * 
     * In short, luma calculation can be summarized by 
     * Y' = 0.299 R' + 0.587 G' + .114 B' where Y' is the grayscale pixel, and 
     * R, G, B are the corresponding red, green, and blue pixels.
     * 
     * @param im the source image
     * 
     * @returns grayscale image
     * 
    */ 
    
    assert(im.c == 3); // The source image must have three channels
    image gray = make_image(im.w, im.h, 1);
    
    for (int i = 0; i < im.w; i ++) 
    {
        for (int j = 0; j < im.h; j ++) 
        {
            float red   = get_pixel(im, i, j, 0);
            float green = get_pixel(im, i, j, 1);
            float blue  = get_pixel(im, i, j, 2);
            
            set_pixel(gray, i, j, 0, red * 0.299 + green * 0.587 + blue * 0.114);
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    /**
     * Shfit all the pixels in a channel of the given image by some value v.
     * 
     * Basically, we add the value v to all the pixels in a particular channel.
     * Using this we can brighten (or darken) the intensities of various channels.
     * 
     * @param[out] im the image to shift
     * @param[in] c the channel to shift
     * @param[in] v the value by which to shift
     * 
    */ 
    
    for (int i = 0; i < im.w; i ++)
    {
        for (int j = 0; j < im.h; j ++)
        {
            set_pixel(im, i, j, c, get_pixel(im, i, j, c) + v);
        }
    }
}

void clamp_image(image im)
{
    /**
     * Checks all the pixels in all channels of given image and make sure they are within bounds.
     * 
     * A pixel of an image is usually stored as 1 byte. When we perform shift operation 
     * on the image, it is possible that the value of pixel might exceed the limits of
     * a byte (pixel value greater than 1). This function makes sure that any out of bounds
     * pixels are contained within bounds.
     * 
     * @param[out] im the image to be clamped
     * 
    */ 
    
    for (int i = 0; i < im.w; i ++)
    {
        for (int j = 0; j < im.h; j ++)
        { 
            for (int c = 0; c < im.c; c ++)
            {
                if (get_pixel(im, i, j, c) > 1) set_pixel(im, i, j, c, 1);
                if (get_pixel(im, i, j, c) < 0) set_pixel(im, i, j, c, 0);
            }
        }
    }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    /**
     * Finds and returns maximum among three values
     * 
     * @param floats a, b and c
     * 
     * @returns maximum(a, b, c)
     * 
     */ 
    
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    /**
     * Finds and returns minimum among three values
     * 
     * @param floats a, b and c
     * 
     * @returns minimum(a, b, c)
     * 
     */
    
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    /**
     * Converts an image from RGB colorspace to HSV colorspace.
     * 
     * First we calculate value. It is very simple. The maximim among
     * the three components of RGB is the value. Saturation is a measure 
     * of how much color is in the pixel compared to neutral white/gray.
     * It is calculated by using formula S = C / V where C is the difference
     * between max and min of the components and V is the value calculated
     * earlier. Finally we calculate hue. For calculation details please refer 
     * to https://en.wikipedia.org/wiki/HSL_and_HSV#Hue_and_chroma
     * 
     * @param[out] im image whose colorspace needs changed
     * 
     */
    
    for (int w = 0; w < im.w; w ++)
    {
        for (int h = 0; h < im.h; h ++) 
        {
            float red = get_pixel(im, w, h, 0);
            float green = get_pixel(im, w, h, 1);
            float blue = get_pixel(im, w, h, 2);
            
            // the largest of the RGB components is the Value
            float value = three_way_max(red, green, blue);
            float min = three_way_min(red, green, blue);
            
            float diff = value - min;
            
            float saturation = 0;
            float hue = 0;
            
            // Saturation is the ratio between the difference and value
            if (value > 0) saturation = diff / value;
            
            // if diff is zero, we will leave hue to be zero
            if (diff != 0) {
                if (value == red) hue = (green - blue) / diff;
                else if (value == green) hue = (blue - red) / diff + 2;
                else hue = (red - green) / diff + 4;
                
                hue = hue < 0 ? hue / 6 + 1 : hue / 6;
            }
                        
            set_pixel(im, w, h, 0, hue);
            set_pixel(im, w, h, 1, saturation);
            set_pixel(im, w, h, 2, value);
        }
    }
}

void hsv_to_rgb(image im)
{
    /**
     * Converts an image from HSV colorspace to RGB colorspace.
     * 
     * This function is the inverse of the previous function. First, we find 
     * chroma C = Value x Saturation. Then we can find a point (R, G, B) along 
     * the bottom three faces of the RGB cube with the same hue and chroma as 
     * our color. For further information, visit:
     * https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_RGB
     * 
     * @param[out] im image whose colorspace needs changed
     * 
     */
    
    for (int w = 0; w < im.w; w ++)
    {
        for (int h = 0; h < im.h; h ++)
        {
            float hue = get_pixel(im, w, h, 0);
            float saturation = get_pixel(im, w, h, 1);
            float value = get_pixel(im, w, h, 2);
            
            float red = 0, green = 0, blue = 0;
            
            float chroma = value * saturation;
            float _h = hue * 6.0;
            
            float X = chroma * (1 - fabs(fmod(_h, 2.0) - 1));
            float diff = value - chroma;
            
            if (0 <= _h && _h <= 1.0) 
            {
                red = chroma;
                green = X;
                blue = 0;
            }
            
            else if (1.0 <= _h && _h <= 2.0)
            {
                red = X;
                green = chroma;
                blue = 0;
            }
            
            else if (2.0 <= _h && _h <= 3.0)
            {
                red = 0;
                green = chroma;
                blue = X;
            }
            
            else if (3.0 <= _h && _h <= 4.0)
            {
                red = 0;
                green = X;
                blue = chroma;
            }
            
            else if (4.0 <= _h && _h <= 5.0) 
            {
                red = X;
                green = 0;
                blue = chroma;
            }
            
            else if (5.0 <= _h && _h <=6.0)
            {
                red = chroma;
                green = 0;
                blue = X;
            }
            
            else
            {
                red = green = blue = 0;
            }
            
            red += diff;
            green += diff;
            blue += diff;
            
            set_pixel(im, w, h, 0, red);
            set_pixel(im, w, h, 1, green);
            set_pixel(im, w, h, 2, blue);
        }
    }
}
