#include "serial_monitor.h"

namespace communication
{
	SerialMonitor::Functional SerialMonitor::endl([](void) -> size_t { return Serial.println(); });
}
