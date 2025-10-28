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

#include <io/Writer.h>

namespace Io {

Writer::Writer() {
}

Writer::~Writer() {
}

void Writer::Write(bool value) {
	Write((uint8_t*)&value, sizeof(bool));
}

void Writer::Write(char value) {
	Write((uint8_t*)&value, sizeof(char));
}

void Writer::Write(uint8_t value) {
	Write((uint8_t*)&value, sizeof(uint8_t));
}

void Writer::Write(int8_t value) {
	Write((uint8_t*)&value, sizeof(int8_t));
}

void Writer::Write(uint16_t value) {
	Write((uint8_t*)&value, sizeof(uint16_t));
}

void Writer::Write(int16_t value) {
	Write((uint8_t*)&value, sizeof(int16_t));
}

void Writer::Write(uint32_t value) {
	Write((uint8_t*)&value, sizeof(uint32_t));
}

void Writer::Write(int32_t value) {
	Write((uint8_t*)&value, sizeof(int32_t));
}

void Writer::Write(float value) {
	Write((uint8_t*)&value, sizeof(float));
}

void Writer::Write(double value) {
	Write((uint8_t*)&value, sizeof(double));
}

void Writer::Write(String value) {
	uint32_t strLen = value.length(); // without terminating '\0'
	Write((uint8_t*)&strLen, sizeof(uint32_t));
	const char* strBuffer = value.c_str();
	Write((uint8_t*)strBuffer, strLen + 1);
}

void Writer::Flush() {
}

void Writer::Close() {
}

} // namespace Io
