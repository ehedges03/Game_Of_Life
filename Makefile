# Alternative GNU Make workspace makefile autogenerated by Premake

ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

ifeq ($(config),debug)
  Libraries_config = debug
  App_config = debug

else ifeq ($(config),release)
  Libraries_config = release
  App_config = release

else ifeq ($(config),dist)
  Libraries_config = dist
  App_config = dist

else
  $(error "invalid configuration $(config)")
endif

PROJECTS := Libraries App

.PHONY: all clean help $(PROJECTS) Libraries

all: $(PROJECTS)

Libraries: Libraries

Libraries:
ifneq (,$(Libraries_config))
	@echo "==== Building Libraries ($(Libraries_config)) ===="
	@${MAKE} --no-print-directory -C Libraries -f Makefile config=$(Libraries_config)
endif

App: Libraries
ifneq (,$(App_config))
	@echo "==== Building App ($(App_config)) ===="
	@${MAKE} --no-print-directory -C App -f Makefile config=$(App_config)
endif

clean:
	@${MAKE} --no-print-directory -C Libraries -f Makefile clean
	@${MAKE} --no-print-directory -C App -f Makefile clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "  debug"
	@echo "  release"
	@echo "  dist"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   Libraries"
	@echo "   App"
	@echo ""
	@echo "For more information, see https://github.com/premake/premake-core/wiki"