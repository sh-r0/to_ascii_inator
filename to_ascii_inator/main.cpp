#include <iostream>
#include <string>
#include <unordered_map>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

const char asciiArray[] = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\" ^ `'. ";

//calculating luma based on Rec. 709 standard
inline double calcLuma_709(uint8_t* _mem) {
	double sum = *_mem * 0.2126;
	sum += *(_mem + 1) * 0.7152;
	sum += *(_mem + 2) * 0.0722;
	sum *= *(_mem + 3) / double(255*255);
	return sum; 
}

//calculating luma based on Rec. 601 standard
inline double calcLuma_601(uint8_t* _mem) {
	double sum = *_mem * 0.299;
	sum += *(_mem + 1) * 0.587;
	sum += *(_mem + 2) * 0.114;
	sum *= *(_mem + 3) / double(255 * 255);
	return sum;
}

//calculating luma based on equal weights of colours
inline double calc_equal(uint8_t* _mem) {
	double sum = *_mem;
	sum += *(_mem + 1);
	sum += *(_mem + 2);
	sum *= *(_mem + 3) / double(3 * 255 * 255);
	return sum;
}

enum flagType : uint8_t {
	flag_sizes, flag_threshold, flag_output, flag_scale, flag_func
};

struct outputData_t {
	uint32_t x, y;
	uint8_t threshold = 0;
	std::string output = "output.txt";
};

int32_t main(int32_t argc, char** argv) {
	if (argc < 2)
		return 1;
	const char outputFile[] = "output.txt";
	outputData_t outputData;
	int32_t oldx = 0, oldy = 0;
	int32_t channels;

	if (!std::filesystem::exists(argv[1])) {
		std::cout << "Couldn't find target file!";
		return 1;
	}

	uint8_t* image = stbi_load(argv[1], &oldx, &oldy, &channels, STBI_rgb_alpha);
	outputData.x = oldx; outputData.y = oldy;
	uint64_t scale = 1;
	double (*calcFunction)(uint8_t*) = calcLuma_709;
	{
		std::unordered_map<std::string, flagType> types = { {"-size", flag_sizes} , {"-output", flag_output}, {"-scale", flag_scale}, {"-func", flag_func }};
		for (uint8_t i = 0; i < argc;) {
			if (!types.contains(argv[i])) { i++; continue; }
			switch (types[argv[i]]) {
			case flag_sizes:
				outputData.x = std::atoi(argv[i + 1]);
				outputData.y = std::atoi(argv[i + 2]);
				i += 3;
				break;
			case flag_output:
				outputData.output = argv[i + 1];
				i += 2;
				break;
			case flag_scale:
				scale = std::atoi(argv[i + 1]);
				i += 2;
				break;
			case flag_func: {
				std::unordered_map<std::string, uint8_t> funcTypes = { {"709", 0 } , {"601", 1}, {"equ", 2}};
				if (!funcTypes.contains(argv[i+1])) { i+=2; continue; }
				switch (funcTypes[argv[i+1]]) {
				case 0:
					calcFunction = calcLuma_709;
					break;
				case 1:
					calcFunction = calcLuma_601;
					break;
				case 2:
					calcFunction = calc_equal;
					break;
					}
				i += 2;
				} break;
			}
		}
	}

	FILE* output = fopen(outputData.output.c_str(), "w");
	if (!output) {
		fprintf(stderr, "Failed to open %s file for writing!\n", outputData.output.c_str());
		return 1;
	}

	double newPixelSizeX;
	double newPixelSizeY;
	if (scale == 1) {
		newPixelSizeX = oldx/double(outputData.x);
		newPixelSizeY = oldy/double(outputData.y);
	}
	else {
		newPixelSizeX = scale;
		newPixelSizeY = scale;
		outputData.x = oldx/scale;
		outputData.y = oldy /scale;
	}
	double yRemainder = 1, xRemainder = 1, yremainder;

	std::vector<char>lineToPush(outputData.x + 1);
	lineToPush[outputData.x] = '\n';

	uint8_t* mem = image;
	for (uint32_t i = 0; i < outputData.x * outputData.y; ++i) {
		double xCurrent = newPixelSizeX;
		double luma = 0;
		while (xCurrent > 0) {
			uint8_t* yMem = mem;
			yremainder = yRemainder;
			double yCurrent = newPixelSizeY;
			double lumaSum = 0;
			while (yCurrent > 0) {
				if (yCurrent >= yremainder) {
					lumaSum += yremainder * calcFunction(mem);

					yCurrent -= yremainder;
					yremainder = 1;
					yMem += 4 * oldx;
				}
				else {
					lumaSum += calcFunction(mem);
					yremainder -= yCurrent;
					yCurrent = 0;
				}
			}
			lumaSum /= newPixelSizeY;
			if (xCurrent >= xRemainder) {
				luma += lumaSum * xRemainder;
				xCurrent -= xRemainder;
				xRemainder = 1;
				mem += 4;
			}
			else {
				luma += lumaSum * xCurrent;
				xRemainder -= xCurrent;
				xCurrent = 0;
			}
		}
		luma /= newPixelSizeX;
		lineToPush[i%outputData.x] = asciiArray[int(luma * sizeof(asciiArray))];
		if (i % outputData.x == outputData.x - 1) {
			fputs(lineToPush.data(), output);
			mem += oldx * 4 * int(newPixelSizeY-1);
			yRemainder = yremainder;
		}
	}

	return 0;
}