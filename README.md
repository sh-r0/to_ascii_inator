# to_ascii_inator - a basic image to text converter

## input filetypes
Program can decode many different image formats such as .jpg, .png and .gif.  
To learn more check stb library supported formats.

## usage
Program should be called using following convention:  
./to_ascii_inator.exe //input filepath// /optional flags/

Flags:  
"-o" - specifying output filename (1 parameter)  
"-s" specifying x and y size of text (2 parameters)  
"-r" specifying magnification rate [2 means image will be 2x zoomed] (1 parameter)  
"-f" specifying function type used to calculate ascii value (1 parameter)  
currently supported function types:  
"luma" - calculation luminance basing on rec. 709 standard, "rgba" - calculating average value of every colour, "r" - taking value of red, "g" - taking value of green, "b" - taking value of blue


##Dependencies

Program uses "stb_image.h" header from stb library that can be found at https://github.com/nothings/stb