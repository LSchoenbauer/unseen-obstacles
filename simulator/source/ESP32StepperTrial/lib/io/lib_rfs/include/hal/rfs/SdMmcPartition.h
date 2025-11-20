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

#include <rfs/FsPartition.h>
#include <utils/Singleton.h>
#include <stddef.h>
#include <inttypes.h>

namespace Hal {
using namespace Rfs;

/** The wrapper class for SD-MMC file systems */
class SdMmcPartition : public Singleton<SdMmcPartition>, public FsPartition {
		typedef Singleton<SdMmcPartition> Base;

	public:
		/** The enumeration of supported SD card types. */
		struct CardType {
				typedef enum {
					/** No partition. */
					None = 0x00,
					/** MMC cards */
					MMC,
					/** SD cards */
					SD,
					/** SDHC cards. */
					SDHC,
					/** Unknown card type. */
					Unknown,
				} Enum;
		};

		virtual FsPartition::PartitionType::Enum GetType() const {
			return FsPartition::PartitionType::SD_MMC_FS;
		}

		virtual bool Mount(const SdMmcMountCfg& cfg);

		/**
		 * Closes the FFS and frees allocated resources.
		 */
		virtual void Unmount();

		/**
		 * Formats the FFS.
		 * @return True upon success, false otherwise.
		 */
		virtual bool Format();

		/**
		 * Provides the type of the SD card.
		 * @return The card type.
		 */
		CardType::Enum GetCardType() const;

		/**
		 * Provides the size of the inserted SD card.
		 * @return The size of the card in bytes.
		 */
		uint64_t GetCardSize() const;

		/**
		 * Provides the total size of  the FFS in bytes.
		 * @return The capacity in bytes.
		 */

		virtual uint64_t GetCapacity() const;

		/**
		 * Provides the consumed size of the FFS in bytes.
		 * @return The occupied size.
		 */
		virtual uint64_t GetUsedSize() const;

	protected:
		virtual ~SdMmcPartition();

	private:
		friend class Singleton<SdMmcPartition> ;
		SdMmcPartition();
};

} // namespace Hal
