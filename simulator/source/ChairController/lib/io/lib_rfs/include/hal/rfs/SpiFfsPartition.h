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

#pragma once

#include <rfs/FsPartition.h>
#include <utils/Singleton.h>
#include <stddef.h>
#include <inttypes.h>

namespace Hal {
using namespace Rfs;

/** The wrapper class for SPI flash file systems */
class SpiFfsPartition : public Singleton<SpiFfsPartition>, public FsPartition {
		typedef Singleton<SpiFfsPartition> Base;

	public:

		virtual FsPartition::PartitionType::Enum GetType() const {
			return FsPartition::PartitionType::SPI_FFS;
		}

		virtual bool Mount(const SpiFfsMountCfg& cfg) override;

		/**
		 * Closes the FFS and frees allocated resources.
		 */
		virtual void Unmount() override;

		/**
		 * Formats the FFS.
		 * @return True upon success, false otherwise.
		 */
		virtual bool Format() override;

		/**
		 * Provides the total size of  the FFS in bytes.
		 * @return The capacity in bytes.
		 */

		virtual uint64_t GetCapacity() const override;

		/**
		 * Provides the consumed size of the FFS in bytes.
		 * @return The occupied size.
		 */
		virtual uint64_t GetUsedSize() const override;

	protected:
		virtual ~SpiFfsPartition();

	private:
		friend class Singleton<SpiFfsPartition> ;
		SpiFfsPartition();
};

} // namespace Hal
