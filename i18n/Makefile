LOCALE = ja_JP.UTF-8

all: $(LOCALE)/LC_MESSAGES/messages.mo $(LOCALE)/LC_MESSAGES/gamedata.mo

%.mo: %.po
	msgfmt $< -o $@

$(LOCALE)/LC_MESSAGES/gamedata.po: \
		$(LOCALE)/LC_MESSAGES/_gamedata-class.po \
		$(LOCALE)/LC_MESSAGES/_gamedata-monster.po \
		$(LOCALE)/LC_MESSAGES/_gamedata-p_race.po
	xgettext $^ -o - | grep -v POT-Creation-Date: > $@
	chmod -w $@

$(LOCALE)/LC_MESSAGES/%.po: %.pot
	\
if [ -f "$@" ]; then \
  msgmerge --update $@ $<; \
else \
  msginit --no-translator --locale ja_JP.UTF-8 --input $< --output-file $@; \
fi

messages.pot:
	xgettext --omit-header -k_ -o $@ ../src/*.c

_gamedata-%.pot:
	../utils/gamedata2pot $* > $@
