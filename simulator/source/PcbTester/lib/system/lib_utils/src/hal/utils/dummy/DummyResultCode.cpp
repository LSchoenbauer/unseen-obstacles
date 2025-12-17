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

#include <utils/ResultCode.h>
#include <string.h>

bool Rc::IsOk() const {
	return mCode == 0; // 0 seems to be OK in most cases
}

const char* Rc::GetName() const {
	return "ERROR";
}

const char* Rc::CopyName(char* target, size_t targetLength) const {
	if (target != 0) {
		strncpy(target, "ERROR", targetLength);
	}
	return target;
}

