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

#include <stddef.h>
#include <inttypes.h>
#include <WString.h>

namespace Io {

/**
 * The abstract base interface class for data writers.
 */
class Writer {
	public:
		/**
		 * Tries to write the given data.
		 *
		 * @param data The pointer to the data to write.
		 * @param size The number of data bytes to write.
		 * @return The number of written bytes. This is equal to 'size' if all requested
		 * bytes could be read, a value less than 'size' if not all bytes could
		 * be read. Especially if 0 is returned, method 'GetStatus' shall be
		 * consulted for details why no data could have been read.
		 */
		virtual size_t Write(uint8_t* data, size_t size) = 0;

		/**
		 * Writes the given boolean value.
		 * @param value The value to write.
		 */
		void Write(bool value);

		/**
		 * Writes the given character value.
		 * @param value The value to write.
		 */
		void Write(char value);

		/**
		 * Writes the given unsigned 8bit integer value.
		 * @param value The value to write.
		 */
		void Write(uint8_t value);

		/**
		 * Writes the given signed 8bit integer value.
		 * @param value The value to write.
		 */
		void Write(int8_t value);

		/**
		 * Writes the given unsigned 16bit integer value.
		 * @param value The value to write.
		 */
		void Write(uint16_t value);

		/**
		 * Writes the given signed 16bit integer value.
		 * @param value The value to write.
		 */
		void Write(int16_t value);
		/**
		 * Writes the given unsigned 32bit integer value.
		 * @param value The value to write.
		 */
		void Write(uint32_t value);

		/**
		 * Writes the given signed 32bit integer value.
		 * @param value The value to write.
		 */
		void Write(int32_t value);

		/**
		 * Writes the given float value.
		 * @param value The value to write.
		 */
		void Write(float value);

		/**
		 * Writes the given double value.
		 * @param value The value to write.
		 */
		void Write(double value);

		/**
		 * Writes the given String value.
		 * @param value The value to write.
		 */
		void Write(String value);

		/**
		 * Flushes all cached data to the underlying resource.
		 */
		virtual void Flush();

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
		Writer();
		virtual ~Writer();

};

} // namespace Io
