//
// Created by brand on 5/26/2025.
//

#ifndef RECORDBIN_H
#define RECORDBIN_H
#include "FFV1EncodeBin.h"
#include "VideoEncoder.h"
#include "bin.h"
#include "h264EncodeBin.h"
#include "queuebin.h"
#include "simpleelement.h"

namespace Keela {
class RecordBin final : public QueueBin, public EjectableElement {
   public:
	explicit RecordBin(const std::string &name);

	void set_directory(const std::string &full_filename);

	~RecordBin() override;

   private:
#ifdef KEELA_USE_FFV1
	Keela::FFV1EncodeBin enc;
#endif
#ifndef KEELA_USE_FFV1

	Keela::H264EncodeBin enc;
#endif

	Keela::SimpleElement mux;
	Keela::SimpleElement sink;

	void link() override;

	void init() override;

	Keela::Element *Head() override {
		return &queue;
	};

	std::vector<Keela::Element *> Leaves() override {
		std::vector<Keela::Element *> ret;
		ret.push_back(&mux);
		return ret;
	}
};
}  // namespace Keela
#endif  // RECORDBIN_H
