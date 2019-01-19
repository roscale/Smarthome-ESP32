#include "helper.hpp"

String createDiscoveryMessage(const char* UUID, const char* name, int powerState) {
	String s = "{";
	s += R"("uuid": ")";
	s += UUID;
	s += R"(", )";
    s += R"("name": ")";
    s += name;
    s += R"(", )";
	s += R"("power_state": )";
	s += powerState;
	s += "}";
	return s;
}