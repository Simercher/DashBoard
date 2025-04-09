#!/bin/bash

# 跳到你的專案目錄
cd /Users/linyuzhan/Desktop/DashBoard/DriverStation || exit 1

# 清除舊的 build
rm -rf build

# 建立新的 build 資料夾
mkdir build && cd build || exit 1

# 配置與建構
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

# 執行應用程式
./appDriverStation.app/Contents/MacOS/appDriverStation
