# OpenGL グラフィックメモ

[![License: MIT](https://img.shields.io/badge/License-MIT-lightgrey.svg)](https://opensource.org/licenses/MIT)
![Launguage-C++](https://img.shields.io/badge/Language-C%2B%2B-orange)

グラフィックに関する実装をOpenGLで行っていきます。  

Vulkanへの移行を始めていますが、OpenGLでの実装も続けていきます。  
また、統合してしまうことも考えておりますが、今のところ未定になります。

## 制作経緯

もともとはLinuxで動かしていたプログラムですが、PCが壊れたのでMacに移していたものです。  
再入門にあたりWindowsでも動かせるようにしました。  

現在動作確認はWindowsとMacのみになります。  
動作環境にLinuxを近々追加する予定です。  

動作環境にiOSとAndroidに追加するかどうかについては未定になります。

## 制作環境

- OS
  - Windows10
  - Mac OSX Catalina 10.15

## ビルド

ビルドツールはCMakeになります。  
グラフィックライブラリはOpenGLで4.3以降がターゲットになります。  
ただし現在使用しているグラフィックボードのスペックなどの諸事情により4.4までの確認になります。

以下はリポジトリにすでにおいてあるのでインストールがされてなくても大丈夫です。

- [GLFW]
- [glad]
- [GLM]
- [boost]
- [fmt]
- [stb]
- [tinyobjloader]
- [freetype]
- [spdlog]
- [imgui]

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
[spdlog]:<https://github.com/gabime/spdlog>
[imgui]:<https://github.com/ocornut/imgui>
