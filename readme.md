# OpenGL グラフィックメモ

グラフィックに関する実装をOpenGLで行っていきます。  
Vulkanでの実装がもう少し楽になりそうならそちらに移行するつもりですが、しばらくはOpenGLのままになりそうです。  
新しくPC買ってなおかつ時間があるようであればVulkanに移行したいです。ある程度目途はついています。

## 制作環境

もともとLinuxで動かしていたプログラムをMacでも保守できるようにして、その後Windowsへ移植しました。  

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
- [tinyobjloader]
  - objファイルの読み込みに使用しました。
  - objファイルの簡潔さを考えると自作でも良かったかもしれませんが、今後の拡張性と保守性などを考えてこちらにしました。
- [freetype]
  - フォントライブラリ。フォントをビットマップデータにして描画しています。
  - LinuxやBSD, iOSやAndroidはこのライブラリを用いているようです。

リポジトリのルートディレクトリにCMakeLists.txtがあるので詳しくはそちらを参照ください。  

## 参考

[OpenGL 4 Shading Language Cookbook - Third Edition](https://www.packtpub.com/product/opengl-4-shading-language-cookbook-third-edition/9781789342253)  
[HLSL Development Cookbook](https://www.packtpub.com/product/hlsl-development-cookbook/9781849694209)  
[Unity 2018 Shaders and Effects Cookbook - Third Edition](https://www.packtpub.com/product/unity-2018-shaders-and-effects-cookbook-third-edition/9781788396233)  
[Physically Based Rendering in Filament](https://google.github.io/filament/Filament.md.html)  

[boost]:<https://www.boost.org/>
[GLFW]:<https://www.glfw.org/>
[glad]:<https://github.com/Dav1dde/glad>
[GLM]:<https://github.com/g-truc/glm>
[fmt]:<https://github.com/fmtlib/fmt>
[stb]:<https://github.com/nothings/stb>
[tinyobjloader]:<https://github.com/tinyobjloader/tinyobjloader>
[freetype]:<https://www.freetype.org/>
