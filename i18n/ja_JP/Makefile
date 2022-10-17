HENGBAND_PATH = ../../../../hengband/hengband

.PHONY: merge-hengband-messages
merge-hengband-messages: hengband.messages.po
	msgcat --lang ja --to-code UTF-8 LC_MESSAGES/messages.po hengband.messages.po -o LC_MESSAGES/messages.po

hengband.messages.po: hengband.in.po
	msgmerge --previous $< ../messages.pot -o $@

hengband.in.po:
	perl -nle 'BEGIN { print qq(msgid ""\nmsgstr ""\n"Content-Type: text/plain; charset=UTF-8\\n"\n"Language: ja\\n"\n) } $$2 ne "\\0" and print qq(#: $$ARGV:$$.\nmsgid "$$2"\nmsgstr "$$1"\n) while /_\(\s*"((?:\\"|[^"])+)"\s*,\s*"((?:\\"|[^"])+)"\s*\)/g; $$. = 0 if eof' ../../../../hengband/hengband/src/**/*.cpp | msguniq -o $@

LC_MESSAGES/_gamedata-%.po: _gamedata-%-hengband.po
	msgmerge $< ../_gamedata-$*.pot > $@

_gamedata-monster-hengband.po: $(HENGBAND_PATH)/lib/edit/r_info.txt
	cat ../_gamedata-monster.pot | ../../utils/applyhengtrans $< > _gamedata-monster-hengband.po

_gamedata-object-hengband.po: $(HENGBAND_PATH)/lib/edit/k_info.txt
	cat ../_gamedata-object.pot | ../../utils/applyhengtrans $< > _gamedata-object-hengband.po

