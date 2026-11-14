#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/sysctl.h>

#include <cpuinfo/log.h>
#include <openbsd/api.h>

static int sysctl_hw_int(int name, const char* description) {
	int mib[2] = {CTL_HW, name};
	int value = 0;
	size_t value_size = sizeof(value);
	if (sysctl(mib, 2, &value, &value_size, NULL, 0) != 0) {
		cpuinfo_log_error("sysctl(\"%s\") failed: %s", description, strerror(errno));
		value = 0;
	} else if (value <= 0) {
		cpuinfo_log_error("sysctl(\"%s\") returned invalid value %d", description, value);
		value = 0;
	}
	return value;
}

struct cpuinfo_openbsd_topology cpuinfo_openbsd_detect_topology(void) {
	struct cpuinfo_openbsd_topology topology = {
		.packages = 0,
		.cores = 0,
		.threads_per_core = 0,
		.threads = 0,
	};
	/*
	 * OpenBSD does not export the package/core/thread hierarchy to
	 * userland. By default (hw.smt=0) only one thread per core is
	 * online, so the online processors are reported as that many
	 * single-threaded cores in one package.
	 */
	const int online = sysctl_hw_int(HW_NCPUONLINE, "hw.ncpuonline");
	if (online == 0) {
		return topology;
	}
	topology.packages = 1;
	topology.cores = (uint32_t)online;
	topology.threads_per_core = 1;
	topology.threads = (uint32_t)online;
	cpuinfo_log_debug(
		"openbsd topology: packages = %d, cores = %d, "
		"threads_per_core = %d",
		topology.packages,
		topology.cores,
		topology.threads_per_core);
	return topology;
}
