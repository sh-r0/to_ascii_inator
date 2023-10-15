# to_ascii_inator
Simple program creating ascii art from image  
Program supports file formats: JPG, PNG, GIF, BMP
# Usage 
Program supports 4 flags which names should be pretty self-explanatory: "-output", "-size", "-scale" and "-func"  
"-output" should be followed by filename that the ascii will go into,  
"-size" takes 2 parameters which are X and Y sizes of generated "image",  
"-scale" takes 1 parameter scaling output size automatically, it's exclusive with "-size" flag,  
"-func" changes function used to calculate value of ascii character, valid values are: "709", "601", "equ", first two use luma weights of 709 and 601 standard, while "equ" uses equal weights of every colour  
Example usage:
to_ascii_inator.exe inputFile.jpg -scale 2 -func 709 -output outputFile.txt
