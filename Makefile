PROJECT_NAME = Petris

export GBDK_HOME="/opt/gbdk"
export BGB="../tools/bgbw64-1.5.7/bgb64.exe"
# export MOD2GBT="../tools/mod2gbt/mod2gbt.exe"
# export MOD2GBT="../tools/mod2gbt_withunbanked/mod2gbt"
export MOD2GBT="tools/mod2gbt_withunbanked"

all: build_gb

run:

clean:

# Fixed Bank 0 + Bank 1 that are a contiguous 32K (_CODE)
# i.e. no MBC and no banking
# (Do *not* use _CODE_1 for Bank 1 since that will create an error)
N_BANKS = 2

include src/MakefileCommon
