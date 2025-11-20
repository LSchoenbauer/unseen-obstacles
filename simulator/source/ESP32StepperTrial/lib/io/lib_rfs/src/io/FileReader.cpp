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

#include <io/FileReader.h>
#include <rfs/RootFileSystem.h>

namespace Io {

FileReader::FileReader(const String& path) :
        mFile(), mLastStatus(IoStatus::ERROR) {
	mFile = Rfs::RootFileSystem::GetInstance()->Open(path, Rfs::RootFileSystem::FileMode::Read);
	mLastStatus = (mFile ? IoStatus::OK : IoStatus::ERROR);
}

FileReader::FileReader(File file) :
        mFile(file), mLastStatus(IoStatus::ERROR) {
	mLastStatus = (mFile ? IoStatus::OK : IoStatus::ERROR);
}

FileReader::~FileReader() {
	Close();
}

void FileReader::Close() {
	mLastStatus = IoStatus::ERROR;
	if (mFile) {
		mFile.close();
		mLastStatus = IoStatus::OK;
	}
}

size_t FileReader::Read(uint8_t* data, size_t size) {
	int resSize = -1;
	if (mFile) {
		resSize = mFile.read(data, size);
	}
	mLastStatus = (resSize == EOF ? IoStatus::EOS : (resSize < 0 ? IoStatus::OK : IoStatus::ERROR));
	return resSize;
}

void FileReader::Skip(size_t size) {
	if (mFile) {
		bool rc = mFile.seek(size, fs::SeekCur);
		mLastStatus = (rc ? IoStatus::OK : IoStatus::ERROR);
	}
}

IoStatus::Enum FileReader::GetLastStatus() {
	return mLastStatus;
}

} // namespace Io
