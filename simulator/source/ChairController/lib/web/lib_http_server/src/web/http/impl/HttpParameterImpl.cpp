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

#include "HttpParameterImpl.h"
#include <utils/Log.h>
#include <utils/SharedGlobals.h>

namespace Web {
namespace Http {

::std::shared_ptr<HttpParameter> HttpParameterImpl::Create(AsyncWebParameter* nativeParam) {
	return std::make_shared<HttpParameterImpl>(nativeParam);
}

HttpParameterImpl::HttpParameterImpl(AsyncWebParameter* nativeParam) :
        Base(), Wrapper(nativeParam) {
}

HttpParameterImpl::~HttpParameterImpl() {
}

const String& HttpParameterImpl::GetName() const {
	return (HasWrapped() ? GetWrapped()->name() : Gbl::invalidString);
}

const String& HttpParameterImpl::GetValue() const {
	return (HasWrapped() ? GetWrapped()->value() : Gbl::invalidString);
}

size_t HttpParameterImpl::GetSize() const {
	return (HasWrapped() ? GetWrapped()->size() : 0);
}

bool HttpParameterImpl::IsPost() const {
	return (HasWrapped() ? GetWrapped()->isPost() : false);
}

bool HttpParameterImpl::IsFile() const {
	return (HasWrapped() ? GetWrapped()->isFile() : false);
}

void HttpParameterImpl::DumpDiagnostics() const {
	LogInfo("HTTP Parameter: %s: %s (%d B), post: %s, file: %s", Logger::Dump(GetName()), Logger::Dump(GetValue()), GetSize(),
	        Logger::Dump(IsPost()), Logger::Dump(IsFile()));
}

}
} // namespace Web::Http
