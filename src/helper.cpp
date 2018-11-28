#include "helper.hpp"

String createDiscoveryMessage(const char* UUID, int powerState) {
	String s = "{";
	s += "'uuid': '";
	s += UUID;
	s += "',";
	s += "'power_state': ";
	s += powerState;
	s += "}";
	return s;
}