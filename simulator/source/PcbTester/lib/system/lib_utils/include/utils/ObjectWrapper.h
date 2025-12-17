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

template<typename T>
class ObjectWrapper {
	public:
		/** Wraps the given object */
		ObjectWrapper(T* obj) :
		        mWrappedObj(obj) {
		}

		/** Copy c'tor */
		explicit ObjectWrapper(const ObjectWrapper<T>& othr) {
			if (othr != 0) {
				mWrappedObj = othr.mWrappedObj;
			}
		}

		/** Destructor */
		virtual ~ObjectWrapper() {
			mWrappedObj = 0;
		}

		/** Assignment operator */
		ObjectWrapper<T>& operator=(const ObjectWrapper<T>& othr) {
			if (this != othr) {
				mWrappedObj = othr.mWrappedObj;
			}
			return this;
		}

		/** Dereferences to the wrapped object */
		const T* operator*() const noexcept {
			return mWrappedObj;
		}

		/** Dereferences to the wrapped object */
		T* operator*() {
			return mWrappedObj;
		}

		/** Determines whether or not a object is wrapped. */
		bool HasWrapped() const {
			return mWrappedObj != 0;
		}

		/** Provides the wrapped object. */
		T* GetWrapped() {
			return mWrappedObj;
		}

		/** Provides the wrapped object. */
		const T* GetWrapped() const {
			return mWrappedObj;
		}

		/** Deletes the wrapped object. */
		void DeleteWrapped() {
			if (mWrappedObj != 0) {
				delete mWrappedObj;
			}
			mWrappedObj = 0;
		}

	private:
		T* mWrappedObj;
};
