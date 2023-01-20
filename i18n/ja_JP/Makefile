# Makefile to bootstrap translations importing Hengband ones.
#
# NOTE: There may be some wrong translations e.g. "He" goes to "勇",
# which is abbreviation of "Hero"

HENGBAND_PATH = ../../../../hengband/hengband
ANGBAND_JA_PATH = ../../../angband-ja

all: \
	LC_MESSAGES/_gamedata-artifact.po \
	LC_MESSAGES/_gamedata-blow_effects.po \
	LC_MESSAGES/_gamedata-blow_methods.po \
	LC_MESSAGES/_gamedata-body.po \
	LC_MESSAGES/_gamedata-class.po \
	LC_MESSAGES/_gamedata-list-equip-slots.po \
	LC_MESSAGES/_gamedata-monster.po \
	LC_MESSAGES/_gamedata-object.po \
	LC_MESSAGES/_gamedata-object_base.po \
	LC_MESSAGES/_gamedata-p_race.po \
	LC_MESSAGES/_gamedata-source.po \
	LC_MESSAGES/_gamedata-terrain.po \
	LC_MESSAGES/messages.po

LC_MESSAGES/messages.po: messages-hengband.po
	msgmerge --no-fuzzy-matching $< $@ -o $@

LC_MESSAGES/_gamedata-%.po: _gamedata-%-hengband.po
	msgmerge --no-fuzzy-matching $< $@ -o $@

messages-hengband.po: _hengband-strings.po
	msgmerge --no-fuzzy-matching $< ../messages.pot -o $@

_hengband-strings.po: $(HENGBAND_PATH)
	perl -nle 'BEGIN { \
		$$version = qx(git -C $(HENGBAND_PATH) describe --tags --always); \
		print q(msgid ""); \
		print q(msgstr ""); \
		print q("Content-Type: text/plain; charset=UTF-8\\n"); \
		print q("Language: ja\\n"); \
	} \
	$$2 ne "\\0" and print qq(# From 変愚蛮怒 $$version#: $$ARGV:$$.\nmsgid "$$2"\nmsgstr "$$1"\n) while /_\(\s*"((?:\\"|[^"])+)"\s*,\s*"((?:\\"|[^"])+)"\s*\)/g; \
	$$. = 0 if eof' \
	$(HENGBAND_PATH)/src/**/*.cpp | msguniq -o $@

.PHONY: clean
clean:
	rm -f messages-hengband.po _hengband-strings.po

_gamedata-monster-hengband.po: $(HENGBAND_PATH)/lib/edit/r_info.txt
	cat ../_gamedata-monster.pot | ../../utils/applytrans --mode=hengband --context=monster $< > $@

_gamedata-object-hengband.po: $(HENGBAND_PATH)/lib/edit/k_info.txt
	cat ../_gamedata-object.pot | ../../utils/applytrans --mode=hengband --context=object $< > $@

_gamedata-artifact-hengband.po: $(HENGBAND_PATH)/lib/edit/a_info.txt
	cat ../_gamedata-artifact.pot | ../../utils/applytrans --mode=hengband --context=artifact $< > $@

_gamedata-terrain-hengband.po: $(HENGBAND_PATH)/lib/edit/f_info.txt
	cat ../_gamedata-terrain.pot | ../../utils/applytrans --mode=hengband --context=terrain $< > $@

_gamedata-artifact-angband_ja.po: $(ANGBAND_JA_PATH)/lib/edit/jp/artifact.txt
	cat ../_gamedata-artifact.pot | ../../utils/applytrans --mode=angband_ja --context=artifact $< > $@
