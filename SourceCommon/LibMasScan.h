// MasScan.h

#ifndef LIB_MASSCAN_HEADER
#define LIB_MASSCAN_HEADER

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

	struct MasScanReport {
		unsigned long countHost     = 0;
		unsigned long countPort     = 0;
		unsigned long countHostOpen = 0;
		unsigned long countPortOpen = 0;
		int errCode = 0;
	};

	int LaunchMasScan(
		const char* Command, 
		const char* outDir,
		const char* uniqueName,
		struct MasScanReport* MSR);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // #ifndef LIB_MASSCAN_HEADER
