/*
**  $Id: androidvideo.h 7348 2007-08-16 04:53:34Z xgwang $
**  
**  Copyright (C) 2003 ~ 2007 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming.
*/

#ifndef _GAL_androidvideo_h
#define _GAL_androidvideo_h

#include "sysvideo.h"

/* Hidden "this" pointer for the video functions */
#define _THIS	GAL_VideoDevice *this

/* Private display data */

struct GAL_PrivateVideoData {
    int w, h;
    void *buffer;
};

#endif /* _GAL_androidvideo_h */
