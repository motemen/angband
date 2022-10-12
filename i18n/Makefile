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
		$(LOCALE)/LC_MESSAGES/_gamedata-blow_effects.po \
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
	chmod +w $@ || true
	xgettext --omit-header -k_ -k_C:1c,2 -o $@ ../src/*.c
	chmod -w $@

_gamedata-%.pot:
	chmod +w $@ || true
	../utils/gamedata2pot $* > $@
	chmod -w $@

_gamedata-source.pot:
	chmod +w $@ || true
	xgettext --omit-header -kGAMEDATA_ -kGAMEDATA_N_ -o $@ ../src/list-mon-race-flags.h
	chmod -w $@

hengband.messages.po: hengband.in.po
	msgmerge --previous $< messages.pot -o $@

hengband.in.po:
	perl -nle 'BEGIN { print qq(msgid ""\nmsgstr ""\n"Content-Type: text/plain; charset=UTF-8\\n"\n"Language: ja\\n"\n) } $$2 ne "\\0" and print qq(#: $$ARGV:$$.\nmsgid "$$2"\nmsgstr "$$1"\n) while /_\(\s*"((?:\\"|[^"])+)"\s*,\s*"((?:\\"|[^"])+)"\s*\)/g; $$. = 0 if eof' ../../../hengband/hengband/src/**/*.cpp | msguniq -o $@

.PHONY: merge-messages
merge-messages: hengband.messages.po
	msgcat --lang ja --to-code UTF-8 ja_JP/LC_MESSAGES/messages.po hengband.messages.po -o ja_JP/LC_MESSAGES/messages.po
