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

#include <hal/HalFab.h>
#include <hal/rfs/esp32/Esp32RfsImpl.h>
#include <hal/rfs/esp32/Esp32FileImplWrapper.h>
#include <memory>

namespace Hal {

fs::FSImplPtr HalFab::GetRfsImplPtr(Rfs::RootFileSystem& rfs) {
	return FSImplPtr(new Esp32RfsFsImpl(rfs));
}

fs::FileImplPtr HalFab::GetRfsFileImplPtr(File f, const String& rootPath) {
	return std::make_shared<Esp32FileImplWrapper>(f, rootPath);
}

} // namespace Hal
