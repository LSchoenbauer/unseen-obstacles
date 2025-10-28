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

#include <memory>

template<typename C>
class SharedSingleton {
	public:
		static std::shared_ptr<C> GetInstance() {
			static SharedSingletonGuard guard;
			if (mMe == nullptr) {
				C* me = new C();
				mMe = std::shared_ptr<C>(me, [](C* obj) {
					delete obj;
				});
			}
			return mMe;
		}

	protected:
		/** Singleton constructor */
		SharedSingleton() {
		}
		virtual ~SharedSingleton() {
		}

		static bool HasMe() {
			return mMe != nullptr;
		}

		static std::shared_ptr<C> GetMe() {
			return mMe;
		}

		static void ApplyMe(std::shared_ptr<C> me) {
			mMe = me;
		}

		/** Singleton memory guard */
		class SharedSingletonGuard {
			public:
				~SharedSingletonGuard() {
					if (HasMe()) {
						mMe = nullptr;
					}
				}
		};

	private:
		friend class SharedSingletonGuard;

		/** Singleton instance */
		static std::shared_ptr<C> mMe;
		/** Singleton copy constructor */
		SharedSingleton(const SharedSingleton&);
};

template<typename C> std::shared_ptr<C> SharedSingleton<C>::mMe = nullptr;
