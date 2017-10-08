CFILES += main.c utils.c aeffects.c control.c led_control.c button_control.c presence_control.c drivers/gpio.c drivers/i2c.c drivers/mt8809.c drivers/pcf857x.c
CFLAGS += -std=gnu11 -ggdb -Wall -Iinc -Idrivers/inc -fPIC
LFLAGS += -lpthread
OUTDIR ?= .

debug: CFLAGS += -DDEBUG -O0
debug: aeffects
release: CFLAGS += -DNDEBUG -O2
release: aeffects

$(OUTDIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) 

aeffects: $(OUTDIR)/$(patsubst %.c,%.o,$(CFILES))
	$(CC) -o aeffects.so $^ $(LFLAGS) -shared
	$(CC) -o aeffects $^ $(LFLAGS)

.PHONY: clean
clean: 
	@rm -f $(OUTDIR)/$(patsubst %.c,$(OUTDIR)/%.o,$(CFILES)) $(OUTDIR)/aeffects $(OUTDIR)/aeffects.so
