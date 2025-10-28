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

#include <WString.h>
#include <inttypes.h>

namespace Net {

/**
 * Encapsulates MAC addresses.
 */
class MacAddress {
	public:

		/**
		 * Constructs an empty MAC address with value 00:00:00:00:00:00.
		 */
		MacAddress();

		/**
		 * Copy constructor.
		 * @param other MAC address to copy
		 */
		MacAddress(const MacAddress& other);

		/**
		 * Constructs a MAC address from the given value.
		 * @param mac The raw MAC address expressed with 6 integer values.
		 */
		MacAddress(const uint8_t mac[6]);

		/**
		 * Destructor
		 */
		~MacAddress();

		/**
		 * Assignment operator.
		 * @param other The MAC address to assign.
		 * @return The changed MAC address.
		 */
		MacAddress& operator=(const MacAddress& other);

		/**
		 * Assigns the raw MAC address.
		 * @param mac The MAC address to assign.
		 * @return The changed MAC address.
		 */
		MacAddress& operator=(const uint8_t mac[6]);

		/**
		 * Provides the internal buffer that stores the MAC address. The buffer has a length of 6 elements.
		 * @return The MAC address buffer containing all 6 segments of the MAC address.
		 */
		uint8_t* operator*();
		const uint8_t* operator*() const;

		/**
		 * Comparison operator.
		 * @param other The MAC address to compare.
		 * @return True if both MAC address share the same value.
		 */
		bool operator==(const MacAddress& other) const;

		/**
		 * Comparison operator.
		 * @param mac The MAC address to compare.
		 * @return True if both MAC address share the same value.
		 */
		bool operator==(const uint8_t mac[6]) const;

		/**
		 * Comparison operator.
		 * @param other The MAC address to compare.
		 * @return True if both MAC address share the same value.
		 */
		bool operator!=(const MacAddress& other) const;

		/**
		 * Comparison operator.
		 * @param mac The MAC address to compare.
		 * @return True if both MAC address share the same value.
		 */
		bool operator!=(const uint8_t mac[6]) const;

		/**
		 * Validity operator.
		 * @return True if the MAC address is valid. This is the case, if it is different to 00:00:00:00:00:00.
		 */
		operator bool() const;

		/**
		 * Provides the MAC address as formatted string.
		 * @return The MAC address as string.
		 */
		String ToString() const;

	private:
		uint8_t mMac[6];
};

} /* namespace Net */
