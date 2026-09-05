# Early makefile for Casio Loopy (Kasami, August 2023)
# Based on Wonderful Toolchain package for SuperH

# Normal use: "make clean && make"

# Memory sizes; should use K/M suffix or decimal integer
# Cart battery-backed save RAM, most have at least 8K if any
# "Floopy Drive" flash cart has 128K
SRAMSIZE = 8K
# Allocated stack size, increase if necessary
STACKSIZE = 2K

EMU=LoopyMSE
# Toolchain programs
WONDERFUL_TOOLCHAIN ?= /opt/wonderful
TOOLBIN ?= $(WONDERFUL_TOOLCHAIN)/toolchain/gcc-sh-elf/bin/
#TOOLBIN =
PREFIX ?= sh-elf-
#PREFIX = sh1-none-elf-
CC  = $(TOOLBIN)$(PREFIX)gcc
CXX = $(TOOLBIN)$(PREFIX)g++
LD  = $(TOOLBIN)$(PREFIX)ld
OBJ = $(TOOLBIN)$(PREFIX)objcopy

# File manipulation progs
MV     = mv
MKDIR  = mkdir -p
RMDIR  = rm -rf
FIXROM = /usr/bin/env python3 $(TOOLSDIR)/fixrom.py # Change to "python" if necessary

# File/dir locations
TOOLSDIR = ./tools
SRCDIR = ./src
BINDIR = ./build
INCDIR = ./inc
OBJDIR = ./obj
RESDIR = ./res
RES_SRCDIR    = $(RESDIR)
GBDK_SRCDIR   = $(SRCDIR)/gbdk
LOOPY_SRCDIR   = $(SRCDIR)/loopy
# Resource source asset:
RES_ASSET_DIR     = $(RESDIR)/exported
RES_GENERATED_DIR = $(RESDIR)/generated
# Resulting binary
ROM    = $(BINDIR)/petris.bin

# Basic compile options
OPTIMIZE = -Os
LIBS =

# Below here probably doesn't need to be touched

LDSCRIPT = $(TOOLSDIR)/loopy.ld

# C Source/object lists
SRCS_C = $(wildcard $(SRCDIR)/*.c)
OBJS_C = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS_C))
RES_SRCS_C = $(wildcard $(RES_SRCDIR)/*.c)
RES_OBJS_C = $(patsubst $(RES_SRCDIR)/%.c,$(OBJDIR)/%.o,$(RES_SRCS_C))
GBDK_SRCS_C = $(wildcard $(GBDK_SRCDIR)/*.c)
GBDK_OBJS_C = $(patsubst $(GBDK_SRCDIR)/%.c,$(OBJDIR)/%.o,$(GBDK_SRCS_C))
LOOPY_SRCS_C = $(wildcard $(LOOPY_SRCDIR)/*.c)
LOOPY_OBJS_C = $(patsubst $(LOOPY_SRCDIR)/%.c,$(OBJDIR)/%.o,$(LOOPY_SRCS_C))

# Asm Source/object lists
SRCS_S = $(wildcard $(SRCDIR)/*.s)
OBJS_S = $(patsubst $(SRCDIR)/%.s,$(OBJDIR)/%.o,$(SRCS_S))
LOOPY_SRCS_S = $(wildcard $(LOOPY_SRCDIR)/*.s)
LOOPY_OBJS_S = $(patsubst $(LOOPY_SRCDIR)/%.s,$(OBJDIR)/%.o,$(LOOPY_SRCS_S))

CFLAGS  = $(OPTIMIZE) -g -gdwarf-4
CFLAGS += -m1 -mrenesas
CFLAGS += -ffreestanding
CFLAGS += -falign-functions=4 -ffunction-sections -fdata-sections
CFLAGS += -fomit-frame-pointer -fno-asynchronous-unwind-tables -fno-unwind-tables
CFLAGS += -Wstack-usage=$(shell numfmt --from=iec $(STACKSIZE)) -I$(INCDIR) -I$(RES_SRCDIR)

CXXFLAGS = -std=c++23 -fno-exceptions -fno-non-call-exceptions -fno-rtti -fno-threadsafe-statics

SIZEDEFS  = -Wl,--defsym=SRAMSIZE=$(SRAMSIZE)
SIZEDEFS += -Wl,--defsym=STACKSIZE=$(STACKSIZE)

LDFLAGS  = -nostartfiles -nolibc -Wl,--gc-sections -Wl,--no-warn-rwx-segment -Wl,--orphan-handling=error -Wl,--print-memory-usage
LDFLAGS += $(SIZEDEFS) -Wl,-T $(LDSCRIPT) $(LIBS)

.PHONY: clean rom

all: rom

rom: $(ROM)

%.elf:
	$(CC) $(LDFLAGS) $^ -o $@

%.bin: %.elf
	$(OBJ) -O binary $< $@
	$(FIXROM) $(ROM)

$(ROM:.bin=.elf): $(OBJS_S) $(LOOPY_OBJS_S) $(OBJS_C) $(RES_OBJS_C) $(GBDK_OBJS_C) $(LOOPY_OBJS_C)

$(OBJDIR)/%.o: $(SRCDIR)/%.s | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJDIR)/%.o: $(LOOPY_SRCDIR)/%.s | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@


$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CFLAGS) $(CXXFLAGS) -c $< -o $@
$(OBJDIR)/%.o: $(RES_SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJDIR)/%.o: $(GBDK_SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJDIR)/%.o: $(LOOPY_SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@


$(OBJDIR):
	$(MKDIR) $@

run:
	$(EMU) $(ROM)

clean:
	$(RM) $(BINDIR)/*
	$(RM) $(OBJDIR)/*

assets-clean:
	rm -f $(RESDIR)/*.c $(RESDIR)/*.h $(RESDIR)/*.png

# TODO: detection of changes and auto rebuilding from that
assets:
	$(TOOLSDIR)/tilepalquant $(RES_ASSET_DIR)/intro_cat.png -o $(RES_GENERATED_DIR)/intro_cat_out.png -use_metafile
	$(TOOLSDIR)/png2asset $(RES_GENERATED_DIR)/intro_cat_out.png -o $(RESDIR)/intro_cat_out.c -use_metafile
	$(TOOLSDIR)/tilepalquant $(RES_ASSET_DIR)/intro_screen.png -o $(RES_GENERATED_DIR)/intro_screen_out.png -use_metafile
	$(TOOLSDIR)/png2asset $(RES_GENERATED_DIR)/intro_screen_out.png -o $(RESDIR)/intro_screen_out.c -use_metafile
	$(TOOLSDIR)/tilepalquant $(RES_ASSET_DIR)/intro_logo.png -o $(RES_GENERATED_DIR)/intro_logo_out.png -use_metafile
	$(TOOLSDIR)/png2asset $(RES_GENERATED_DIR)/intro_logo_out.png -o $(RESDIR)/intro_logo_out.c -use_metafile
	$(TOOLSDIR)/tilepalquant $(RES_ASSET_DIR)/font_8x16.png -o $(RES_GENERATED_DIR)/font_8x16_out.png -use_metafile
	$(TOOLSDIR)/png2asset $(RES_GENERATED_DIR)/font_8x16_out.png -o $(RESDIR)/font_8x16_out.c -use_metafile
