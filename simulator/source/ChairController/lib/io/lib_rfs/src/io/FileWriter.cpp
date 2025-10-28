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

#include <io/FileWriter.h>
#include <rfs/RootFileSystem.h>

namespace Io {

FileWriter::FileWriter(const String& path, bool append) :
        mFile(), mLastStatus(IoStatus::ERROR) {
	mFile = Rfs::RootFileSystem::GetInstance()->Open(path,
	        append ? Rfs::RootFileSystem::FileMode::WriteAppend : Rfs::RootFileSystem::FileMode::WriteTruncate);
	mLastStatus = (mFile ? IoStatus::OK : IoStatus::ERROR);
}

FileWriter::FileWriter(File file) :
        mFile(file), mLastStatus(IoStatus::ERROR) {
	mLastStatus = (mFile ? IoStatus::OK : IoStatus::ERROR);
}

FileWriter::~FileWriter() {
	Close();
}

void FileWriter::Close() {
	mLastStatus = IoStatus::ERROR;
	if (mFile) {
		mFile.close();
		mLastStatus = IoStatus::OK;
	}
}

size_t FileWriter::Write(uint8_t* data, size_t size) {
	int32_t resSize = -1;
	if (mFile) {
		resSize = (int32_t)mFile.write(data, size);
	}
	mLastStatus = (resSize == EOF ? IoStatus::EOS : (resSize < 0 ? IoStatus::OK : IoStatus::ERROR));
	return resSize;
}

void FileWriter::Flush() {
	if (mFile) {
		mFile.flush();
		mLastStatus = IoStatus::OK;
	}
}

IoStatus::Enum FileWriter::GetLastStatus() {
	return mLastStatus;
}

} // namespace Io
