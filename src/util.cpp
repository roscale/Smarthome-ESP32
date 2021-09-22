#include <sstream>
#include <structures/GlobalConfig.hpp>
#include "util.hpp"
#include "constants.hpp"

std::string createDiscoveryJson() {
	auto& cfg = GlobalConfig::instance();

	std::stringstream ss;
	ss << "{"
	   << R"("uuid": ")"
	   << UUID
	   << R"(", )"
	   << R"("name": ")"
	   << cfg.name
	   << R"(", )"
	   << R"("power": )"
	   << (cfg.power ? "true" : "false")
	   << "}";

	return ss.str();
}

std::string createWiFiInfoJson() {
	auto& cfg = GlobalConfig::instance();

	std::stringstream ss;
	ss << "{"
	   << R"("ssid": ")"
	   << cfg.ssid
	   << R"(", )"
	   << R"("psk": ")"
	   << cfg.psk
	   << R"(")"
	   << "}";

	return ss.str();
}