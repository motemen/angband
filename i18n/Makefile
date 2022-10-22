LOCALE = ja_JP

GAMEDATA_POTS = \
		_gamedata-class.pot \
		_gamedata-monster.pot \
		_gamedata-p_race.pot \
		_gamedata-blow_methods.pot \
		_gamedata-blow_effects.pot \
		_gamedata-source.pot \
		_gamedata-object.pot \
		_gamedata-object_base.pot \
		_gamedata-artifact.pot \
		_gamedata-terrain.pot

.PHONY: all
all: $(LOCALE)/LC_MESSAGES/messages.mo $(LOCALE)/LC_MESSAGES/gamedata.mo

.PHONY: all-pots
all-pots: $(GAMEDATA_POTS)

%.mo: %.po
	msgfmt $< -o $@

$(LOCALE)/LC_MESSAGES/messages.mo: $(LOCALE)/LC_MESSAGES/messages.po

$(LOCALE)/LC_MESSAGES/gamedata.po: \
		$(patsubst %.pot,$(LOCALE)/LC_MESSAGES/%.po,$(GAMEDATA_POTS))

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
	chmod +w $@ || true
	xgettext --omit-header -k_ -kN_ -k_C:1c,2 -o $@ ../src/*.c
	chmod -w $@

_gamedata-%.pot:
	chmod +w $@ || true
	../utils/gamedata2pot $* > $@
	chmod -w $@

_gamedata-source.pot:
	chmod +w $@ || true
	xgettext --omit-header -k_GAMEDATA -kN_GAMEDATA -k -o $@ ../src/*.c ../src/*.h
	chmod -w $@
