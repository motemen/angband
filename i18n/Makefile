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
		_gamedata-terrain.pot \
		_gamedata-body.pot \
		_gamedata-list-equip-slots.pot \
		_gamedata-ego_item.pot \
		_gamedata-list-mon-message.pot \
		_gamedata-player_timed.pot \
		_gamedata-pain.pot \
		_gamedata-trap.pot \
		_gamedata-ui_entry.pot \
		_gamedata-player_property.pot \
		_gamedata-list-mon-race-flags.pot \
		_gamedata-brand.pot

.PHONY: all
all: all-pots $(LOCALE)/LC_MESSAGES/messages.mo $(LOCALE)/LC_MESSAGES/gamedata.mo

.PHONY: all-pots
all-pots: messages.pot $(GAMEDATA_POTS)

.PHONY: clean
clean:
	rm -f messages.pot $(GAMEDATA_POTS)

%.mo: %.po
	msgfmt $< -o $@

$(LOCALE)/LC_MESSAGES/messages.mo: $(LOCALE)/LC_MESSAGES/messages.po

$(LOCALE)/LC_MESSAGES/gamedata.mo: $(LOCALE)/LC_MESSAGES/gamedata.po

$(LOCALE)/LC_MESSAGES/gamedata.po: \
		$(patsubst %.pot,$(LOCALE)/LC_MESSAGES/%.po,$(GAMEDATA_POTS))
	chmod +w $@ || true
	xgettext $^ -o - | grep -v POT-Creation-Date: > $@
	chmod -w $@

$(LOCALE)/LC_MESSAGES/%.po: %.pot
	\
if [ -f "$@" ]; then \
  msgmerge --update $@ $<; \
else \
  msginit --no-translator --locale ja_JP.UTF-8 --input $< --output-file $@; \
fi

messages.pot: $(wildcard ../src/*.c ../src/list-options.h)
	chmod +w $@ || true
	xgettext --omit-header -k_ -kN_ -k_C:1c,2 -o $@ ../src/*.c ../src/list-options.h
	chmod -w $@

_gamedata-list-%.pot: ../src/list-%.h
	chmod +w $@ || true
	xgettext --omit-header -kN_GAMEDATA -o $@ ../src/list-$*.h
	perl -i -ple 'print qq(msgctxt "list-$*") if /^msgid /' $@
	chmod -w $@

_gamedata-%.pot:
	chmod +w $@ || true
	../utils/gamedata2pot $* > $@
	chmod -w $@

_gamedata-source.pot:
	chmod +w $@ || true
	xgettext --omit-header -k_GAMEDATA -k_GAMEDATA_C:1c,2 -kN_GAMEDATA -k -o $@ ../src/*.c
	chmod -w $@
