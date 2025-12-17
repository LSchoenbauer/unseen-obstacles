/*
 * ===============================================================================
 * (c) HTL Leonding
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

#pragma once

#include <FS.h>

using namespace fs;
namespace Rfs {

class FsPartition : public FS {
	public:
		/** The enumeration of supported partition types. */
		struct PartitionType {
				typedef enum {
					/** No partition. */
					None = 0x00,
					/** SPI FFS partition */
					SPI_FFS = 0x01,
					/** SD-MMC partition (typically found on ESP32-Cam) */
					SD_MMC_FS = 0x02,
					/** Any partition. */
					Any = 0xFF,
					/** All partitions. */
					All = 0xFF,
				} Enum;
		};

		/** The common configuration properties for mounting partitions. */
		struct MountCfg {
				/** The root path of the partition. */
				const char* mMountPoint;
				/** True to automatically format the partition if the opening process fails.*/
				bool mFormatOnFail;

				MountCfg(const char* mountPoint = 0, const char* defaultMountPoint = "/", bool formatOnFail = true) :
				        mMountPoint(mountPoint), mFormatOnFail(formatOnFail) {
					if (mMountPoint == 0 || strlen(mMountPoint) == 0) {
						mMountPoint = defaultMountPoint;
					}
				}
		};

		/** The configuration for mounting partitions of type SPI_FFS. */
		struct SpiFfsMountCfg : public MountCfg {
				/** The label of the partition. */
				const char* mPartitionLabel;
				/** The label of the partition. */
				uint8_t mMaxOpenFiles;

				SpiFfsMountCfg(const char* mountPoint = 0, bool formatOnFail = true, const char* partitionLabel = 0,
				        uint8_t maxOpenFiles = 10) :
				        MountCfg(mountPoint, "/spiffs", formatOnFail), mPartitionLabel(partitionLabel),
				        mMaxOpenFiles(maxOpenFiles) {
				}
		};

		/** The configuration for mounting partitions of type SD_MMC. */
		struct SdMmcMountCfg : public MountCfg {
				/** True to enable mode 1, false otherwise. */
				bool mMode1Enabled;

				SdMmcMountCfg(const char* mountPoint = 0, bool formatOnFail = true, bool mode1Enabled = false) :
				        MountCfg(mountPoint, "/sdmmc", formatOnFail), mMode1Enabled(mode1Enabled) {
				}
		};

		/**
		 * Provides the instance of the partition of the given type.
		 * @param type The type of the partition to retrieve.
		 * @return The pointer to the partition or 0 for unsupported
		 * or disabled partition types.
		 */
		static FsPartition* GetPartitionInstance(PartitionType::Enum type);

		/**
		 * Mounts the the partition of type SPI_FFS using the given configuration.
		 * This methods fails on all partition types different to SPI_FFS.
		 * @param cfg The mount configuration.
		 * @return True upon success, false otherwise.
		 */
		virtual bool Mount(const SpiFfsMountCfg& cfg = SpiFfsMountCfg());

		/**
		 * Mounts the partition of type SD_MMC_FS using the given configuration.
		 * This methods fails on all partition types different to SD_MMC_FS.
		 * @param cfg The mount configuration.
		 * @return True upon success, false otherwise.
		 */
		virtual bool Mount(const SdMmcMountCfg& cfg = SdMmcMountCfg());

		/**
		 * Provides the type of the partition.
		 * @return The partition type.
		 */
		virtual PartitionType::Enum GetType() const = 0;

		/**
		 * Closes the FFS and frees allocated resources.
		 */
		virtual void Unmount() = 0;

		/**
		 * Formats the FFS.
		 * @return True upon success, false otherwise.
		 */
		virtual bool Format() = 0;

		/**
		 * Provides the total size of  the FFS in bytes.
		 * @return The capacity in bytes.
		 */

		virtual uint64_t GetCapacity() const = 0;

		/**
		 * Provides the consumed size of the FFS in bytes.
		 * @return The occupied size.
		 */
		virtual uint64_t GetUsedSize() const = 0;

		/**
		 * Dumps the partition tables if supported by the system.
		 */
		static void DumpPartitionTable();

	protected:
		FsPartition(FSImplPtr fsImpl);
		virtual ~FsPartition();
};

} // namespace Rfs;
