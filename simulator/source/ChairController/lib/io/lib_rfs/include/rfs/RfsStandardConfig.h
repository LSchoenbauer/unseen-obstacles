/*
 * ===============================================================================
 * c) HTL Leonding
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Licensed under MIT License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the license.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * All trademarks used in this document are property of their respective owners.
 * ===============================================================================
 */

/**
 * This is the standard configuration for LeoIoT Root File System (RFS) library.
 * 
 * Native file systems or partitions are mounted at specific mounts at the RFS.
 * Entry point to the RFS is the singleton instance of class `rfs/RootFileSystem`.
 * 
 * Note: no native file system is enabled by default.
 * + To enable SPIFFS set build flag `RFS_USE_SPIFFS`. This is the default file system
 *   for ESP32.
 * + To enabled SD/MMC card file system set build flag `RFS_USE_SDMMC`. For example
 *   ESP32-CAM supports SD/MMC cards out of the box.
 * 
 * Debug logging specific for RFS can be enabled with build flag `LOG_RFS_DBG_ENABLED`.
*/

#pragma once

#include <stddef.h>
#include <inttypes.h>

struct CStrMapNode {
		const char* origin;
		const char* destination;
};

#ifdef RFS_USE_SPIFFS
	#pragma message("SPIFFS file system is enabled (remove build flag: 'RFS_USE_SPIFFS' to disable it)")
	struct StdSpiFfsConfig {
			// mount point must start with '/'
			const char* mountPoint = "/ffs";
			const bool formatOnFail = true;
			const uint8_t maxOpenFiles = 10;
			const char* partitionLabel = "spiffs"; // MUST! match partition name defined in partition table!
	};
#else
	#pragma message("SPIFFS file system is disabled, set build flag: 'RFS_USE_SPIFFS' to enable it")
#endif

#ifdef RFS_USE_SDMMC
	#pragma message("SD-MMC file system is enabled (remove build flag: 'RFS_USE_SDMMC' to disable it)")
	struct StdSdMmcConfig {
			// mount point must start with '/'
			const char* mountPoint = "/sdmmc";
			const bool formatOnFail = true;
			const bool mode1Bit = false;
	};
#else
	#pragma message("SD-MMC file system is disabled, set build flag: 'RFS_USE_SDMMC' to enable it")
#endif

struct StdRfsConfig {
#ifdef RFS_USE_SPIFFS
		const StdSpiFfsConfig spiffs;
#endif
#ifdef RFS_USE_SDMMC
		const StdSdMmcConfig sd_mmc;
#endif
		const size_t pathMapSize = 1;
		const CStrMapNode pathMap[1] = { { "/nvs", "/ffs/nvs" } };
} gStdRfsCfg;

