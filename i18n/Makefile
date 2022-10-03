all: ja_JP.UTF-8/LC_MESSAGES/messages.mo ja_JP.UTF-8/LC_MESSAGES/gamedata.mo

messages.pot: always
	xgettext --omit-header -k_ -o $@ ../src/*.c

gamedata-class.pot: always
	../utils/gamedata2pot class > $@

ja_JP.UTF-8/LC_MESSAGES/messages.po: messages.pot
	msgmerge --update $@ $<

ja_JP.UTF-8/LC_MESSAGES/gamedata.po: gamedata-class.pot
	if [ -f "$@" ]; then \
		msgmerge --update $@ $<; \
	else \
		msginit --no-translator --locale ja_JP.UTF-8 --input $< --output-file $@; \
	fi

%.mo: %.po
	msgfmt $< -o $@

always:
