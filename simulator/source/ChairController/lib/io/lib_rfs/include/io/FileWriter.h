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

#include <io/Writer.h>
#include <io/IoStatus.h>

#include <WString.h>
#include <FS.h>

#include <stddef.h>
#include <inttypes.h>

namespace Io {

/**
 * The writer implementation for writing data to files.
 */
class FileWriter : public Writer {
	public:

		/**
		 * Constructs a reader that reads content from the file at the given path.
		 * If the file does not exist, each method returns an error.
		 *
		 * @param path The path of the file to read.
		 * @param append True for appending the file, false (default) to write to a cleared file.
		 */
		FileWriter(const String& path, bool append = false);

		/**
		 * Constructs a reader that reads content from the given file.
		 * If the file does not exist, each method returns an error.
		 *
		 * @param path The path of the file to read.
		 */
		FileWriter(File file);

		/**
		 * D'tor
		 */
		virtual ~FileWriter();

		virtual void Close() override;

		virtual size_t Write(uint8_t* data, size_t size) override;

		virtual void Flush() override;

		virtual IoStatus::Enum GetLastStatus() override;

	protected:

	private:
		File mFile;
		IoStatus::Enum mLastStatus;

};

} // namespace Io
