# OpenGL グラフィックメモ

グラフィックに関する実装をOpenGLで行っていきます。  
Vulkanでの実装がもう少し楽になりそうならそちらに移行するつもりですが、しばらくはOpenGLのままになりそうです。  
新しくPC買ってなおかつ時間があるようであればVulkanに移行したいです。ある程度目途はついています。

## 制作環境

もともとLinuxで動かしていたプログラムをMacでも保守できるようにして、その後Windowsへ移植しました。  
Linuxを動かしていたPCが故障してしまったので今はWindowsのみの動作確認になります。  
ただし、MacOSでもOpenGL4.2以降が必要なプログラムが動かないだけで他は動きます。

## ビルド

ビルドツールはCMakeになります。  
グラフィックライブラリはOpenGLで4.3以降がターゲットになります。  
ただし現在使用しているグラフィックボードのスペックなどの諸事情により4.4までの確認になります。

以下はリポジトリにすでにおいてあるのでインストールがされてなくても大丈夫です。

- [GLFW]
  - OpenGLやOpenGL ES, Vulkan用のWindowやInputをどうこうしてくれるライブラリです。
- [glad]
  - OpenGL用のローダーです。
- [GLM]
  - グラフィック用の数学ライブラリ(ヘッダオンリー)です。
- [boost]
  - C++の時期標準ライブラリ。今回はあまり使用していないため依存しない場合も考慮してもいいかもしれません。
- [fmt]
  - C++20のstd::formatの代用になります。今回はヘッダオンリーにしています。
- [stb]
  - 今回使用しているのは画像のローダーになります。

リポジトリのルートディレクトリにCMakeLists.txtがあるので詳しくはそちらを参照ください。  

[boost]:<https://www.boost.org/>
[GLFW]:<https://www.glfw.org/>
[glad]:<https://github.com/Dav1dde/glad>
[GLM]:<https://github.com/g-truc/glm>
[fmt]:<https://github.com/fmtlib/fmt>
[stb]:<https://github.com/nothings/stb>
