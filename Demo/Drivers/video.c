#include <video.h>
#include <mailbox.h>
#include <5x5_font.h>
char loaded = 0;
#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8
int SCREEN_WIDTH;
int SCREEN_HEIGHT;

//mailbuffer must be 16 byte aligned for GPU
unsigned int mailbuffer[22] __attribute__((aligned (16)));
unsigned int* framebuffer;

void initFB(){
	//get the display size
	/*mailbuffer[0] = 8 * 4;		//mailbuffer size
	mailbuffer[1] = 0;		//response code
	mailbuffer[2] = 0x00040003;	//test display size
	mailbuffer[3] = 8;		//value buffer size
	mailbuffer[4] = 0;		//Req. + value length (bytes)
	mailbuffer[5] = 0;		//width
	mailbuffer[6] = 0;		//height
	mailbuffer[7] = 0;		//terminate buffer

	//spam mail the GPU until the response code is ok
	int attempts = 0;
	while(mailbuffer[1] != 0x80000000){
		mailboxWrite((int)mailbuffer, 8);
		mailboxRead(8);

		//if it keeps failing, just set to default and move along
		if(attempts >= 5){
			//I don't bother breaking; just fake the response
			mailbuffer[1] = 0x80000000;
			mailbuffer[5] = 640;
			mailbuffer[6] = 480;
		}

		attempts++;
	}*/

	SCREEN_WIDTH = 1920;//mailbuffer[5];
	SCREEN_HEIGHT = 1080;//mailbuffer[6];

	mailbuffer[0] = 22 * 4;		//mail buffer size
	mailbuffer[1] = 0;		//response code

	mailbuffer[2] = 0x00048003;	//set phys display
	mailbuffer[3] = 8;		//value buffer size
	mailbuffer[4] = 8;		//Req. + value length (bytes)
	mailbuffer[5] = SCREEN_WIDTH;	//screen x
	mailbuffer[6] = SCREEN_HEIGHT;	//screen y

	mailbuffer[7] = 0x00048004;	//set virtual display
	mailbuffer[8] = 8;		//value buffer size
	mailbuffer[9] = 8;		//Req. + value length (bytes)
	mailbuffer[10] = SCREEN_WIDTH;	//screen x
	mailbuffer[11] = SCREEN_HEIGHT; //screen y

	mailbuffer[12] = 0x0048005;	//set depth
	mailbuffer[13] = 4;		//value buffer size
	mailbuffer[14] = 4;		//Req. + value length (bytes)
	mailbuffer[15] = 32;		//bits per pixel
	//pixel format is ARGB, 0xFF0000FF is blue at full alpha transparency

	mailbuffer[16] = 0x00040001;	//allocate buffer
	mailbuffer[17] = 8;		//value buffer size
	mailbuffer[18] = 4;		//Req. + value length (bytes)
	mailbuffer[19] = 0;		//framebuffer address
	mailbuffer[20] = 0;		//framebuffer size

	mailbuffer[21] = 0;		//terminate buffer

	//spam mail the GPU until the response code is ok
	while(mailbuffer[1] != 0x80000000){
		mailboxWrite((int)mailbuffer, 8);
		mailboxRead(8);
	}

	//https://github.com/raspberrypi/firmware/wiki/Accessing-mailboxes
	//shift FB by 0x40000000 if L2 cache is enabled, or 0xC0000000 if disabled
	framebuffer = (unsigned int*)(mailbuffer[19] - 0xC0000000);
}

__attribute__((no_instrument_function))
void drawPixel(unsigned int x, unsigned int y, int colour) {
    framebuffer[y * SCREEN_WIDTH + x] = colour;
}

__attribute__((no_instrument_function))
void drawRect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, int colour) {
    unsigned int i, j = 0;
    for(i = x1; i < x2; i++) {
        for(j = y1; j < y2; j++) {
            drawPixel(i, j, colour);
        }
    }
}

//characters are stored in 5x5_font.h as binary (6x8 font)
//there are 6 bytes which describe 8 pixels in a column
//	{0x7c,	0x24,	0x24,	0x24,	0x7c,	0x00}, // A
//						0
//						0
//	1	1	1	1	1	0
//	1				1	0
//	1				1	0
//	1	1	1	1	1	0
//	1				1	0
//	1				1	0
__attribute__((no_instrument_function))
void drawChar(unsigned char c, int x, int y, int colour){
	int i, j;

	//convert the character to an index
	c = c & 0x7F;
	if (c < ' ') {
		c = 0;
	} else {
		c -= ' ';
	}

	//draw pixels of the character
	for (j = 0; j < CHAR_WIDTH; j++) {
		for (i = 0; i < CHAR_HEIGHT; i++) {
			//unsigned char temp = font[c][j];
			if (font[c][j] & (1<<i)) {
				framebuffer[(y + i) * SCREEN_WIDTH + (x + j)] = colour;
			}
		}
	}
}

__attribute__((no_instrument_function))
void drawString(const char* str, int x, int y, int colour){
	while (*str) {
		drawChar(*str++, x, y, colour);
		x += CHAR_WIDTH; 
	}
}

int position_x = 0;
int position_y = 0;
__attribute__((no_instrument_function))
void println(const char* message, int colour){
	//clear the line and draw the string
	//drawRect(0, position, 420, position + 8, 0x00000000);
	drawString(message, position_x, position_y, colour);
	position_y = position_y + CHAR_HEIGHT + 1;
	if(position_y > SCREEN_HEIGHT){
		position_y = 0;
		position_x += 142;
	}
}

//This is a macro which dumps the CPU registers to the screen
//It MUST be a macro because a call to a function would dirty
//the registers. So this is compiled inline where you 'call'
//it and it saves the registers on the stack, prints them
//and then restores them as if it had never happened.
//SP and PC might be offset by +- 4 bytes? add/sub to correct.
//Should probably push/pop cspr/spsr if needed.
int regs[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char hex[16] = {'0','1','2','3','4','5','6','7',
				'8','9','A','B','C','D','E','F'};
unsigned char messageh[14] = "r#: 0x????????";
int i;
#define dumpDebug()										\
	extern int regs[];									\
	__asm volatile(										\
		"push	{r0-r15}\n"			 					\
		"ldr	r0, =regs		\n"						\
		"ldr	r1, [sp]		\n"	/*r0*/				\
		"str	r1, [r0]		\n"						\
		"ldr	r1, [sp, #4]	\n"	/*r1*/				\
		"str	r1, [r0, #4]	\n"						\
		"ldr	r1, [sp, #8]	\n"	/*r2*/				\
		"str	r1, [r0, #8]	\n"						\
		"ldr	r1, [sp, #12]	\n"	/*r3*/				\
		"str	r1, [r0, #12]	\n"						\
		"ldr	r1, [sp, #16]	\n"	/*r4*/				\
		"str	r1, [r0, #16]	\n"						\
		"ldr	r1, [sp, #20]	\n"	/*r5*/				\
		"str	r1, [r0, #20]	\n"						\
		"ldr	r1, [sp, #24]	\n"	/*r6*/				\
		"str	r1, [r0, #24]	\n"						\
		"ldr	r1, [sp, #28]	\n"	/*r7*/				\
		"str	r1, [r0, #28]	\n"						\
		"ldr	r1, [sp, #32]	\n"	/*r8*/				\
		"str	r1, [r0, #32]	\n"						\
		"ldr	r1, [sp, #36]	\n"	/*r9*/				\
		"str	r1, [r0, #36]	\n"						\
		"ldr	r1, [sp, #40]	\n"	/*r10*/				\
		"str	r1, [r0, #40]	\n"						\
		"ldr	r1, [sp, #44]	\n"	/*r11*/				\
		"str	r1, [r0, #44]	\n"						\
		"ldr	r1, [sp, #48]	\n"	/*r12*/				\
		"str	r1, [r0, #48]	\n"						\
		"ldr	r1, [sp, #52]	\n"	/*r13*/				\
		"str	r1, [r0, #52]	\n"						\
		"ldr	r1, [sp, #56]	\n"	/*r14*/				\
		"str	r1, [r0, #56]	\n"						\
		"ldr	r1, [sp, #60]	\n"	/*r15*/				\
		"str	r1, [r0, #60]	\n"						\
	);													\
	/*dump the registers r0-r12*/						\
	for(i = 0; i < 16; i++){							\
		messageh[1] = hex[i]; /*register number in hex, 	\
		bitshift the integer and take the last 4 bits, 	\
		then converts those to hex characters*/			\
		messageh[6] = hex[(regs[i] >> 28)&0xF];			\
		messageh[7] = hex[(regs[i] >> 24)&0xF];			\
		messageh[8] = hex[(regs[i] >> 20)&0xF];			\
		messageh[9] = hex[(regs[i] >> 16)&0xF];			\
		messageh[10] = hex[(regs[i] >> 12)&0xF];			\
		messageh[11] = hex[(regs[i] >> 8)&0xF];			\
		messageh[12] = hex[(regs[i] >> 4)&0xF];			\
		messageh[13] = hex[(regs[i] >> 0)&0xF];			\
		messageh[14] = 0; /*null termination*/			\
		drawString(messageh, SCREEN_WIDTH - 100, i * 8,	\
											0xFF00FF00);\
	}													\
	__asm volatile(	/*	don't pop r13 or r15*/			\
		"pop	{r0-r12}	\n"							\
		"add 	sp, sp, #4	\n"	/*skip r13 (sp)*/		\
		"pop	{r14}		\n"							\
		"add 	sp, sp, #4	\n"	/*skip r15 (pc)*/		\
	);

__attribute__((no_instrument_function))
void printHex(const char* message, int hexi, int colour){
	char m[42];
	int i = 0;
	while (*message){
		m[i] = *message++;
		i++;
	}
	//overwrite the null terminator
	m[i + 0] = hex[(hexi >> 28)&0xF];
	m[i + 1] = hex[(hexi >> 24)&0xF];
	m[i + 2] = hex[(hexi >> 20)&0xF];
	m[i + 3] = hex[(hexi >> 16)&0xF];
	m[i + 4] = hex[(hexi >> 12)&0xF];
	m[i + 5] = hex[(hexi >> 8)&0xF];
	m[i + 6] = hex[(hexi >> 4)&0xF];
	m[i + 7] = hex[(hexi >> 0)&0xF];
	m[i + 8] = 0; //null termination
	println(m, colour);
}

void videotest(){
	//This loop turns on every pixel the screen size allows for.
	//If the shaded area is larger or smaller than your screen, 
	//you have under/over scan issues. Add disable_overscan=1 to your config.txt
	for(int x = 0; x < SCREEN_WIDTH * SCREEN_HEIGHT; x++){
		framebuffer[x] = 0xFF111111;
	}

	dumpDebug();

	//division crashes the system here but not in other places it seems?
	drawString("Forty-Two", SCREEN_WIDTH / 2 - 4.5 * CHAR_WIDTH, SCREEN_HEIGHT / 2 + CHAR_HEIGHT / 2, 0xFF00FF00);
}
