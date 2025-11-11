#include "keela-widgets/cameratrace.h"

Keela::CameraTrace::CameraTrace(std::shared_ptr<TraceBin> trace_bin, std::shared_ptr<TraceGizmo> trace_gizmo,
                                const std::string& name)
    : m_trace_bin(std::move(trace_bin)), m_trace_gizmo(std::move(trace_gizmo)), m_name(name) {
}

std::shared_ptr<Keela::TraceBin> Keela::CameraTrace::get_trace_bin() {
	return m_trace_bin;
}

std::shared_ptr<Keela::TraceGizmo> Keela::CameraTrace::get_trace_gizmo() {
	return m_trace_gizmo;
}

std::string Keela::CameraTrace::get_name() {
	return m_name;
}