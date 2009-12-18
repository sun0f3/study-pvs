# Main make file



target := pop3s
TARGET_PORT := 8087
BIN_DIR := $(PWD)/bin
SOURCE_DIR := $(PWD)/src
BUILD_DIR := $(PWD)/tmp
REPORT_DIR := $(PWD)/report
MAILDROP_DIR := $(PWD)/maildir
SCENARIOS_DIR := $(PWD)/scenarios
SCENARIOS_TEX := scenarios.tex
LOG_MAIN := $(BUILD_DIR)/server_current.log
LOG_DROP := $(BUILD_DIR)/server_dropped.log
LIBS := -lpthread -lrt

TESTER := python $(PWD)/scenstarter.py
STOPPER := python $(PWD)/pop3starter.py stop
DROPPER := python $(PWD)/pop3starter.py drop
test_scenarios := $(patsubst $(SCENARIOS_DIR)/%, %, $(wildcard  $(SCENARIOS_DIR)/*.scn))
source_files := $(wildcard  $(SOURCE_DIR)/*.c)
object_files := $(source_files:.c=.o)
simplers_cflows := $(wildcard  $(REPORT_DIR)/*.scf)
cflows := $(simplers_cflows:.scf=_scf.dot)
CFLOW := cflow --level "0= "
CFLOW2DOT := python $(PWD)/cflow2dot.py
SIMPLECFLOW := grep -v -f 
MAKE_GRAPHER := python ./scripts/mkf2dot.py dump

.PHONY: all report clean tests 


