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

namespace Web {
namespace Http {

struct HttpRequestMethod {
		typedef enum {
// @formatter:off
			GET,
			POST,
			DELETE,
			PUT,
			PATCH,
			HEAD,
			OPTIONS,
			ANY
// @formatter:on
		} Enum;

		static const char* GetName(Enum method);
};

}
} // namespace Web::Http
