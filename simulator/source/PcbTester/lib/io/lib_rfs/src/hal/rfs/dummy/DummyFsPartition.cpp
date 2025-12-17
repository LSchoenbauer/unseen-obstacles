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

#include <rfs/FsPartition.h>

namespace Rfs {

FsPartition::FsPartition(FSImplPtr fsImpl) :
        FS(fsImpl) {
}

FsPartition::~FsPartition() {
}

void FsPartition::DumpPartitionTable() {
	// nothing to dump, no warning to log
}

} // namespace Rfs
