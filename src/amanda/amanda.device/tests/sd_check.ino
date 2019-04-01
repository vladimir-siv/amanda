#include <SPI.h>
#include <SD.h>

extern const int _SD_CS;

Sd2Card card;
SdVolume volume;
SdFile root;
void _sd_check(void)
{
	pinMode(_SD_CS, OUTPUT);

	Serial.print(F("Initializing SD card..."));

	if (!card.init(SPI_HALF_SPEED, _SD_CS))
	{
		Serial.println(F(" Initialization failed. Things to check:"));
		Serial.println(F("* is a card inserted?"));
		Serial.println(F("* is your wiring correct?"));
		Serial.println(F("* did you change the chipSelect pin to match your shield or module?"));
		while (1);
	}
	else Serial.println(F(" Wiring is correct and a card is present."));

	Serial.println();
	Serial.print(F("Card type:         "));
	switch (card.type())
	{
		case SD_CARD_TYPE_SD1: Serial.println(F("SD1")); break;
		case SD_CARD_TYPE_SD2: Serial.println(F("SD2")); break;
		case SD_CARD_TYPE_SDHC: Serial.println(F("SDHC")); break;
		default: Serial.println(F("Unknown"));
	}

	// Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
	if (!volume.init(card))
	{
		Serial.println(F("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card"));
		while (1);
	}

	Serial.print(F("Clusters:          "));
	Serial.println(volume.clusterCount());
	Serial.print(F("Blocks x Cluster:  "));
	Serial.println(volume.blocksPerCluster());

	Serial.print(F("Total Blocks:      "));
	Serial.println(volume.blocksPerCluster() * volume.clusterCount());
	Serial.println();

	// print the type and size of the first FAT-type volume
	uint32_t volumesize;
	Serial.print(F("Volume type is:    FAT"));
	Serial.println(volume.fatType(), DEC);

	volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
	volumesize *= volume.clusterCount();       // we'll have a lot of clusters
	volumesize /= 2;                           // SD card blocks are always 512 bytes (2 blocks are 1KB)
	Serial.print(F("Volume size (Kb):  "));
	Serial.println(volumesize);
	Serial.print(F("Volume size (Mb):  "));
	volumesize /= 1024;
	Serial.println(volumesize);
	Serial.print(F("Volume size (Gb):  "));
	Serial.println((float)volumesize / 1024.0);

	Serial.println(F("\nFiles found on the card (name, date and size in bytes): "));
	root.openRoot(volume);

	// list all files in the card with date and size
	root.ls(LS_R | LS_DATE | LS_SIZE);
}

void _sd_file_read(const char* const filepath)
{
	pinMode(_SD_CS, OUTPUT);

	Serial.print(F("Initializing SD card..."));

	if (!SD.begin(_SD_CS))
	{
		Serial.println(F(" Initialization failed!"));
		while (1);
	}
	Serial.println(F(" Initialization done."));

	File file = SD.open(filepath);
	if (file)
	{
		Serial.print(filepath);
		Serial.println(':');

		while (file.available())
		{
			Serial.write(file.read());
		}

		Serial.println();

		Serial.print(F("~EOF:"));
		Serial.println(filepath);
		
		file.close();
	}
	else
	{
		Serial.print(F("Error opening "));
		Serial.println(filepath);
	}
}
