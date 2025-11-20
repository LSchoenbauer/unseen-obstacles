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

#include <hal/rfs/SpiFfsPartition.h>
#include <hal/rfs/esp32/Esp32FileImplWrapper.h>
#include <utils/Log.h>
#include <rfs/RfsLogger.h>

#include <memory>

#ifdef RFS_USE_SPIFFS
#include <FSImpl.h>
#include <SPIFFS.h>

namespace Hal {
using namespace Rfs;

class Esp32SpiFfsImpl : public FSImpl {
	public:
		Esp32SpiFfsImpl() {
		}

		virtual ~Esp32SpiFfsImpl() {
		}

		virtual FileImplPtr open(const char* path, const char* mode, const bool create = false) override {
			RfsLogDbg("-- opening SPIFFS file: '%s' [%s]", path, mode);
			return std::make_shared<Esp32FileImplWrapper>(SPIFFS.open(path, mode, create), "");
		}

		virtual bool exists(const char* path) override {
			return SPIFFS.exists(path);
		}

		virtual bool rename(const char* pathFrom, const char* pathTo) override {
			return SPIFFS.rename(pathFrom, pathTo);
		}

		virtual bool remove(const char* path) override {
			return SPIFFS.remove(path);
		}

		virtual bool mkdir(const char* path) override {
			return SPIFFS.mkdir(path);
		}

		virtual bool rmdir(const char* path) override {
			return SPIFFS.rmdir(path);
		}
};

// ----------------------------------------------------------------

SpiFfsPartition::SpiFfsPartition() :
        FsPartition(FSImplPtr(new Esp32SpiFfsImpl())) {
}

SpiFfsPartition::~SpiFfsPartition() {
}

bool SpiFfsPartition::Mount(const SpiFfsMountCfg& cfg) {
	RfsLogDbg("RFS: Mounting SPIFFS @ '%s' - '%s' [auto-format: %c, max open files: %d]", cfg.mMountPoint,
	        cfg.mPartitionLabel != 0 ? "No Label" : cfg.mPartitionLabel, cfg.mFormatOnFail ? 'Y' : 'N', cfg.mMaxOpenFiles);
	return SPIFFS.begin(cfg.mFormatOnFail, cfg.mMountPoint, cfg.mMaxOpenFiles, cfg.mPartitionLabel);
}

void SpiFfsPartition::Unmount() {
	SPIFFS.end();
}

bool SpiFfsPartition::Format() {
	return SPIFFS.format();
}

uint64_t SpiFfsPartition::GetCapacity() const {
	return SPIFFS.totalBytes();
}

uint64_t SpiFfsPartition::GetUsedSize() const {
	return SPIFFS.usedBytes();
}

} // namespace Hal

#else
// SPIFFS file system is disabled

namespace Hal {
using namespace Rfs;
SpiFfsPartition::SpiFfsPartition() : FsPartition( 0 ) {}
SpiFfsPartition::~SpiFfsPartition() {}
bool SpiFfsPartition::Mount(const SpiFfsMountCfg& cfg) { return false; }
void SpiFfsPartition::Unmount() {}
bool SpiFfsPartition::Format() { return false; }
uint64_t SpiFfsPartition::GetCapacity() const { return 0; }
uint64_t SpiFfsPartition::GetUsedSize() const { return 0; }

} // namespace Hal

#endif // RFS_USE_SDMMC