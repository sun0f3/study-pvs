#
# Генератор отчёта по pop3s
#

# основная цель
SOURCE_DIR := $(PWD)/report
TARGET := report
PDF := $(SOURCE_DIR)/$(TARGET).pdf
BIN_DIR := $(PWD)/bin
BUILD_DIR := $(PWD)/tmp
dot_graphs := $(wildcard  $(SOURCE_DIR)/*.dot) $(wildcard  $(BUILD_DIR)/*.dot)
dot_output := $(dot_graphs:.dot=.png)

# Используемые программы
#DIA=dia -t png 
PDFLATEX = pdflatex -interaction=nonstopmode

.PHONY: all 


all : clean $(dot_output) pdf

%.png: %.dot
	cat $< | dot -Tpng > $(BUILD_DIR)/$(notdir $@)


pdf: $(PDF)

# Основная цель
$(PDF) :
	@cd $(BUILD_DIR)/ && export TEXINPUTS=.:$(SOURCE_DIR): && export && $(PDFLATEX) $(@:.pdf=.tex) && mv ./$(TARGET).pdf $(BIN_DIR)/$(TARGET).pdf
	sleep 10
	


# Удаление ВСЕХ не-исходных файло
clean:
	-@rm -f $(BIN_DIR)/$(TARGET).pdf
	cd $(BUILD_DIR)/ && find . -type f -name "*.toc" -exec rm -f {} \;

