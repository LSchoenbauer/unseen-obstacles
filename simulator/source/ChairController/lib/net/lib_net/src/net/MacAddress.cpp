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

#include <net/MacAddress.h>
#include <stdio.h>

namespace Net {

MacAddress::MacAddress() :
        mMac { 0, 0, 0, 0, 0, 0 } {
}

MacAddress::MacAddress(const uint8_t mac[6]) :
        mMac { mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] } {
}

MacAddress::MacAddress(const MacAddress& o) :
        mMac { o.mMac[0], o.mMac[1], o.mMac[2], o.mMac[3], o.mMac[4], o.mMac[5] } {
}

MacAddress::~MacAddress() {
}

MacAddress& MacAddress::operator=(const MacAddress& other) {
	if (this != &other) {
		operator=(other.mMac);
	}
	return *this;
}

MacAddress& MacAddress::operator=(const uint8_t mac[6]) {
	mMac[0] = mac[0];
	mMac[1] = mac[1];
	mMac[2] = mac[2];
	mMac[3] = mac[3];
	mMac[4] = mac[4];
	mMac[5] = mac[5];
	return *this;
}

uint8_t* MacAddress::operator*() {
	return mMac;
}
const uint8_t* MacAddress::operator*() const {
	return mMac;
}

bool MacAddress::operator==(const MacAddress& other) const {
	return (this == &other || operator==(other.mMac));
}

bool MacAddress::operator==(const uint8_t mac[6]) const {
	return (mac[0] == this->mMac[0] && mac[1] == this->mMac[1] && mac[2] == this->mMac[2] && mac[3] == this->mMac[3]
	        && mac[4] == this->mMac[4] && mac[5] == this->mMac[5]);
}

bool MacAddress::operator!=(const MacAddress& other) const {
	return !operator==(other);
}

bool MacAddress::operator!=(const uint8_t mac[6]) const {
	return !operator==(mac);
}

MacAddress::operator bool() const {
	return this->mMac[0] != 0 || this->mMac[1] != 0 || this->mMac[2] != 0 || this->mMac[3] != 0 || this->mMac[4] != 0
	        || this->mMac[5] != 0;
}

String MacAddress::ToString() const {
	const unsigned int strBufLen = 6 * 2 + 5 + 1;
	char strBuf[strBufLen];
	snprintf(strBuf, strBufLen, "%02X:%02X:%02X:%02X:%02X:%02X", mMac[0], mMac[1], mMac[2], mMac[3], mMac[4], mMac[5]);
	return String(strBuf);
}

} /* namespace Net */
