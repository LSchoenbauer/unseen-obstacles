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

#include <rfs/RootFileSystem.h>
#include <FS.h>
#include <memory>

namespace Hal {

/**
 * Factory for classes that abstracts underlying hardware and frameworks.
 */
class HalFab {
	public:
		static fs::FSImplPtr GetRfsImplPtr(Rfs::RootFileSystem& rfs);
		static fs::FileImplPtr GetRfsFileImplPtr(File f, const String& rootPath);
};

} // namespace Hal
