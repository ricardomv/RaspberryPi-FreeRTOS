#ifndef _VIDEO_H_
#define _VIDEO_H_

void initFB();
void drawChar(unsigned char c, int x, int y, int color);
void drawString(const char* str, int x, int y, int color);
void videotest();

#endif
