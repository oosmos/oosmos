//
// adcbtnphtest Class
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
 	
#ifndef adcbtnphtest_h
#define adcbtnphtest_h

#include "oosmos.h"
#include "adcbtnphtest.h"
#include "adcbtnph.h"

typedef struct adcbtnphtestTag adcbtnphtest;
 	
extern adcbtnphtest * adcbtnphtestNew(adc * pAdc, uint32_t HoldTimeMS);
 	
#endif