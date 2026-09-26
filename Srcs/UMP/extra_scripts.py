Import("env")
from SCons.Script import DefaultEnvironment

global_env = DefaultEnvironment()
global_env.Append(CPPPATH=["$PROJECT_DIR/lib/pico-pio-usb/src"])
env.Append(CPPPATH=["$PROJECT_DIR/lib/pico-pio-usb/src"])

# pico_stdio_usb の stdio_usb_descriptors.c は自前の usb_descriptors.c と衝突するため除外
def exclude_stdio_usb_descriptors(env, node):
    if "stdio_usb_descriptors" in str(node):
        return None
    return node

global_env.AddBuildMiddleware(exclude_stdio_usb_descriptors)
env.AddBuildMiddleware(exclude_stdio_usb_descriptors)
