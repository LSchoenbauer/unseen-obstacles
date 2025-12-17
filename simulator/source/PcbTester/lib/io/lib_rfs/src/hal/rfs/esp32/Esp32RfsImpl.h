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

#include <rfs/RootFileSystem.h>
#include <hal/rfs/esp32/Esp32FileImplWrapper.h>
#include <rfs/RfsImplUtils.h>
#include <FS.h>
#include <FSImpl.h>

namespace Hal {
using namespace Rfs;

class Esp32RfsFsImpl : public FSImpl {
	public:
		// @formatter:off
		Esp32RfsFsImpl(RootFileSystem& rfs) : FSImpl(), mRfs(rfs) { _mountpoint = "/"; }
		virtual ~Esp32RfsFsImpl() {}

		virtual FileImplPtr open(const char* path, const char* mode, const bool create = false) override  {
			RootFileSystem::FileMode::Enum modeEnum = (mode != 0
					? (mode[0] == 'a'
							? RootFileSystem::FileMode::WriteAppend
							: (mode[0] == 'w'
									? RootFileSystem::FileMode::WriteTruncate
									: RootFileSystem::FileMode::Read))
					: RootFileSystem::FileMode::Read);
			return std::make_shared<Esp32FileImplWrapper>(mRfs.Open(path, modeEnum, create), RfsImplUtils::GetMountPoint(mRfs, path));
		}

		virtual bool exists(const char* path) override { return mRfs.Exists(path); }
		virtual bool rename(const char* pathFrom, const char* pathTo) override { return mRfs.Rename(pathFrom, pathTo); }
		virtual bool remove(const char* path) override { return mRfs.Remove(path); }
		virtual bool mkdir(const char* path) override { return mRfs.MkDir(path); }
		virtual bool rmdir(const char* path) override { return mRfs.RmDir(path); }
// @formatter:on
	protected:
		RootFileSystem& mRfs;

};

} // namespace Hal

