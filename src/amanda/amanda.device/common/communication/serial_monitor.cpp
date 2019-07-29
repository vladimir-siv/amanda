#include "serial_monitor.h"

namespace communication
{
	SerialMonitor::Functional SerialMonitor::endl([](void) -> void { Serial.println(); });
}
