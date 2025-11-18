# Makefile to build all ESP-IDF projects

EXAMPLES = hello_world blink basic_freertos_smp_usage assembly
TARGET = esp32c6
IDF_PATH ?= $(HOME)/esp/esp-idf

.PHONY: all clean $(EXAMPLES)

all: $(EXAMPLES)

$(EXAMPLES):
	@echo "Building $@ for target $(TARGET)..."	
	@cd examples/$@ && cd $(IDF_PATH) && . ./export.sh && cd - && idf.py set-target $(TARGET) && idf.py build

clean:
	@cd $(IDF_PATH) && . ./export.sh && cd -
	@for example in $(EXAMPLES); do \
		echo "Cleaning $$example..."; \
		cd examples/$$example && idf.py clean && rm -rf build 2>/dev/null || true && cd ../..; \
	done