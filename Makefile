PROJECT_NAME = PETRIS

export GBDK_HOME="/opt/gbdk"
export BGB="../tools/bgbw64-1.5.7/bgb64.exe"
export MOD2GBT="../mod2gbt.exe"


all: build_gb

run:

clean:

N_BANKS = 2

include src/MakefileCommon
