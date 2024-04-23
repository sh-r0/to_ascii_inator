#include <iostream>
#include <vector>
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct rgba_t {
	uint8_t r, g, b, a;
};

inline double calcLuma(rgba_t _pixel) {
	return ( (_pixel.a/255.0) * ((_pixel.r * 0.2126 + _pixel.g * 0.7152 + _pixel.b * 0.0722)/255.0) );
}

inline double calcRGBA(rgba_t _pixel) {
	return (_pixel.r + _pixel.g + _pixel.b) / (255.0 * 3) * _pixel.a / 255.0;
}

inline double calcRed(rgba_t _pixel) {
	return _pixel.r / 255.0;
}

inline double calcGreen(rgba_t _pixel) {
	return _pixel.r / 255.0;
}

inline double calcBlue(rgba_t _pixel) {
	return _pixel.r / 255.0;
}

int32_t main(int32_t argc, char** argv) {
	if (argc < 2) {
		printf("Wrong calling method!\nShould be to_ascii_inator <filepath> /flags/");
		return EXIT_FAILURE;
	}

	std::string output_path = "output.txt";
	std::string filepath = argv[1];
	uint32_t out_x, out_y;
	double (*calcVal)(rgba_t) = calcLuma;
	int32_t x, y;
	int32_t channels;
	
	rgba_t* data = reinterpret_cast<rgba_t*>(stbi_load(filepath.c_str(), &x, &y, &channels, 4));
	if (data == nullptr) {
		fprintf(stderr, "couldnt open input file!\n");
		return EXIT_FAILURE;
	}
	out_x = x, out_y = y;
	
	std::unordered_map<std::string, uint8_t> argMap;
	argMap.insert({ "-o", 0 }); argMap.insert({ "-s", 1 }); argMap.insert({ "-r", 2 }); argMap.insert({ "-f", 3 });
	std::unordered_map<std::string, uint8_t> funcMap;
	funcMap.insert({ "luma", 0 }); funcMap.insert({ "rgba", 1 }); funcMap.insert({ "r",2 }); funcMap.insert({ "g",3 }); funcMap.insert({ "b",4 });
	for (uint32_t i = 1; i < argc; i++) {
		if (argMap.find( argv[i]) == argMap.end())
			continue;

		switch (argMap[argv[i]]) {
		case 0:
			output_path = argv[i + 1];
			i++;
			break;
		case 1:
			out_x = std::atoi(argv[i + 1]);
			out_y = std::atoi(argv[i + 2]);
			i+=2;
			break;
		case 2:
			out_x /= std::atoi(argv[i + 1]);
			out_y /= std::atoi(argv[i + 1]);
			i++;
			break;
		case 3:
			if (funcMap.find(argv[i+1]) == funcMap.end())
				continue;

			switch (funcMap[argv[i + 1]]) {
			case 0:
				calcVal = calcLuma;
				break;
			case 1:
				calcVal = calcRGBA;
				break;
			case 2:
				calcVal = calcRed;
				break;
			case 3:
				calcVal = calcGreen;
				break;
			case 4:
				calcVal = calcBlue;
				break;
			}

			break;
		}
	}

	FILE* outputFile = fopen(output_path.c_str(), "w");
	if (outputFile == nullptr) {
		fprintf(stderr, "couldnt open output file!\n");
		return EXIT_FAILURE;
	}

	char* line = reinterpret_cast<char*>( malloc(out_x + 2));
	line[out_x] = '\n';
	line[out_x + 1] = 0;
	for (uint32_t i = 0; i < out_y; i++) {
		double start_y = 0, end_y = 0;
		std::vector<std::pair<uint32_t, double>> wages;
		start_y = double(i) * y / out_y;
		end_y = double(i + 1) * y / out_y;

		uint32_t k = floor(start_y);
		if (end_y <= ceil(start_y)) {
			wages.push_back({k, end_y - start_y});
		} else {
			while (1) {
				if (k < start_y) {
					wages.push_back({ k, ceil(start_y) - start_y });
				} else {
					wages.push_back({ k, k + 1 < end_y ? 1 : end_y - k });
				}
				k++;
				if (k >= ceil(end_y)) break;
			 }
		}

		for (uint32_t j = 0; j < out_x; j++) {
			double luma = 0;
			double start_x, end_x;
			start_x = double(j) * x / out_x;
			end_x = double(j + 1) * x / out_x;
			uint32_t k2 = floor(start_x);

			if (end_x <= ceil(start_x)) {
				double w2 = end_x - start_x;
				for (const auto& w : wages) {
					luma += calcVal(data[w.first * x + k2]) * w.second * w2;
				}
			} else {
				while (1) {
					if (k2 < start_x) {
						double w2 = ceil(start_x) - start_x;
						for (const auto& w : wages) {
							luma += calcVal(data[w.first * x + k2]) * w.second * w2;
						}
					}
					else {
						double w2 = k2 + 1 < end_x ? 1 : end_x - k2;
						for (const auto& w : wages) {
							luma += calcVal(data[w.first * x + k2]) * w.second * w2;
						}
					}
					k2++;
					if (k2 >= ceil(end_x)) break;
				}
			}
			luma /= (end_x - start_x) * (end_y - start_y);
			luma = luma < 1 ? luma : 1;
			
			line[j] = "@$#*!=;:~-,."[unsigned(luma * 11)];
		}
		if (fputs(line, outputFile) == EOF) fprintf(stderr,"error writing buffer to file at line %u!\n", i);
	}

	fclose(outputFile);
	return 0;
}