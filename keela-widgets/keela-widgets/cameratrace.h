#ifndef CAMERATRACE_H
#define CAMERATRACE_H

#include "GLTraceRender.h"

namespace Keela {
/**
 * CameraTrace is a simple implementation of ITraceable that holds references
 * to a TraceBin and TraceGizmo. It acts as a data holder for trace components.
 */
class CameraTrace final : public ITraceable {
   public:
	CameraTrace(std::shared_ptr<TraceBin> trace_bin, std::shared_ptr<TraceGizmo> trace_gizmo, const std::string& name);
	~CameraTrace() override = default;

	std::shared_ptr<TraceBin> get_trace_bin() override;
	std::shared_ptr<TraceGizmo> get_trace_gizmo() override;
	std::string get_name() override;

   private:
	std::shared_ptr<TraceBin> m_trace_bin;
	std::shared_ptr<TraceGizmo> m_trace_gizmo;
	std::string m_name;
};
}  // namespace Keela

#endif  // CAMERATRACE_H