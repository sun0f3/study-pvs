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



all: $(object_files)
	gcc $(subst $(SOURCE_DIR), $(BUILD_DIR), $^) $(LIBS) -o $(target)
	mv $(target) $(BIN_DIR)/$(target)
	
%.o: %.c
	gcc -o $(subst $(SOURCE_DIR), $(BUILD_DIR), $@) -c $< 

%_scf.dot: %.scf
	$(CFLOW) $(source_files) | $(CFLOW2DOT) > $(BUILD_DIR)/$(notdir $@)

report: $(cflows)
	@cat  $(PWD)/Makefile | $(MAKE_GRAPHER) > $(BUILD_DIR)/make_main.dot
	@cat  $(REPORT_DIR)/Makefile | $(MAKE_GRAPHER) > $(BUILD_DIR)/make_report.dot
	$(MAKE) -C $(REPORT_DIR)

tests: start_target $(test_scenarios) stop_target

start_target:
	$(BIN_DIR)/$(target) $(TARGET_PORT) $(LOG_MAIN) $(LOG_DROP) &
	$(DROPPER)
	@echo "" > $(BUILD_DIR)/$(SCENARIOS_TEX)

stop_target:
	$(STOPPER)

%.scn:
	-@rm -rf $(MAILDROP_DIR)/*
	-$(TESTER) $(TARGET_PORT) $(SCENARIOS_DIR)/$@ > $(BUILD_DIR)/$(@:.scn=.tex)
	@echo "\\input{$(BUILD_DIR)/$(@:.scn=.tex)}" >> $(BUILD_DIR)/$(SCENARIOS_TEX)
	$(DROPPER) && sleep 1
	@cp $(LOG_DROP) $(BUILD_DIR)/$(@:.scn=.log).tex
	@echo "\\end{verbatim}" >> $(BUILD_DIR)/$(@:.scn=.log).tex

clean:
	-@rm -f $(BUILD_DIR)/*
	-@rm -f $(BIN_DIR)/$(target)


