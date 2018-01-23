/*
**  $Id: androidvideo.c 8944 2007-12-29 08:29:16Z xwyan $
**  
**  Copyright (C) 2003 ~ 2007 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming.
*/

/* Dummy GAL video driver implementation; this is just enough to make an
 *  GAL-based application THINK it's got a working video driver, for
 *  applications that call GAL_Init(GAL_INIT_VIDEO) when they don't need it,
 *  and also for use as a collection of stubs when porting GAL to a new
 *  platform for which you haven't yet written a valid video driver.
 *
 * This is also a great way to determine bottlenecks: if you think that GAL
 *  is a performance problem for a given platform, enable this driver, and
 *  then see if your application runs faster without video overhead.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"

#ifdef _MGGAL_ANDROID
#include <jni.h>

#include "androidvideo.h"


#define ANDROIDVID_DRIVER_NAME "android"

/* Initialization/Query functions */
static int ANDROID_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **ANDROID_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *ANDROID_SetVideoMode(_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int ANDROID_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void ANDROID_VideoQuit(_THIS);

/* Hardware surface functions */
static int ANDROID_AllocHWSurface(_THIS, GAL_Surface *surface);
static void ANDROID_FreeHWSurface(_THIS, GAL_Surface *surface);

/* ANDROID driver bootstrap functions */

static int ANDROID_Available(void)
{
    return(1);
}

static void ANDROID_DeleteDevice(GAL_VideoDevice *device)
{
    free(device->hidden);
    free(device);
}

static GAL_VideoDevice *ANDROID_CreateDevice(int devindex)
{
    GAL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = (GAL_VideoDevice *)malloc(sizeof(GAL_VideoDevice));
    if ( device ) {
        memset(device, 0, (sizeof *device));
        device->hidden = (struct GAL_PrivateVideoData *)
                malloc((sizeof *device->hidden));
    }
    if ( (device == NULL) || (device->hidden == NULL) ) {
        GAL_OutOfMemory();
        if ( device ) {
            free(device);
        }
        return(0);
    }
    memset(device->hidden, 0, (sizeof *device->hidden));

    /* Set the function pointers */
    device->VideoInit = ANDROID_VideoInit;
    device->ListModes = ANDROID_ListModes;
    device->SetVideoMode = ANDROID_SetVideoMode;
    device->SetColors = ANDROID_SetColors;
    device->VideoQuit = ANDROID_VideoQuit;
#ifndef _MGRM_THREADS
    device->RequestHWSurface = NULL;
#endif
    device->AllocHWSurface = ANDROID_AllocHWSurface;
    device->CheckHWBlit = NULL;
    device->FillHWRect = NULL;
    device->SetHWColorKey = NULL;
    device->SetHWAlpha = NULL;
    device->FreeHWSurface = ANDROID_FreeHWSurface;

    device->free = ANDROID_DeleteDevice;

    return device;
}

VideoBootStrap ANDROID_bootstrap = {
    ANDROIDVID_DRIVER_NAME, "Android video driver",
    ANDROID_Available, ANDROID_CreateDevice
};


static int ANDROID_VideoInit(_THIS, GAL_PixelFormat *vformat)
{
    fprintf (stderr, "NEWGAL>ANDROID: Calling init method!\n");

    /* Determine the screen depth (use default 8-bit depth) */
    /* we change this during the GAL_SetVideoMode implementation... */
/*
    vformat->BitsPerPixel = 8;
    vformat->BytesPerPixel = 1;
*/
    vformat->BitsPerPixel = 16;
    vformat->BytesPerPixel = 2;

    switch (vformat->BitsPerPixel) {
        case 16:
            vformat->BytesPerPixel = 2;
            vformat->Rmask = 0x0000F800;
            vformat->Gmask = 0x000007E0;
            vformat->Bmask = 0x0000001F;
            break;
        case 32:
            vformat->BytesPerPixel = 4;
            vformat->Amask = 0xFF000000;
            vformat->Rmask = 0x00FF0000;
            vformat->Gmask = 0x0000FF00;
            vformat->Bmask = 0x000000FF;
            break;
        default:
            GAL_SetError ("NEWGAL>Android: Not supported depth: %d, "
                "please try to use Shadow NEWGAL engine with targetname android.\n", vformat->BitsPerPixel);
            return -1;
    }



    /* We're done! */
    return(0);
}

static GAL_Rect **ANDROID_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    if (format->BitsPerPixel < 8) {
        return NULL;
    }

    return (GAL_Rect**) -1;
}


void* __mg_android_buffer = NULL;
jobject __mg_bitmap;
JNIEnv* __mg_env;

static GAL_Surface *ANDROID_SetVideoMode(_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    int pitch;


    pitch = width * ((bpp + 7) / 8);
    pitch = (pitch + 3) & ~3;

    this->hidden->buffer = __mg_android_buffer;

if (__mg_android_buffer == NULL) {
    if (this->hidden->buffer) {
        free (this->hidden->buffer);
    }

    pitch = width * ((bpp + 7) / 8);
    pitch = (pitch + 3) & ~3;

    this->hidden->buffer = malloc (pitch * height);
    if (!this->hidden->buffer) {
        fprintf (stderr, "NEWGAL>ANDROID: "
                "Couldn't allocate buffer for requested mode\n");
        return NULL;
    }

    memset (this->hidden->buffer, 0, pitch * height);
}
    if (!this->hidden->buffer) {
        fprintf (stderr, "NEWGAL>ANDROID: "
                "Couldn't allocate buffer for requested mode\n");
        return NULL;
    }

    LOGE("buff width:%d, height:%d", width, height);

    memset (this->hidden->buffer, 0x90, pitch * height);

    /* Allocate the new pixel format for the screen */
    if (!GAL_ReallocFormat (current, bpp, 0, 0, 0, 0)) {
        free(this->hidden->buffer);
        this->hidden->buffer = NULL;
        fprintf (stderr, "NEWGAL>ANDROID: "
                "Couldn't allocate new pixel format for requested mode\n");
        return(NULL);
    }

    /* Set up the new mode framebuffer */
    current->flags = flags & GAL_FULLSCREEN;
    this->hidden->w = current->w = width;
    this->hidden->h = current->h = height;
    current->pitch = pitch;
    current->pixels = this->hidden->buffer;

    /* We're done */
    return(current);
}

/* We don't actually allow hardware surfaces other than the main one */
static int ANDROID_AllocHWSurface(_THIS, GAL_Surface *surface)
{
    return(-1);
}
static void ANDROID_FreeHWSurface(_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}

static int ANDROID_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors)
{
    /* do nothing of note. */
    return(1);
}

/* Note:  If we are terminated, this could be called in the middle of
   another video routine -- notably UpdateRects.
*/
static void ANDROID_VideoQuit(_THIS)
{
    if (this->screen->pixels != NULL)
    {
	if (__mg_bitmap != NULL) {
           AndroidBitmap_unlockPixels(__mg_env, __mg_bitmap);
	}
        this->screen->pixels = NULL;
    }
}

JNIEXPORT jstring JNICALL
Java_com_example_houhuihua_myapplication_MainActivity_setAndroidGALBuffer( JNIEnv* env,
                                                  jobject object,
                                                  jobject bitmap)
{
    printf("it is only a test%d", 100);

/*
    // Allocate native pixels buffer

    AndroidBitmapInfo bitmapInfo;
    AndroidBitmap_getInfo(env, bitmap, &bitmapInfo);

    int w = bitmapInfo.width;
    int h = bitmapInfo.height;
    int buffer_size = w * h * 4;
    uint8_t* buffer = malloc (buffer_size);

    // Copy pixels to native buffer

    void* bitmapPixels;
    AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels)

    memcpy((void*)buffer, bitmapPixels, buffer_size);

    AndroidBitmap_unlockPixels(env, bitmap);
*/
    __mg_bitmap = bitmap;
    __mg_env = env;

    AndroidBitmap_lockPixels(env, bitmap, &__mg_android_buffer);

    return (*env)->NewStringUTF(env, "Hello from JNI !  Compiled with ABI, __mg_android_buffer.");
}

JNIEXPORT jstring JNICALL
Java_com_example_houhuihua_myapplication_MainActivity_fillAndroidGALBuffer( JNIEnv* env,
                                                  jobject object
)
{
    memset ((char*)__mg_android_buffer, 0x95, 20000);

    LOGE("log:fillAndroidGALBuffer");

    return (*env)->NewStringUTF(env, "Hello from JNI !  Compiled with ABI, fillAndroidGALBuffer.");
}


#endif /* _MGGAL_ANDROID */
