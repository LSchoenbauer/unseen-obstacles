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

#include <io/IoStatus.h>

#include <WString.h>
#include <stddef.h>
#include <inttypes.h>

namespace Io {

/**
 * The abstract base interface class for data readers.
 */
class Reader {
	public:

		/**
		 * Tries to read the given number of bytes into the given data sink.
		 * The buffer must be capable to absorb at least the number of bytes
		 * specified by 'size'.
		 * The method 'consumes' the read bytes. This means, the a subsequent invocation
		 * of this method provides the adjacent bytes.
		 *
		 * @param data The pointer to the data sink to which data shall be copied.
		 * @param size The number of bytes to read.
		 * @return The number of bytes read. This is equal to 'size' if all requested
		 * bytes could be read, a value less than 'size' if not all bytes could
		 * be read. Especially if 0 is returned, method 'GetStatus' shall be
		 * consulted for details why no data could have been read.
		 */
		virtual size_t Read(uint8_t* data, size_t size) = 0;

		/**
		 * Reads and provides a single boolean value.
		 * @return The read value.
		 */
		bool Read_Bool();

		/**
		 * Reads and provides a single char value.
		 * @return The read value.
		 */
		char Read_Char();

		/**
		 * Reads and provides a single unsigned 8bit integer value.
		 * @return The read value.
		 */
		uint8_t Read_UInt8();

		/**
		 * Reads and provides a single signed 8bit integer value.
		 * @return The read value.
		 */
		int8_t Read_Int8();

		/**
		 * Reads and provides a single unsigned 16bit integer value.
		 * @return The read value.
		 */
		uint16_t Read_UInt16();

		/**
		 * Reads and provides a single signed 16bit integer value.
		 * @return The read value.
		 */
		int16_t Read_Int16();

		/**
		 * Reads and provides a single unsigned 32bit integer value.
		 * @return The read value.
		 */
		uint32_t Read_UInt32();

		/**
		 * Reads and provides a single signed 32bit integer value.
		 * @return The read value.
		 */
		int32_t Read_Int32();

		/**
		 * Reads and provides a single float value.
		 * @return The read value.
		 */
		float Read_Float();

		/**
		 * Reads and provides a single double value.
		 * @return The read value.
		 */
		double Read_Double();

		/**
		 * Reads and provides a string value.
		 * @return The read value.
		 */
		String Read_String();

		/**
		 * Skips over the given number of bytes without reading them. Subsequent calls
		 * of method 'Read' provides the bytes adjacent to the skipped section, if any.
		 * @param size The number of bytes to skip.
		 */
		virtual void Skip(size_t size);

		/**
		 * Closes the stream and releases the underlying resource.
		 */
		virtual void Close();

		/**
		 * Provides the status of the last operation.
		 * @return The current status.
		 */
		virtual IoStatus::Enum GetLastStatus() = 0;

	protected:
		Reader();
		virtual ~Reader();

};

} // namespace Io
