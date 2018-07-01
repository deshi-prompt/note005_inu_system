# deshi-pro note No004

イヌもどきかわさきロボット
副題 ：SBCによる多脚ロボ制御システムの設計と実装

# CI state

[![Build Status](https://travis-ci.org/deshi-prompt/note005_inu_system.svg?branch=master)](https://travis-ci.org/deshi-prompt/note005_inu_system)

# creating pdf

## requirement

- tool
 - see [here](https://github.com/deshi-prompt/adoc_styles)

- フォント
 - あおぞら明朝（本文） http://blueskis.wktk.so/AozoraMincho/
 - 源真ゴシック(見出し、本文太字など） http://jikasei.me/font/genshin/

## procedure

```bash
> git clone git@github.com:deshi-prompt/note005_inu_system.git
> cd note005_inu_system
> git clone -b v1.0.0_C91 git@github.com:deshi-prompt/adoc_styles.git styles
> ./make.sh
> evince build/index.pdf
```

<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="クリエイティブ・コモンズ・ライセンス" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" /></a><br /><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">deshi-pro note No.001</span> は <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">クリエイティブ・コモンズ 表示 - 非営利 - 継承 4.0 国際 ライセンスの下で提供されています。</a>

