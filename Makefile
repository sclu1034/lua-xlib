PROJECT = xlib
PREFIX ?= /usr/local
BUILD_DIR = out

LUA_VERSION ?= 5.1
LUA ?= $(shell command -v lua$(LUA_VERSION))
LUA_BINDIR ?= /usr/bin
LUA_LIBDIR ?= /usr/lib/x86_64-linux-gnu/lua/$(LUA_VERSION)
LUA_INCDIR ?= /usr/include/lua$(LUA_VERSION)

ifdef LIBDIR
INSTALL_LIBDIR ?= $(LIBDIR)
else
INSTALL_LIBDIR ?= $(PREFIX)/lib/lua/$(LUA_VERSION)
endif

ifdef LUADIR
INSTALL_LUADIR ?= $(LUADIR)
else
INSTALL_LUADIR ?= $(PREFIX)/share/lua/$(LUA_VERSION)
endif

ifdef DOCDIR
INSTALL_DOCDIR ?= $(DOCDIR)
else
INSTALL_DOCDIR ?= $(PREFIX)/share/doc/$(PROJECT)
endif

CC ?= gcc
PKG_CONFIG ?= pkg-config

PKGS = x11 xrandr lua$(LUA_VERSION)

CFLAGS ?= -fPIC
LIBFLAG ?= -shared

CCFLAGS ?= $(CFLAGS)
CCFLAGS += -Og -Wall -g -rdynamic $(shell $(PKG_CONFIG) --cflags $(PKGS)) -I$(LUA_INCDIR) -I"./"

ifdef CI
CHECK_ARGS ?= --formatter TAP
TEST_ARGS ?= --output=TAP

# In CI, not all packages are available with `.pc`s.
# They are handled specially.
PKGS = x11 xrandr

CCFLAGS += -Werror
endif

# Override debug-optimized build with performance optimization
ifeq "$(PROFILE)" "release"
CCFLAGS += -O2 -DNDEBUG
else
CCFLAGS += -DLUA_USE_APICHECK
endif

LIBS = -L$(shell dirname "$(shell $(CC) -print-libgcc-file-name)") -L"$(LUA_LIBDIR)" -L"./"
LIBS += $(shell $(PKG_CONFIG) --libs $(PKGS))
OBJS = $(shell find src -type f -iname '*.c' | sed 's/\(.*\)\.c$$/$(BUILD_DIR)\/\1\.o/')

TARGET = $(BUILD_DIR)/$(PROJECT).so

.PHONY: clean doc doc-content doc-styles install test check rock

build: $(TARGET)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(shell dirname "$@")
	@echo "\033[1;97m$(CC) $< -o $@\033[0m"
	@$(CC) -c $(CCFLAGS) $< -o $@

$(TARGET): $(OBJS)
	@echo "\033[1;97m$(CC) -o $@\033[0m"
	@$(CC) $(LIBFLAG) -o $@ $(OBJS) $(LIBS)

doc-styles:
	@echo "\033[1;97mGenerate stylesheet\033[0m"
	sass doc/ldoc.scss $(BUILD_DIR)/doc/ldoc.css

doc-content:
	@mkdir -p "$(BUILD_DIR)/doc" "$(BUILD_DIR)/src"
	@echo "\033[1;97mPreprocess sources\033[0m"
	sh tools/process_docs.sh "$(BUILD_DIR)"
	@echo "\033[1;97mGenerate documentation\033[0m"
	ldoc --config=doc/config.ld --dir "$(BUILD_DIR)/doc" --project $(PROJECT) "$(BUILD_DIR)/src"

doc: doc-content doc-styles
ifdef CI
	touch "$(BUILD_DIR)/doc/.nojekyll"
endif

clean:
	rm -r out/

install: build doc
	@echo "\033[1;97mInstall C library\033[0m"
	install -vDm 644 -t $(INSTALL_LIBDIR)/$(PROJECT) $(TARGET)

	@echo "\033[1;97mInstall documentation\033[0m"
	install -vd $(INSTALL_DOCDIR)
	cp -vr $(BUILD_DIR)/doc/* $(INSTALL_DOCDIR)

uninstall:
	rm $(INSTALL_LIBDIR)/$(PROJECT)
	rm -r $(INSTALL_DOCDIR)

check:
	@echo "Nothing to do"

test:
	busted --config-file=.busted.lua --lua=$(LUA) $(TEST_ARGS)

rock:
	luarocks --local --lua-version $(LUA_VERSION) make rocks/lua-xlib-scm-2.rockspec

run: build
	env LUA_CPATH="./out/?.so;$${LUA_CPATH}" $(LUA) test.lua

repl: build
	env LUA_CPATH="./out/?.so;$${LUA_CPATH}" $(LUA)

debug: build
	env LUA_CPATH="./out/?.so;$${LUA_CPATH}" gdb $(LUA)
