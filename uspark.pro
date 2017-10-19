TEMPLATE = subdirs

CONFIG = ordered

# configuration
include(config.pri)

SUBDIRS += \
    spark \
    samples/01_sdl_test \
    samples/02_urho_test \
    samples/03_realscene \
