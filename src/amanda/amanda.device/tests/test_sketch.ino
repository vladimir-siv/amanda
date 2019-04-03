const int _ETH_CS = 53;
const int _SD_CS = 4;

extern void _setup_ethernet_server(void);
extern void _check_for_ethernet_client(void);

extern void _sd_check(void);
extern void _sd_file_read(const char* const);

extern void _sd_xml(void);

void setup(void)
{
	Serial.begin(9600);
	while (!Serial);
	Serial.flush();

	_setup_ethernet_server();
	Serial.println();
	_sd_check();
	Serial.println();
	_sd_file_read("core.xsa");
	Serial.println();
	_sd_xml();
	Serial.println();
}

void loop(void)
{
	_check_for_ethernet_client();
}
