//
// Created by brand on 5/26/2025.
//

#ifndef BIN_H
#define BIN_H
#include <memory>
#include <string>
#include <gstreamer-1.0/gst/gst.h>

#include "elementbase.h"

namespace Keela {
    class Bin : public virtual Keela::Element {
    public:
        explicit Bin(const std::string &name);

        Bin();

        ~Bin() override;

        operator GstElement *() const override;

        operator GstBin *() const;

        /// dump the bin data to a graphviz graph
        void dump_bin_graph() const;


        /// add many elements to the current bin
        template<typename... Elements>
        void add_elements(Elements... elements) {
            gst_bin_add_many(static_cast<GstBin *>(*this), static_cast<GstElement *>(elements)..., nullptr);
        }

    protected:
        std::shared_ptr<GstBin> bin;

        /**
         * Create a ghost pad for an internal GstElement
         */
        void add_ghost_pad(GstElement *element, const std::string &pad_name) const;

    private:
        /*
         * Create required elements and add them to the bin
         */
        virtual void init() {
        };

        /*
         * Link elements together
         */
        virtual void link() {
        }
    };
}
#endif //BIN_H
