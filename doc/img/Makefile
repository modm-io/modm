
SRC = logo.pic

# output format
#FORMAT = eps
FORMAT = pdf
#FORMAT = png

OBJECTS = $(SRC:%.pic=%.$(FORMAT))

SHELL = sh
REMOVE = rm -f
REMOVEDIR = rm -rf
COPY = cp

# messages
MSG_SEQUENCE_DIAGRAM = Creating Sequence Diagram

all: diagrams

diagrams: $(OBJECTS)

# -F Helvetica
%.ps: %.pic
	pic2plot -Tps $< > $*.ps

%.png: %.pic
	pic2plot -Tpng $< > $*.png

%.eps: %.ps
	ps2epsi $<
	mv $*.epsi $*.eps

%.pdf: %.eps
	epstopdf $<

# cleanup
clean:
	@echo
	$(REMOVE) $(SRC:%.pic=%.$(FORMAT))
	$(REMOVE) $(SRC:%.pic=%.eps)

# Listing of phony targets.
.PHONY : all graph
