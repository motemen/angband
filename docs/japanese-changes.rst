日本語化フォークの変更点
========================

このリポジトリは Angband 本家から派生し、日本語化を含む国際化のための拡張を含んでいます。主な変更点は以下の通りです。

* `i18n/` ディレクトリと gettext のテンプレート／辞書を追加し、
  `messages.pot` や `_gamedata-*.pot` から各ロケールの `*.po` を生成する構成。
* ビルド時に `LOCALE` を指定すると `-DUSE_LOCALE` や `-DUSE_DOUBLEWIDTH` が
  有効化され、libintl へのリンクが追加されるなど、ロケール対応のビルドフラグを導入。
* `src/i18n.h` で `_`, `_C`, `_GAMEDATA` などのマクロや `MAX_CHAR_VISUAL_WIDTH`
  を定義し、gettext を経由したメッセージ取得と全角幅の考慮を実装。
* `src/i18n.c` は二倍幅文字判定とテキストの視覚幅計算を提供し、
  ワイド文字を含む翻訳メッセージのレイアウト調整を可能に。
* 初期化処理では `ANGBAND_DIR_I18N` パスが構築され、
  `bindtextdomain` と `textdomain` により翻訳ファイルを読み込むよう変更。
* UI 表示は視覚幅を用いた文字位置計算へ改修され、
  メッセージ長やタイトル表示などで `i18n_text_visualwidth` や
  `i18n_visualwidth` を利用。
* 英語特有の複数形や文構造を翻訳可能にするため、
  `ngettext` や `i18n_text_split` を用いた多言語対応を追加。

