DEFINES=PROJECT_CONF_H=\"project-conf.h\"

ifndef TARGET
TARGET = z1
endif

CONTIKI = ../..

CONTIKI_PROJECT = channel-interferencer

all: $(CONTIKI_PROJECT)
	
CONTIKI_WITH_RIME = 1
include $(CONTIKI)/Makefile.include

