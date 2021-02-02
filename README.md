# OpenGL グラフィックメモ

[![Build Status](https://travis-ci.org/mnrn/ReGL.svg?branch=main)](https://travis-ci.org/mnrn/ReGL)
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
- [stb]
- [tinyobjloader]
- [imgui]
- [fmt]
- [spdlog]
- [boost]

リポジトリのルートディレクトリにCMakeLists.txtがあるので詳しくはそちらを参照ください。  

## Features

### 物理ベースレンダリング(Physically Based Rendering)

![PBR](https://github.com/mnrn/ReGL/blob/main/Docs/Images/pbr.png)

BRDFによるMicrofacet Modelの描画を行っています。  
このあたりの理論はGoogleの物理ベースレンダリングエンジンFilamentのドキュメントなどを参考にしました。  
左が金属(Metallic Material)、右が非金属(Dielectric Material)です。

---

### 遅延レンダリング(Deferred Rendering)

![Deferred](https://github.com/mnrn/ReGL/blob/main/Docs/Images/deferred.png)

ゲームではあまり使われないかもしれません。  
ライティングを多用する場合は候補に入れても良いかもしれません。

---

### スクリーンスペースアンビエントオクルージョン(SSAO)

![SSAO](https://github.com/mnrn/ReGL/blob/main/Docs/Images/ssao.png)

アンビエントオクルージョン(環境光遮蔽)をスクリーン空間上で考えて計算するシェーディング方法です。  
Deferred Renderingで実装しましたが、Forward Renderingでも用いることは可能です。

![SSAOOnly](https://github.com/mnrn/ReGL/blob/main/Docs/Images/ssao_only.png)

SSAOシーンのみのレンダリング

---

### シャドウマップ

![ShadowMap](https://github.com/mnrn/ReGL/blob/main/Docs/Images/shadowmap.png)

1024x1024の解像度のシャドウマップです。  

---

### 平行分割シャドウマップ

![PSSM](https://github.com/mnrn/ReGL/blob/main/Docs/Images/pssm.png)

カメラの視錐台を分割して各視錐台に対してシャドウマップを適用し、影を描画する方法です。  
実装していて気づきましたが、パラメータによってだいぶちらつきが発生するため、バウンディングスフィアによる安定化を採用しました。

![PSSMIndicator3](https://github.com/mnrn/ReGL/blob/main/Docs/Images/pssm_indicator3.png)

視錐台を3分割にしてシャドウマップを生成した場合です。  
手前のほうがくっきりしているのがわかると思います。

---

### コンピュートパーティクル

![ComputeParticles](https://github.com/mnrn/ReGL/blob/main/Docs/Images/compute_particles.png)

コンピュートシェーダーによるパーティクルと重力場のシミュレートになります。  
コンピュートシェーダーを使用しているのでOpenGL4.3以降が必要になるためMacOSでは動作しないことに注意してください。

## 参考

[OpenGL 4 Shading Language Cookbook - Third Edition](https://www.packtpub.com/product/opengl-4-shading-language-cookbook-third-edition/9781789342253)  
[HLSL Development Cookbook](https://www.packtpub.com/product/hlsl-development-cookbook/9781849694209)  
[Unity 2018 Shaders and Effects Cookbook - Third Edition](https://www.packtpub.com/product/unity-2018-shaders-and-effects-cookbook-third-edition/9781788396233)  
[Physically Based Rendering in Filament](https://google.github.io/filament/Filament.md.html)  
[Advanced-Lighting - SSAO](https://learnopengl.com/Advanced-Lighting/SSAO)  
[Cascaded Shadow Maps](https://developer.download.nvidia.com/SDK/10.5/opengl/src/cascaded_shadow_maps/doc/cascaded_shadow_maps.pdf)  
[GPU Gems3 Chapter 10. Parallel-Split Shadow Maps on Programmable GPUs](https://developer.nvidia.com/gpugems/gpugems3/part-ii-light-and-shadows/chapter-10-parallel-split-shadow-maps-programmable-gpus)

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
