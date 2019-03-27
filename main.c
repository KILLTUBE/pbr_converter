#include <stdio.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

enum {
	AO_TEX = 0,
	DIFF_TEX,
	DISP_TEX,
	NOR_TEX,
	ROUGH_TEX,
	SPEC_TEX,
	NUM_TEX_TYPES
};

char *substrings[NUM_TEX_TYPES] = {
	"_AO_",
	"_diff_",
	"_disp_",
	"_nor_",
	"_rough_",
	"_spec_"
};

int main(int argc, char *argv[])
{
	unsigned char *datas[NUM_TEX_TYPES];
	unsigned char *newImage;

	int i;
	int w = 0, h = 0;
	unsigned char *out;

	for (i = 0; i < NUM_TEX_TYPES; i++)
		datas[i] = NULL;

	for (i = 1; i < argc; i++)
	{
		int j;
		for (j = 0; j < NUM_TEX_TYPES; j++)
		{
			if (strstr(argv[i], substrings[j]))
			{
				int nw, nh, c;
				printf("loading %s\n", argv[i]);
				fflush(stdout);
				datas[j] = stbi_load(argv[i], &nw, &nh, &c, 4);

				if (!datas[j])
				{
					printf("invalid file, ignoring\n");
					break;
				}

				if (w && w != nw)
				{
					printf("size mismatch! %s is %d pixels wide, expected %d", argv[i], nw, w);
					return 0;
				}
				w = nw;

				if (h && h != nh)
				{
					printf("size mismatch! %s is %d pixels high, expected %d", argv[i], nh, h);
					return 0;
				}
				h = nh;

				break;
			}
		}
	}

	newImage = malloc(w * h * 4);

	if (datas[DIFF_TEX])
	{
		printf("writing output.png\n");
		fflush(stdout);
		stbi_write_png("output.png", w, h, 4, datas[DIFF_TEX], w * 4);
	}

	if (datas[NOR_TEX])
	{
		if (datas[DISP_TEX])
		{
			unsigned char *inNormal = datas[NOR_TEX];
			unsigned char *inHeight = datas[DISP_TEX];
			out = newImage;

			for (i = 0; i < w * h; i++)
			{
				out[0] = inNormal[0];
				out[1] = inNormal[1];
				out[2] = inNormal[2];
				out[3] = inHeight[0];
				out += 4;
				inNormal += 4;
				inHeight += 4;
			}

			printf("writing output_n.png\n");
			fflush(stdout);
			stbi_write_png("output_n.png", w, h, 4, newImage, w * 4);
		}
		else
		{
			printf("writing output_n.png\n");
			fflush(stdout);
			stbi_write_png("output_n.png", w, h, 4, datas[NOR_TEX], w * 4);
		}
	}

	if (datas[SPEC_TEX] || datas[ROUGH_TEX] || datas[AO_TEX])
	{
		unsigned char *inSpec = datas[SPEC_TEX];
		unsigned char *inRough = datas[ROUGH_TEX];
		unsigned char *inAO = datas[AO_TEX];
		out = newImage;

		for (i = 0; i < w * h; i++)
		{
			out[0] = datas[SPEC_TEX] ? inSpec[0] : 0;
			out[1] = datas[ROUGH_TEX] ? inRough[1] : 255;
			out[2] = datas[AO_TEX] ? inAO[2] : 255;
			out[3] = 255;
			out += 4;
			inSpec += 4;
			inRough += 4;
			inAO += 4;
		}

		printf("writing output_s.png\n");
		fflush(stdout);
		stbi_write_png("output_s.png", w, h, 4, newImage, w * 4);
	}

	printf("press enter to continue\n");
	fflush(stdout);
	getchar();
}
