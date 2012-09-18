
INDIR = source
TEMPDIR = .temp
OUTDIR = www

INPUT = $(wildcard $(INDIR)/*.md) $(wildcard $(INDIR)/**/*.md) $(wildcard $(INDIR)/**/**/*.md)
GENERATED = $(patsubst $(INDIR)/%.md,$(TEMPDIR)/%.html,$(INPUT))
POSTPROCESSED = $(patsubst $(TEMPDIR)/%.html,$(OUTDIR)/%.html,$(GENERATED))

all: environment generate postprocess copy

environment:
	mkdir -p $(TEMPDIR)
	mkdir -p $(OUTDIR)
	cd $(TEMPDIR); (cd ../$(INDIR); find -type d ! -name .) | xargs mkdir -p
	cd $(OUTDIR); (cd ../$(INDIR); find -type d ! -name .) | xargs mkdir -p

generate: environment $(GENERATED)

postprocess: generate $(POSTPROCESSED)

# Copy static files
copy:
	cp -r static/* www/

show:
	firefox www/ &

# Convert markdown files to HTML
$(TEMPDIR)/%.html: $(INDIR)/%.md template/default.html
	pandoc --template=template/default.html -s -o $@ $<

# Post process the HTML files to add some additional markup
$(OUTDIR)/%.html: $(TEMPDIR)/%.html tools/postprocess.py
	python tools/postprocess.py -p $(TEMPDIR) -s $(INDIR) $< $@

clean:
	rm -rf $(TEMPDIR)
	rm -rf $(OUTDIR)

.PHONY : copy clean
