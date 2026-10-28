# Keela videotestsrc

This document describes how to build and use the custom `keelavideotestsrc` GStreamer plugin that generates a bouncing ball pattern with alternating black/white backgrounds. The goal is to have a test source that mimics the experimental setup where we are recording different measurements in alternating video frames.

### Technical Specs

- **Default Resolution**: 320x240 (can be negotiated)
- **Default Frame Rate**: 500 FPS (can be negotiated)
- **Format**: GRAY8 (grayscale)
- **Ball Size**: 40 pixel diameter (20 pixel radius)
- **Movement Pattern**: X: `sin(time * 2.0)`, Y: `sin(time * 1.5)`

## Building the Plugin

### Prerequisites

- GStreamer 1.8+ development packages
- CMake 3.10+
- GCC compiler

### Build Commands

```bash
# From project root directory
cd build
make gstkeelavideotestsrc
```

The plugin will be built as `build/plugins/libkeelavideotestsrc.so`.

## Testing the Plugin

### 1. Inspect Plugin Information

```bash
export GST_PLUGIN_PATH=$(pwd)/build/plugins
gst-inspect-1.0 keelavideotestsrc
```

This will show:

- Plugin name: `keelavideotestsrc`
- Element name: `keelavideotestsrc`
- Description: "Keela Ball Test Source"
- Available properties and capabilities

### 2. Test with gst-launch

```bash
export GST_PLUGIN_PATH=$(pwd)/build/plugins
gst-launch-1.0 keelavideotestsrc ! videoconvert ! autovideosink
```

**Note**: The `videoconvert` element is required because the plugin outputs GRAY8 format, which needs conversion for most video sinks.

### 3. Test at High Frame Rate

```bash
export GST_PLUGIN_PATH=$(pwd)/build/plugins
gst-launch-1.0 keelavideotestsrc ! video/x-raw,format=GRAY8,width=320,height=240,framerate=500/1 ! videoconvert ! autovideosink
```

### 4. Test with Keela Application

```bash
export GST_PLUGIN_PATH=$(pwd)/build/plugins
./build/keela-exe/keela
```

## Integration with Keela Application

The plugin is automatically used by the Keela application when the `GST_PLUGIN_PATH` environment variable is set. The application:

1. Scans for custom plugins in the plugin path
2. Uses `keelavideotestsrc` if available
3. Falls back to standard `videotestsrc` if not found

No code changes needed - just set the environment variable!
