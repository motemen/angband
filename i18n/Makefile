ja_JP.UTF-8/LC_MESSAGES/messages.mo: ja.po
	msgfmt $< -o $@

messages.pot:
	xgettext -k_ -o $@ ../src/*.c

ja.po:
	msginit --no-translator --locale ja --input messages.pot

