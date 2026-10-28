//
// Created by brand on 9/1/2025.
//

#ifndef EJECTBIN_H
#define EJECTBIN_H
#include "bin.h"
#include "elementbase.h"

namespace Keela {
/**
 * A pipeline element that may be removed from a pipeline during live playback
 *
 * This class is only suitable for elements which lie at the end of the pipeline and have no downstream elements.
 */
class EjectableElement : virtual private Keela::Element {
   public:
	/**
	 * Prepare the element for ejection from the pipeline, but do not wait for the element to finish processing
	 * remaining data.
	 *
	 * Callers must at some point call `EjectableElement::Eject(false);` to finish the ejection process
	 */
	void PrepareEject();

	/**
	 * Eject the element from the pipeline and wait for it to finish processing remaining data.
	 *
	 *
	 * @param prepare true if the element should prepare itself for ejection, false if not
	 */
	void Eject(bool prepare);

   private:
	/**
	 * Get a pointer to the first element in the bin. Subclasses must be able to guarantee that they own the element
	 * pointer
	 *
	 * @return
	 */
	virtual Keela::Element *Head() = 0;

	/**
	 * get a list of the "leaf" elements of the bin. Subclasses must be able to guarantee that they own all element
	 * pointers
	 *
	 * @return a list of all "leaf" elements in the bin
	 */
	virtual std::vector<Keela::Element *> Leaves() = 0;

	/// callback unlinks element, sends EOS on its src, then installs an EOS event callback to clean up the remaining
	/// data
	static GstPadProbeReturn pad_block_callback(GstPad *pad, GstPadProbeInfo *info, gpointer user_data);

	/// callback cleans up any remaining data from the bin and removes it from the pipeline
	static GstPadProbeReturn event_callback(GstPad *pad, GstPadProbeInfo *info, gpointer user_data);

	/// used to count how many EOS events recieved from the leaf elements
	unsigned int leaves_eos_count = 0;
	bool is_ejecting = false;
	bool safe_to_remove = false;
	std::mutex remove_mutex;
	std::condition_variable remove_condition;
};
}  // namespace Keela

#endif  // EJECTBIN_H
