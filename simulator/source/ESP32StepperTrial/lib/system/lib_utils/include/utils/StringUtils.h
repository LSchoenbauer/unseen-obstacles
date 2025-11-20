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

#include <WString.h>

/**
 * Provides utility functions for strings.
 */
class StringUtils {
	public:

		/** Creates a String object from the given byte buffer. */
		static String Of(const uint8_t* buffer, const uint32_t bufferLength);
		/** Creates a String object from the given byte buffer with the given maximum length. */
		static String Of(const uint8_t* buffer, const uint32_t bufferLength, const uint32_t maxBufferLength);

		/** Provides the C-string of the provided 'str' if it is valid, otherwise the 'defaultCStr' is returned. */
		static inline const char* CStrElvis(const String& str, const char* defaultCStr = "NULL") { return str ? str.c_str() : defaultCStr;}

		/**
		 * Copies the given String into the given byte buffer with the given size.
		 * If the length of the string is equal to the maximum buffer size, the character sequence
		 * within the buffer is NOT 0-terminated, otherwise it is.
		 */
		static void NCopy(uint8_t* dst, const String& src, size_t dstLen);

	private:
		StringUtils() = delete;
		~StringUtils() = delete;
};
