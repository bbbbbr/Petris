PROJECT_NAME = Petris

export GBDK_HOME="/home/birch/git/gbdev/gbdk2020/gbdk-2020-git/build/gbdk"
export BGB="../tools/bgbw64-1.5.8/bgb64.exe"
# export MOD2GBT="../tools/mod2gbt/mod2gbt.exe"
export MOD2GBT="tools/mod2gbt_withunbanked"
export MOD2GBT_SINGLEARRAY="tools/mod2gbt_singlearray_withunbanked"
export APULTRA="tools/apultra"
export BIN2C="tools/bin2c"
export GBXFLASHER="tools/gbxcart_rw_flasher_v1.34"

all: build_gb

run:

clean:

# Fixed Bank 0 + Bank 1 that are a contiguous 32K (_CODE)
# i.e. no MBC and no banking
# (Do *not* use _CODE_1 for Bank 1 since that will create an error)
N_BANKS = 2

include src/MakefileCommon
