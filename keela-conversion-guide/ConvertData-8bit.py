import subprocess
import numpy as np
import json
import sys
import os
from scipy.io import savemat

raw_path = str(sys.argv[1])

def extract_luma_3d(path):
    # get width, height, and framecount
    probe = subprocess.check_output([
        "ffprobe","-v","error",
        "-select_streams","v:0",
        "-count_frames",
        "-show_entries","stream=width,height,nb_frames",
        "-show_entries", "stream=nb_read_frames",
        "-of","json", path
    ])
    
    info = json.loads(probe)["streams"][0]
    W, H = int(info["width"]), int(info["height"])
    T = int(info["nb_read_frames"])

    # run ffmpeg to extract Y plane as 8‑bit grayscale
    cmd = [
        "ffmpeg",
        "-i", path,
        "-map", "0:v:0",
        "-vsync","0",
        "-vf", "extractplanes=y",
        "-pix_fmt", "gray8",
        "-f", "rawvideo",
        "-hide_banner", "-loglevel", "error",
        "-"
    ]

    # read raw bytes
    frame_bytes = W * H * 1                         # 1 for uint8
    raw = subprocess.check_output(cmd)

    # convert to array
    arr = np.frombuffer(raw, dtype="<u1")           # 8-bit gray
    arr = arr.reshape((T, H, W))

    return arr                                      # uint8, values 0–255 for 8-bit Y


def extract_filename(s: str) -> str:
    if not isinstance(s, str):
        raise TypeError("Input must be a string.")

    last_slash = s.rfind('/')
    last_dot = s.rfind('.')

    if last_slash == -1 or last_dot == -1 or last_dot <= last_slash + 1:
        return ""

    return s[last_slash + 1:last_dot]

def extract_pathway(s: str) -> str:
    if not isinstance(s, str):
        raise TypeError("Input must be a string.")

    last_slash = s.rfind('/')

    if last_slash == -1:
        return ""

    return s[0:last_slash]


original_data = extract_luma_3d(raw_path)

float_data = original_data.astype(np.float32)

new_path = extract_pathway(raw_path) + '/MatFiles'

try:
    os.mkdir(new_path)
    print("Directory '%s' created!" % new_path)
except FileExistsError:
    print("Directory '%s' already exists" % new_path)

# Save the array to a .mat file
savemat(new_path + '/' + extract_filename(raw_path) + '.mat', {'data': float_data})
