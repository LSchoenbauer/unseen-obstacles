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

#include <io/Reader.h>

namespace Io {

bool Reader::Read_Bool() {
	bool value;
	Read((uint8_t*)&value, sizeof(bool));
	return value;
}

char Reader::Read_Char() {
	char value;
	Read((uint8_t*)&value, sizeof(char));
	return value;
}

uint8_t Reader::Read_UInt8() {
	uint8_t value;
	Read(&value, sizeof(uint8_t));
	return value;
}

int8_t Reader::Read_Int8() {
	int8_t value;
	Read((uint8_t*)&value, sizeof(int8_t));
	return value;
}

uint16_t Reader::Read_UInt16() {
	uint16_t value;
	Read((uint8_t*)&value, sizeof(uint16_t));
	return value;
}

int16_t Reader::Read_Int16() {
	int16_t value;
	Read((uint8_t*)&value, sizeof(int16_t));
	return value;
}

uint32_t Reader::Read_UInt32() {
	uint32_t value;
	Read((uint8_t*)&value, sizeof(uint32_t));
	return value;
}

int32_t Reader::Read_Int32() {
	int32_t value;
	Read((uint8_t*)&value, sizeof(int32_t));
	return value;
}

float Reader::Read_Float() {
	float value;
	Read((uint8_t*)&value, sizeof(float));
	return value;
}

double Reader::Read_Double() {
	double value;
	Read((uint8_t*)&value, sizeof(double));
	return value;
}

String Reader::Read_String() {
	uint32_t strLen = 0;
	Read((uint8_t*)&strLen, sizeof(uint32_t));
	char strBuffer[strLen + 1];
	uint32_t len = Read((uint8_t*)strBuffer, strLen);
	if (len == strLen) {
		strBuffer[strLen] = '\0';
		return String(strBuffer);
	}
	return String();
}

void Reader::Skip(size_t size) {
}

void Reader::Close() {
}

} // namespace Io
