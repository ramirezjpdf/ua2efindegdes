#include <jsni.h>
#include<android/log.h>
#include<andorid/bitmap.h>

#define LOG_TAG "libua2efindedges"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA__ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

typedef struct
{
	uint8_t alpha;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} argb;

JNIEXPORT void JNICALL Java_com_example_ua2efindedges_UA2EFindEdges_converttogray(JNIEnv* env, jobject obj, jobject bitmapcolor, jobject bitmapgray)
{
	AndroidBitmapInfo infocolor;
	void* pixelscolor;
	AndroidBitmapInfo infogray;
	void* pixelsgray;
	int ret;
	int y;
	int x;

	if ((ret = AndroidBitmap_getInfo(env, bitmapcolor, &infocolor)) < 0)
	{
		LOGE("AndridBitmap_getInfo(bitmapcolor) failed! error=%d", ret);
		return;
	}

	if((ret = AndroidBitmap_getInfo(env, bitmapgray, &infogray)) < 0)
	{
		LOGE("AndroidBitmap_getInfo(bitmapgray) falied! error=%d", ret);
		return;
	}

	LOGI("color image :: width is %d; height is %d; stride is %d; format is %d; flags is %d", infocolor.width, infocolor.height, infocolor.stride, infocolor.format, infocolor.flags);
	if(infocolor.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
	{
		LOGE("Bitmap from bitmapcolor format is not RGBA_8888!");
		return;
	}

	LOGI("gray image :: width is %d; height is %d; stride is %d; format is %d; flags is %d", infogray.width, infogray.height, infogray.stride, infogray.format, infogray.flags);
	if(infogray.format != ANDROID_BITMAP_FORMAT_A_8888)
	{
		LOGE("Bitmap from bitmapgray format is not A_8888!");
		return;
	}

	if((ret = AndroidBitmap_lockPixels(env, bitmapcolor, &pixelscolor)) < 0)
	{
		LOGE("AndroidBitmap_lockPixels for bitmapcolor failed! error=%d", ret);
	}
	if((ret = AndroidBitmap_lockPixels(env, bitmapgray, &pixelsgray)) < 0)
	{
		LOGE("AndroidBitmap_lockPixels for bitmapgray failed! ereor=%d", ret);
	}

	//finally image processing algorithm for modifying pixels
	for(y = 0; y < infocolor.height; i++)
	{
		argb* line = (argb*) pixelscolor;
		uint8_t* grayline = (uint8_t) pixelsgray;
		for(x = 0; x < infocolor.width; x++)
		{
			grayline[x] = 0.3 * line[x].red + 0.59 * line[x].green + 0.11 * line[x].blue;
		}

		pixelscolor = (char*)pixelscolor + infocolor.stride;
		pixelsgray = (char*)pixelsgray + infogray.stride;
	}

	AndroidBitmap_unlockPixels(env, bitmapcolor);
	AndroidBitmap_unlockPixels(env, bitmapgray);
}

JNIEXPORT void JNICALL Java_com_example_ua2efindedges_UA2EFindEdges_detectedges(JNIEnv* env, jobject obj, jobject bitmapgray, jobject bitmapedges)
{
	AndroidBitmapInfo infogray;
	void* pixelsgray;
	AndroidBitmapInfo infoedges;
	void* pixelsedge;
	int ret;
	int y;
	int x;
	int sumX, sumY, sum;
	int i, j;
	int Gx[3][3];
	int Gy[3][3];
	uint8_t* graydata;
	uint8_t* edgedata;

	Gx[0][0] = -1;Gx[0][1] = 0;Gx[0][2] = 1;
	Gx[1][0] = -2;Gx[1][1] = 0;Gx[1][2] = 2;
	Gx[2][0] = -1;Gx[2][1] = 0;Gx[2][2] = 1;

	Gy[0][0] = 1;Gy[0][1] = 2;Gy[0][2] = 1;
	Gy[1][0] = 0;Gy[1][1] = 0;Gy[1][2] = 0;
	Gy[2][0] = -1;Gy[2][1] = -2;Gy[2][2] = -1;

	LOGI("detectedges in JNI code");

	if((ret = AndroidBitmap_getInfo(env, bitmapgray, &infogray)) < 0)
	{
		LOGE("AndroidBitmap_getInfo(bitmapgray) falied! error=%d", ret);
		return;
	}
	if((ret = AndroidBitmap_getInfo(env, bitmapedges, &infoedges)) < 0)
	{
		LOGE("AndroidBitmap_getInfo(bitmapedges) falied! error=%d", ret);
		return;
	}

	LOGI("gray image :: width is %d; height is %d; stride is %d; format is %d; flags is %d", infogray.width, infogray.height, infogray.stride, infogray.format, infogray.flags);
	if(infogray.format != ANDROID_BITMAP_FORMAT_A_8888)
	{
		LOGE("Bitmap from bitmapgray format is not A_8888!");
		return;
	}
	LOGI("edge image :: width is %d; height is %d; stride is %d; format is %d; flags is %d", infoedges.width, infoedges.height, infoedges.stride, infoedges.format, infoedges.flags);
	if(infoedges.format != ANDROID_BITMAP_FORMAT_A_8888)
	{
		LOGE("Bitmap from bitmapedges format is not A_8888!");
		return;
	}

	if((ret = AndroidBitmap_lockPixels(env, bitmapgray, &pixelsgray)) < 0)
	{
		LOGE("AndroidBitmap_lockPixels for bitmapgray failed! error=%d", ret);
	}
	if((ret = AndroidBitmap_lockPixels(env, bitmapedges, &pixelsedge)) < 0)
	{
		LOGE("AndroidBitmap_lockPixels for bitmapedges failed! error=%d", ret);
	}

	//finally the image processing algorithm for modifying the image
	graydata = (uint8_t*) pixelsgray;
	edgedata = (uint8_t*) pixelsedge;

	for(y = 0;y < infogray.height - 1; y++)
	{
		for(x = 0; x < infogray.width; x++)
		{
			sumX = 0;
			sumY = 0;
			//check boundaries
			if(y == 0 || y == infogray.height - 1)
			{

			}
		}
	}
}
