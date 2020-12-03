cmake_minimum_required(VERSION 3.12)

# gladという名前でCMakeに認識させるライブラリを作成します。
add_library(glad STATIC
    glad.c
)
