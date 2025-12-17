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
#include <FS.h>
#include <FSImpl.h>

namespace Hal {
using namespace Rfs;

class Esp32FileImplWrapper : public FileImpl {
	public:
		Esp32FileImplWrapper(File f, const String& rootPath) :
		        mNativeFile(f), mRootPath(rootPath) {
		}

		virtual ~Esp32FileImplWrapper() {
			if (mNativeFile) {
				mNativeFile.close();
			}
		}

    	virtual const char* path() const override {
			return mNativeFile ? mNativeFile.path() : 0;
		};

		virtual size_t write(const uint8_t* buf, size_t size) override {
			return mNativeFile ? mNativeFile.write(buf, size) : 0;
		}

		virtual size_t read(uint8_t* buf, size_t size) override {
			return mNativeFile ? mNativeFile.read(buf, size) : 0;
		}

		virtual void flush() override {
			if (mNativeFile) {
				mNativeFile.flush();
			}
		}

		virtual bool seek(uint32_t pos, SeekMode mode) override {
			return mNativeFile ? mNativeFile.seek(pos, mode) : 0;
		}

		virtual size_t position() const override {
			return mNativeFile ? mNativeFile.position() : 0;
		}

		virtual size_t size() const override {
			return mNativeFile ? mNativeFile.size() : 0;
		}
    	
		virtual bool setBufferSize(size_t size) override {
			return mNativeFile ? mNativeFile.setBufferSize(size) : false;
		}

		virtual void close() override {
			if (mNativeFile) {
				mNativeFile.close();
			}
		}

		virtual time_t getLastWrite() override {
			return mNativeFile ? mNativeFile.getLastWrite() : 0;
		}

		virtual const char* name() const override {
//			return mNativeFile ? (mRootPath + mNativeFile.name()).c_str() : 0;
			return mNativeFile ? mNativeFile.name() : 0;
		}

		virtual boolean isDirectory() override {
			return mNativeFile ? mNativeFile.isDirectory() : 0;
		}

		virtual FileImplPtr openNextFile(const char* mode) override {
			return mNativeFile ? std::make_shared<Esp32FileImplWrapper>(mNativeFile.openNextFile(mode), mRootPath) : FileImplPtr();
		}

		virtual void rewindDirectory(void) override {
			if (mNativeFile) {
				mNativeFile.rewindDirectory();
			}
		}

		virtual boolean seekDir(long position) override {
			return mNativeFile ? mNativeFile.seekDir(position) : 0;
		}

		virtual String getNextFileName(void) override {
			return mNativeFile ? mNativeFile.getNextFileName() : "";
		}

		virtual String getNextFileName(bool* isDir) override {
			return mNativeFile ? mNativeFile.getNextFileName(isDir) : "";
		}

		virtual operator bool() override {
			return mNativeFile;
		}

	private:
		File mNativeFile;
		String mRootPath;
};

} // namespace Hal
