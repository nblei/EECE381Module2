
#include "Global.h"

bool loadSDCard(alt_up_sd_card_dev* device) {
	if((device = alt_up_sd_card_open_dev("/dev/SD_Card")) != NULL) {
		if (alt_up_sd_card_is_Present()) {
			printf("Card connected.\n");
			if (alt_up_sd_card_is_FAT16()) {
				printf("FAT16 file system detected.\n");
				return true;
			} else {
				printf("Unknown file system.\n");
			}
		}
	}
	return false;
}

void initAnimate(struct Cursor* cursor) {
	int timer = 2000000;
	IOWR_16DIRECT(TIMESTAMP_BASE, 8, timer & 0xFFFF);
	IOWR_16DIRECT(TIMESTAMP_BASE, 12, timer >> 16);
	IOWR_16DIRECT(TIMESTAMP_BASE, 4, 0x07);
	alt_irq_register(TIMESTAMP_IRQ, cursor, (void*)animate_ISR);
}

void initAudioBuffer() {
	int timer = 5000000;
	IOWR_16DIRECT(AUDIOBUFFERPROCESS_BASE, 8, timer & 0xFFFF);
	IOWR_16DIRECT(AUDIOBUFFERPROCESS_BASE, 12, timer >> 16);
	IOWR_16DIRECT(AUDIOBUFFERPROCESS_BASE, 4, 0x07);
	alt_irq_register(AUDIOBUFFERPROCESS_IRQ, NULL, (void*)mix_ISR);

}
int main()
{
	//SD device initialization
	up_dev.device_reference = NULL;
	while(!loadSDCard(up_dev.device_reference)) {
		printf("SD card is not connected.\n");
	}

	initVGA();
	initAudioDeviceController();
	initSoundMixer();

	struct CmdScheduler* scheduler = initCmdScheduler();

	//Serial device initialization
	com = initRS232(scheduler);

	initPushKeyController();

	initDatabase();
	initMemory();

	//sync database
	update();

	//Test VGA Output
	/*struct Image* testImg;
	//struct Image* testImg1;
	while((testImg = loadSDImage("TEST.BMP")) == NULL);
	//while((testImg1 = loadSDImage("ART3.BMP")) == NULL);
	draw(35, 35, testImg);
	//draw(0, 20, testImg1);
	killImage(testImg);
	alt_up_char_buffer_string(char_buffer, "Initialization Completed", 27, 5);
	//graphicTester();*/
	//Test End

	struct Cursor* cursor = initCursor(10, 100);
	initAudioBuffer();
	initAnimate(cursor);

	int i = 2;
	syncPlay(1, 100, 0);

	while(1) {
		cmdProcessing(scheduler);
		//updateMixer();

		i = soundTester(i);

	}


	return 0;
}
