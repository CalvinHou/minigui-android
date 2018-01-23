/*
** $Id: androidial.h 7335 2007-08-16 03:38:27Z xgwang $
**
** androidial.h:. the head file of Android IAL Engine.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Created by Wei YongMing, 2000/09/13
*/

#ifndef GUI_IAL_ANDROID_H
    #define GUI_IAL_ANDROID_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitAndroidInput (INPUT* input, const char* mdev, const char* mtype);
void    TermAndroidInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_ANDROID_H */


