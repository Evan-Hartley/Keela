//
// Created by brand on 5/31/2025.
//

#ifndef WIDGETELEMENT_H
#define WIDGETELEMENT_H
#include "elementbase.h"

namespace Keela {
class WidgetElement {
   public:
	virtual ~WidgetElement() = default;

	/// return a pointer to the widget managed by this element
	virtual gpointer get_widget() = 0;
};
}  // namespace Keela
#endif  // WIDGETELEMENT_H
