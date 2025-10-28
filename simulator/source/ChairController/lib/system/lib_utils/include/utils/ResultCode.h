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

#include <inttypes.h>
#include <stddef.h>

class Rc {
	public:

		/** Construct a result code indicating 'OK'. */
		Rc();

		/** Construct a result code wrapping the given code value. */
		Rc(int32_t code) :
		        mCode(code) {
		}

		/** Copy constructor. */
		Rc(const Rc& code) :
		        mCode(code.mCode) {
		}

		/** Assigns the given numeric result code. */
		Rc operator=(int32_t code) {
			mCode = code;
			return *this;
		}

		/** Assigns the given numeric result code. */
		Rc operator=(const Rc& code) {
			mCode = code.mCode;
			return *this;
		}

		/**
		 * Determines whether or not the result indicates success.
		 * @return True for success, false otherwise.
		 */
		bool IsOk() const;

		/**
		 * Provides the underlying code value.
		 * @return The code.
		 */
		int32_t GetCode() const {
			return mCode;
		}

		/**
		 * Provides the user friendly name or label for the result code.
		 * @return The result name.
		 */
		const char* GetName() const;
		const char* CopyName(char* target, size_t targetLength) const;

		/**
		 * Determines whether or not the result indicates success.
		 * @return True for success, false otherwise.
		 */
		operator bool() const {
			return IsOk();
		}

		friend bool operator==(const Rc& lhs, int32_t rhs) {
			return lhs.mCode == rhs;
		}

		friend bool operator==(int32_t lhs, const Rc& rhs) {
			return operator==(rhs, lhs);
		}

		friend bool operator==(const Rc& lhs, const Rc& rhs) {
			return &lhs == &rhs || lhs.mCode == rhs.mCode;
		}

		friend bool operator!=(const Rc& lhs, int32_t rhs) {
			return !operator==(lhs, rhs);
		}

		friend bool operator!=(int32_t lhs, const Rc& rhs) {
			return !operator==(rhs, lhs);
		}

		friend bool operator!=(const Rc& lhs, const Rc& rhs) {
			return !operator==(lhs, rhs);
		}

	private:
		int32_t mCode;

};
