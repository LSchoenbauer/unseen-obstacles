/*
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

#include <rfs/FsPartition.h>
#include <hal/rfs/SpiFfsPartition.h>
#include <hal/rfs/SdMmcPartition.h>
#include <utils/Log.h>

using namespace fs;
namespace Rfs {

FsPartition* FsPartition::GetPartitionInstance(PartitionType::Enum type) {
	switch (type) {
#ifdef RFS_USE_SPIFFS
		case PartitionType::SPI_FFS:
			return Hal::SpiFfsPartition::GetInstance();
#endif
#ifdef RFS_USE_SDMMC
		case PartitionType::SD_MMC_FS:
			return Hal::SdMmcPartition::GetInstance();
#endif
		default:
			return 0;
	}
	return 0;
}

// note: c'tor and d'tor are device specific or defined in 'DummyFsPartition.cpp'

bool FsPartition::Mount(const FsPartition::SpiFfsMountCfg& cfg) {
	return false;
}

bool FsPartition::Mount(const FsPartition::SdMmcMountCfg& cfg) {
	return false;
}

} // namespace Rfs;

