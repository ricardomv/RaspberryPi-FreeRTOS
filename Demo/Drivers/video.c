#include "video.h"
#include "5x5_font.h"

#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8
#define SCREEN_X 1920
#define SCREEN_Y 1080

extern void PUT32(int dest, int src);
extern int GET32(int src);

unsigned int mailbuffer[22] __attribute__((aligned (16)));
unsigned int* framebuffer;

void MailboxWrite(int data_addr, int channel){
	int mailbox = 0x3f00B880;
	while(1){
		if((GET32(mailbox + 0x18)&0x80000000) == 0) break;
	}
	PUT32(mailbox + 0x20, data_addr + channel);
	return;
}

int MailboxRead(int channel){
	int ra;
	int mailbox = 0x3f00B880;
	while(1){
		while(1){
			ra = GET32(mailbox + 0x18);
			if((ra&0x40000000) == 0) break;
		}
		ra = GET32(mailbox + 0x00);
		if((ra&0xF) == channel) break;
	}
	return(ra);
}

void initFB(){
	//good docuentation on the mailbox functions
	//https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface

	mailbuffer[0] = 22 * 4; //mail buffer size
	mailbuffer[1] = 0; //response code

	mailbuffer[2] = 0x00048003; //set phys display
	mailbuffer[3] = 8; //value buffer size
	mailbuffer[4] = 8; //Req. + value length (bytes)
	mailbuffer[5] = SCREEN_X; //screen x
	mailbuffer[6] = SCREEN_Y; //screen y

	mailbuffer[7] = 0x00048004; //set virtual display
	mailbuffer[8] = 8; //value buffer size
	mailbuffer[9] = 8; //Req. + value length (bytes)
	mailbuffer[10] = SCREEN_X; //screen x
	mailbuffer[11] = SCREEN_Y; //screen y

	mailbuffer[12] = 0x0048005; //set depth
	mailbuffer[13] = 4; //value buffer size
	mailbuffer[14] = 4; //Req. + value length (bytes)
	mailbuffer[15] = 32; //bits per pixel
	//pixel format is ARGB, 0xFF0000FF is blue at full alpha transparency

	mailbuffer[16] = 0x00040001; //allocate buffer
	mailbuffer[17] = 8; //value buffer size
	mailbuffer[18] = 4; //Req. + value length (bytes)
	mailbuffer[19] = 0; //framebuffer address
	mailbuffer[20] = 0; //framebuffer size

	mailbuffer[21] = 0; //terminate buffer

	//spam mail the GPU until the response code is ok
	while(mailbuffer[1] != 0x80000000){
		MailboxWrite((int)mailbuffer, 8);
		MailboxRead(8);
	}

	//the framebuffer pointer is returned as a physical address,
	//subtracting 0xC0000000 converts it to virtual/
	framebuffer = (unsigned int*)(mailbuffer[19] - 0xC0000000);
}

void DrawChar(unsigned char c, int x, int y, int color) {
	int i, j;

	// Convert the character to an index
	c = c & 0x7F;
	if (c < ' ') {
		c = 0;
	} else {
		c -= ' ';
	}

	// Draw pixels
	for (j = 0; j < CHAR_WIDTH; j++) {
		for (i = 0; i < CHAR_HEIGHT; i++) {
			unsigned char temp = font[c][j];
			if (temp & (1<<i)) {
				framebuffer[(y + i) * SCREEN_X + (x + j)] = color;
			}
		}
	}
}

void DrawString(const char* str, int x, int y, int color) {
	while (*str) {
		DrawChar(*str++, x, y, color);
		x += CHAR_WIDTH;
	}
}

void videotest(){
	//add disable_overscan=1 to your config.txt if you have under/over scan issues
	/*int x;
	for(x = 0; x < SCREEN_X * SCREEN_Y; x++){
		framebuffer[x] = 0x00000000;
	}*/
	DrawString("Forty-Two", 0, 0, 0xFF00FF00);
}
