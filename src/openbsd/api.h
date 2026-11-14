#pragma once

#include <stdint.h>

struct cpuinfo_openbsd_topology {
	uint32_t packages;
	uint32_t cores;
	uint32_t threads;
	uint32_t threads_per_core;
};

struct cpuinfo_openbsd_topology cpuinfo_openbsd_detect_topology(void);
