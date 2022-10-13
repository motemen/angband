.PHONY: merge-messages
merge-hengband-messages: hengband.messages.po
	msgcat --lang ja --to-code UTF-8 LC_MESSAGES/messages.po hengband.messages.po -o LC_MESSAGES/messages.po

hengband.messages.po: hengband.in.po
	msgmerge --previous $< ../messages.pot -o $@

hengband.in.po:
	perl -nle 'BEGIN { print qq(msgid ""\nmsgstr ""\n"Content-Type: text/plain; charset=UTF-8\\n"\n"Language: ja\\n"\n) } $$2 ne "\\0" and print qq(#: $$ARGV:$$.\nmsgid "$$2"\nmsgstr "$$1"\n) while /_\(\s*"((?:\\"|[^"])+)"\s*,\s*"((?:\\"|[^"])+)"\s*\)/g; $$. = 0 if eof' ../../../../hengband/hengband/src/**/*.cpp | msguniq -o $@
