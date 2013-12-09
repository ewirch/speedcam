#include <stdio.h>
#include <unistd.h>
#include "camera.h"
#include "graphics.h"
#include "sensor.h"

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 512

#define MAIN_TEXTURE_WIDTH 512
#define MAIN_TEXTURE_HEIGHT 512

unsigned char blackScreen[MAIN_TEXTURE_WIDTH*MAIN_TEXTURE_HEIGHT*4];

void paint_frame(GfxTexture &texture) {
	float aspect_ratio = float(MAIN_TEXTURE_WIDTH)/float(MAIN_TEXTURE_HEIGHT);
	float screen_aspect_ratio = (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT;
	BeginFrame();
//	DrawTextureRect(&texture,
//			-aspect_ratio/screen_aspect_ratio,  1.f,
//			aspect_ratio/screen_aspect_ratio, -1.f);
	DrawTextureRect(&texture,-1,1,1,-1);
	EndFrame();
}

void initBlackScreen() {
	for (int i = 0; i < MAIN_TEXTURE_WIDTH; i++) {
		for (int j= 0; j < MAIN_TEXTURE_HEIGHT; j++) {
			blackScreen[(i*MAIN_TEXTURE_HEIGHT+j)*4] = 0;
			blackScreen[(i*MAIN_TEXTURE_HEIGHT+j)*4+1] = 0;
			blackScreen[(i*MAIN_TEXTURE_HEIGHT+j)*4+2] = 0;
			blackScreen[(i*MAIN_TEXTURE_HEIGHT+j)*4+3] = 255;
		}
	}
}

//entry point
int main(int argc, const char **argv)
{
	bool do_argb_conversion = true;
	int num_levels = 1;

	initBlackScreen();

	//init graphics and the camera
	InitGraphics();
	CCamera* cam = StartCamera(MAIN_TEXTURE_WIDTH, MAIN_TEXTURE_HEIGHT, 30, num_levels, do_argb_conversion);

	if (cam) {
		connectDistanceSensor();

		GfxTexture texture;
		texture.Create(MAIN_TEXTURE_WIDTH, MAIN_TEXTURE_HEIGHT);

		printf("Running frame loop\n");



		for(int i = 0; i < 3000; i++)
		{
			const void* frame_data; int frame_sz;

			texture.SetPixels(blackScreen);
			paint_frame(texture);
			blockUntilDistanceChanges();
			if(cam->BeginReadFrame(0,frame_data,frame_sz))
			{
//				for (int i = 0; i < 100; i++) {
//					printf("%i ", (int)(((unsigned char*)frame_data)[i]));
//				}
//				printf("\n");
//				for (int i = 0; i < MAIN_TEXTURE_WIDTH; i++) {
//						for (int j= 0; j < MAIN_TEXTURE_HEIGHT; j++) {
//							((unsigned char*)frame_data)[(i+j)*4] = 0;
//							((unsigned char*)frame_data)[(i+j)*4+1] = 0;
//							((unsigned char*)frame_data)[(i+j)*4+2] = 0;
//							((unsigned char*)frame_data)[(i+j)*4+3] = 255;
//						}
//					}
				texture.SetPixels(frame_data);
				cam->EndReadFrame(0);
			}
			paint_frame(texture);

//			sleep(3);
		}


		closeDistanceSensor();
		StopCamera();
	}
}
