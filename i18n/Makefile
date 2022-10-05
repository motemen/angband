LOCALE = ja_JP

all: $(LOCALE)/LC_MESSAGES/messages.mo $(LOCALE)/LC_MESSAGES/gamedata.mo

%.mo: %.po
	msgfmt $< -o $@

$(LOCALE)/LC_MESSAGES/messages.mo: $(LOCALE)/LC_MESSAGES/messages.po

$(LOCALE)/LC_MESSAGES/gamedata.po: \
		$(LOCALE)/LC_MESSAGES/_gamedata-class.po \
		$(LOCALE)/LC_MESSAGES/_gamedata-monster.po \
		$(LOCALE)/LC_MESSAGES/_gamedata-p_race.po \
		$(LOCALE)/LC_MESSAGES/_gamedata-blow_methods.po \
		$(LOCALE)/LC_MESSAGES/_gamedata-source.po

	chmod +w $@
	xgettext $^ -o - | grep -v POT-Creation-Date: > $@
	chmod -w $@

$(LOCALE)/LC_MESSAGES/%.po: %.pot
	\
if [ -f "$@" ]; then \
  msgmerge --update $@ $<; \
else \
  msginit --no-translator --locale ja_JP.UTF-8 --input $< --output-file $@; \
fi

messages.pot: $(wildcard ../src/*.c)
	xgettext --omit-header -k_ -o $@ ../src/*.[ch]

_gamedata-%.pot:
	../utils/gamedata2pot $* > $@

_gamedata-source.pot:
	xgettext --omit-header -kGAMEDATA_ -kGAMEDATA_N_ -o $@ ../src/*.[ch]
