%.ml: %.iml
	imandra-extract -o $@ $<

%.re: %.ire
	imandra-extract -reason -o $@ $<

IMLS = $(shell find . -type f -iname '*.iml')
IRES = $(shell find . -type f -iname '*.ire')
MLS = $(patsubst %.iml,%.ml,$(IMLS))
RES = $(patsubst %.ire,%.re,$(IRES))

.PHONY: all
all: $(MLS) $(RES)

clean:
	rm -f ${MLS} ${RES}
