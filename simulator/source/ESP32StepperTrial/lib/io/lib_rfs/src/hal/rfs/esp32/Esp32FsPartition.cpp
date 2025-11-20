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
#include <utils/Log.h>
#include <esp_partition.h>

namespace Rfs {

class Esp32PartitionInfoDumper {
	public:
		static void DumpPartition(esp_partition_type_t type, esp_partition_subtype_t subType) {
#ifdef LOG_INFO_ENABLED
			const int maxLength = 8;
			int length = 0;
			char padding[maxLength + 1] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0' };
			esp_partition_iterator_t part_it = esp_partition_find(type, subType, NULL);
			while (part_it != NULL) {
				const esp_partition_t* pd = esp_partition_get(part_it);
				padding[length] = ' ';
				length = strlen(pd->label);
				length = length > maxLength ? 0 : maxLength - length;
				padding[length] = '\0';
				LogInfo("  - %s'%s' @ 0x%08X of %7d B, Type: %s/0x%02X, %s", padding, pd->label, pd->address, pd->size,
				        (pd->type == ESP_PARTITION_TYPE_APP ? " APP" : pd->type == ESP_PARTITION_TYPE_DATA ? "DATA" : "TYPE?"),
				        pd->subtype, pd->encrypted ? "ENC" : "NON-ENC");
				part_it = esp_partition_next(part_it);
			}
			esp_partition_iterator_release(part_it);
#endif
		}
};

// --------------------------------------------

FsPartition::FsPartition(FSImplPtr fsImpl) :
        FS(fsImpl) {
}
FsPartition::~FsPartition() {
}

void FsPartition::DumpPartitionTable() {
#ifdef LOG_INFO_ENABLED
	LogInfo("PartitionType Table:");
	Esp32PartitionInfoDumper::DumpPartition(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY);
	Esp32PartitionInfoDumper::DumpPartition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY);
#endif
}

} // namespace Rfs
