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

#include <utils/StringUtils.h>
#include <cstring>

String StringUtils::Of(const uint8_t* buffer, const uint32_t bufferLength) {
	char strBuf[bufferLength + 1];
	if (buffer != 0) {
		memcpy(strBuf, buffer, bufferLength);
		strBuf[bufferLength] = '\0';
	}
	return String(strBuf);
}

String StringUtils::Of(const uint8_t* buffer, const uint32_t bufferLength, const uint32_t maxBufferLength) {
	return StringUtils::Of(buffer, (bufferLength <= maxBufferLength ? bufferLength : maxBufferLength));
}

void StringUtils::NCopy(uint8_t* dst, const String& src, size_t dstLen) { // @suppress("Name convention for function")
	size_t strLen = src.length();
	size_t cpyLen = dstLen < strLen ? dstLen : strLen;
	memcpy(dst, reinterpret_cast<const uint8_t*>(src.c_str()), cpyLen);
	if (cpyLen < dstLen) {
		// leave the char seq unterminated if it fits into the buffer, otherwise null the rest
		memset((dst + cpyLen), 0, (dstLen - cpyLen));
	}
}

