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

#include <hal/rfs/SdMmcPartition.h>
#include <hal/rfs/esp32/Esp32FileImplWrapper.h>
#include <utils/Log.h>

#include <memory>

#ifdef RFS_USE_SDMMC
#include <SD_MMC.h>

namespace Hal {
using namespace Rfs;

class Esp32SdMmcImpl : public FSImpl {
	public:
		Esp32SdMmcImpl() {
		}

		virtual ~Esp32SdMmcImpl() {
		}

		virtual FileImplPtr open(const char* path, const char* mode, const bool create = false) override {
			return std::make_shared<Esp32FileImplWrapper>(SD_MMC.open(path, mode, create), "");
		}

		virtual bool exists(const char* path) override {
			return SD_MMC.exists(path);
		}

		virtual bool rename(const char* pathFrom, const char* pathTo) override {
			return SD_MMC.rename(pathFrom, pathTo);
		}

		virtual bool remove(const char* path) override {
			return SD_MMC.remove(path);
		}

		virtual bool mkdir(const char* path) override {
			return SD_MMC.mkdir(path);
		}

		virtual bool rmdir(const char* path) override {
			return SD_MMC.rmdir(path);
		}
};

// ----------------------------------------------------------------

SdMmcPartition::SdMmcPartition() :
        FsPartition(FSImplPtr(new Esp32SdMmcImpl())) {
}

SdMmcPartition::~SdMmcPartition() {
}

bool SdMmcPartition::Mount(const SdMmcMountCfg& cfg) {
	return SD_MMC.begin(cfg.mMountPoint, cfg.mMode1Enabled, cfg.mFormatOnFail);
}

void SdMmcPartition::Unmount() {
	SD_MMC.end();
}

bool SdMmcPartition::Format() {
	LogError("Formatting an SD-Card mounted via SD-MMC is not supported.");
	return false;
}

SdMmcPartition::CardType::Enum SdMmcPartition::GetCardType() const {
	switch (SD_MMC.cardType()) {
		case CARD_NONE:
			return SdMmcPartition::CardType::None;
		case CARD_MMC:
			return SdMmcPartition::CardType::MMC;
		case CARD_SD:
			return SdMmcPartition::CardType::SD;
		case CARD_SDHC:
			return SdMmcPartition::CardType::SDHC;
		case CARD_UNKNOWN:
			return SdMmcPartition::CardType::Unknown;
	}
	return SdMmcPartition::CardType::Unknown;
}

uint64_t SdMmcPartition::GetCardSize() const {
	return SD_MMC.cardSize();
}

uint64_t SdMmcPartition::GetCapacity() const {
	return SD_MMC.totalBytes();
}

uint64_t SdMmcPartition::GetUsedSize() const {
	return SD_MMC.usedBytes();
}

} // namespace Hal

#else
// SD-MMC file system is disabled

namespace Hal {
using namespace Rfs;
SdMmcPartition::SdMmcPartition() : FsPartition( 0 ) {}
SdMmcPartition::~SdMmcPartition() {}
bool SdMmcPartition::Mount(const SdMmcMountCfg& cfg) { return false; }
void SdMmcPartition::Unmount() {}
bool SdMmcPartition::Format() { return false; }
SdMmcPartition::CardType::Enum SdMmcPartition::GetCardType() const { return SdMmcPartition::CardType::Unknown; }
uint64_t SdMmcPartition::GetCardSize() const { return 0; }
uint64_t SdMmcPartition::GetCapacity() const { return 0; }
uint64_t SdMmcPartition::GetUsedSize() const { return 0; }

} // namespace Hal

#endif // RFS_USE_SDMMC