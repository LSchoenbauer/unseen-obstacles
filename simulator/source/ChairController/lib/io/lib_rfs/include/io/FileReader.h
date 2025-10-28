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

#include <io/Reader.h>
#include <io/IoStatus.h>

#include <WString.h>
#include <FS.h>

#include <stddef.h>
#include <inttypes.h>

namespace Io {

/**
 * The reader implementation for reading data from files.
 */
class FileReader : public Reader {
	public:

		/**
		 * Constructs a reader that reads content from the file at the given path.
		 * If the file does not exist, each method returns an error.
		 *
		 * @param path The path of the file to read.
		 */
		FileReader(const String& path);

		/**
		 * Constructs a reader that reads content from the given file.
		 * If the file does not exist, each method returns an error.
		 *
		 * @param path The path of the file to read.
		 */
		FileReader(File file);

		/**
		 * D'tor
		 */
		virtual ~FileReader();

		virtual void Close() override;

		virtual size_t Read(uint8_t* data, size_t size) override;

		void Skip(size_t size) override;

		virtual IoStatus::Enum GetLastStatus() override;

	protected:

	private:
		File mFile;
		IoStatus::Enum mLastStatus;

};

} // namespace Io
